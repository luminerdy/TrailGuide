#!/usr/bin/env python3
"""Bounded V3 RAM flight recorder client. Stdlib only; serial is explicit.

record starts and closes; fetch later needs only --session, not a paced chat.
No DTR/RTS toggling, reset, target compile, network, or data upload.
"""
import argparse
import json
import os
from pathlib import Path
import secrets
import select
import struct
import termios
import tempfile
import time
import zlib

MAGIC=0x31524654
SIZE=40
CAPACITY=8192
CAPACITIES=(2304,8192)
RECORD=struct.Struct('<III6h4H8B')

class ProtocolError(ValueError):
    pass

def require(condition,message):
    if not condition:raise ProtocolError(message)

def fields(line,kind):
    words=line.strip().split()
    require(words[:2]==['TGFR1',kind], 'unexpected frame kind')
    out={}
    for word in words[2:]:
        require(word.count('=')==1,'malformed frame field')
        k,v=word.split('=')
        require(k not in out,'duplicate field')
        try:out[k]=v if k=='hex' else int(v,16 if k in ('crc','fullcrc','magic') else 10)
        except ValueError as e:raise ProtocolError('invalid numeric field') from e
    return out

def validate_chunk(lines,session,token,offset,count,total,fullcrc,capacity=CAPACITY):
    require(0<count<=32 and 0<=offset<total<=capacity<=CAPACITY and offset+count<=total,'invalid chunk bounds')
    require(len(lines)==count+2,'incomplete or duplicated chunk')
    b=fields(lines[0],'BEGIN');e=fields(lines[-1],'END')
    expected={'session':session,'token':token,'offset':offset,'count':count,'total':total,'size':SIZE,'magic':MAGIC,'version':1,'fullcrc':fullcrc}
    require(set(b)==set(expected)|{'crc'},'unexpected begin fields')
    require(all(b[k]==v for k,v in expected.items()),'stale or incompatible begin')
    require(e=={'session':session,'token':token,'offset':offset,'count':count,'crc':b['crc']},'incomplete or mismatched end')
    data=bytearray()
    for i,line in enumerate(lines[1:-1],offset):
        d=fields(line,'DATA')
        require(set(d)=={'session','token','index','hex'} and d['session']==session and d['token']==token and d['index']==i,'stale/duplicated/out-of-order data')
        require(len(d['hex'])==SIZE*2,'wrong record size')
        try:raw=bytes.fromhex(d['hex'])
        except ValueError as e:raise ProtocolError('invalid record hex') from e
        require(len(raw)==SIZE,'non-exact record hex')
        data.extend(raw)
    require(zlib.crc32(data)==b['crc'],'chunk CRC mismatch')
    return bytes(data)

def validate_status(s,frozen=False):
    require(isinstance(s,dict),'status object required')
    keys=('magic','version','size','capacity','session','state','count','crc','reason','start','end','error')
    require(all(isinstance(s.get(k),int) and not isinstance(s[k],bool) for k in keys),'missing status fields')
    require(all(0<=s[k]<=0xffffffff for k in keys),'unsigned status metadata bounds')
    for key,maximum in (('token',0xffffffff),('ready',1)):
        if key in s:require(isinstance(s[key],int) and not isinstance(s[key],bool) and 0<=s[key]<=maximum,'optional status metadata bounds')
    require(s['magic']==MAGIC and s['version']==1 and s['size']==SIZE and s['capacity'] in CAPACITIES,'incompatible RAM ABI')
    require(0<=s['count']<=s['capacity'] and 0<=s['crc']<=0xffffffff and 0<=s['session']<=0xffffffff and s['state'] in (0,1,2),'status bounds')
    if frozen:require(s['state']==2 and s['session']>0 and s['reason'] in (1,2,3),'not a frozen session')
    return s

def validate_capture(c):
    require(isinstance(c,dict) and c.get('format')=='TGFR1','wrong capture format')
    s=validate_status(c.get('status'),True)
    try:raw=bytes.fromhex(c['data_hex'])
    except (KeyError,ValueError,TypeError) as e:raise ProtocolError('invalid capture hex') from e
    require(len(c['data_hex'])==s['count']*SIZE*2 and len(raw)==s['count']*SIZE,'incomplete capture')
    require(zlib.crc32(raw)==s['crc'],'full capture CRC mismatch')
    return raw

def decoded(raw):
    for index,values in enumerate(RECORD.iter_unpack(raw)):
        poll,sample,errors,*v=values
        yield {'index':index,'poll_ms':poll,'sample_ms':sample,'read_errors':errors,
               'raw_acc':v[:3],'raw_gyro':v[3:6],
               'gates_before':v[6],'gates_after':v[7],'reject':v[8],'resets':v[9],
               'phase_before':v[10],'phase_after':v[11],'status0':v[12],
               'flags':v[13],'clipped_axes':v[14],'reset_reason':v[15]}

class Port:
    def __init__(self,path):
        self.fd=os.open(path,os.O_RDWR|os.O_NOCTTY|os.O_NONBLOCK)
        try:
            # Exclusive access prevents a second reader stealing a command/response.
            import fcntl
            fcntl.flock(self.fd,fcntl.LOCK_EX|fcntl.LOCK_NB)
            a=termios.tcgetattr(self.fd)
            a[0]=a[1]=a[3]=0;a[2]=termios.CLOCAL|termios.CREAD|termios.CS8
            a[4]=a[5]=termios.B115200;a[6][termios.VMIN]=0;a[6][termios.VTIME]=0
            termios.tcsetattr(self.fd,termios.TCSANOW,a)
            termios.tcflush(self.fd,termios.TCIFLUSH)
            self.buffer=b''
        except BaseException:
            # Ownership begins at open, even when __init__ cannot return a Port.
            try:os.close(self.fd)
            except OSError:pass  # Never replace the original setup exception.
            raise
    def close(self):os.close(self.fd)
    def send(self,command):
        data=('\n'+command+'\n').encode('ascii');deadline=time.monotonic()+2
        while data:
            require(time.monotonic()<deadline,'command write timeout')
            if select.select([], [self.fd], [], .1)[1]:
                try:n=os.write(self.fd,data)
                except BlockingIOError:continue
                data=data[n:]
    def line(self,deadline):
        while time.monotonic()<deadline:
            if b'\n' in self.buffer:
                line,self.buffer=self.buffer.split(b'\n',1)
                return line.decode('ascii',errors='replace').strip()
            if len(self.buffer)>4096:self.buffer=b''
            if select.select([self.fd],[],[],min(.1,max(0,deadline-time.monotonic())))[0]:
                try:raw=os.read(self.fd,4096)
                except BlockingIOError:continue
                require(bool(raw),'serial reader disconnected')
                self.buffer+=raw
        raise ProtocolError('response timeout; RAM is retained; retry fetch without reset')

def status(port,command='STATUS',timeout=6):
    token=secrets.randbelow(0xffffffff)+1
    port.send(f'{command} {token}')
    deadline=time.monotonic()+timeout
    while True:
        line=port.line(deadline)
        if not line.startswith('TGFR1 STATUS '):continue
        try:s=fields(line,'STATUS')
        except ProtocolError:continue
        if s.get('token')!=token:continue
        validate_status(s)
        if command!='STATUS':
            errors={1:'recording busy; RAM unchanged',2:'PSRAM unavailable/allocation failed; recording not started',3:'invalid dump bounds/session or recording not frozen'}
            require(s['error']==0,f"device refused command: {errors.get(s['error'],'unknown error')} (error={s['error']} state={s['state']})")
        return s

def freeze(port,session,retries=3):
    require(isinstance(session,int) and not isinstance(session,bool) and 0<session<=0xffffffff,'nonzero uint32 session required')
    require(isinstance(retries,int) and 1<=retries<=10,'invalid retry bound')
    for attempt in range(retries):
        try:s=status(port,f'FREEZE {session}')
        except ProtocolError:
            # Query after a lost ACK; never re-arm or change the requested session.
            s=status(port)
            require(s['session']==session,'wrong session; freeze not confirmed')
            if s['state']==1 and attempt+1<retries:continue
        require(s['session']==session and s['error']==0,'freeze refused or wrong session')
        validate_status(s,True)
        return s
    raise ProtocolError('freeze not confirmed; RAM retained')

def duration_ms(s):
    require(s['capacity'] in CAPACITIES,'unknown capture duration')
    return 45000 if s['capacity']==2304 else 120000

def default_wait(s):
    return duration_ms(s)/1000+15

def fetch(port,session,wait=None,retries=3):
    if wait is not None:require(isinstance(wait,(float,int)) and 0<=wait<=600,'wait must be finite and between 0 and 600 seconds')
    deadline=None
    while True:
        s=status(port)
        if deadline is None:deadline=time.monotonic()+(default_wait(s) if wait is None else wait)
        require(s['session']==session,'wrong session; refusing stale/overwritten capture')
        if s['state']==2:break
        require(s['state']==1 and time.monotonic()<deadline,'session not frozen before wait deadline')
        time.sleep(.5)
    validate_status(s,True)
    raw=bytearray()
    for offset in range(0,s['count'],32):
        count=min(32,s['count']-offset)
        for attempt in range(retries):
            token=secrets.randbelow(0xffffffff)+1
            port.send(f'DUMP {session} {offset} {count} {token}')
            lines=[];end=time.monotonic()+8
            try:
                while True:
                    line=port.line(end)
                    if not line.startswith('TGFR1 '):continue
                    kind=line.split()[1]
                    if kind not in ('BEGIN','DATA','END'):continue
                    f=fields(line,kind)
                    if f.get('session')!=session or f.get('token')!=token:continue
                    if kind=='BEGIN':require(not lines,'duplicate chunk begin')
                    require(bool(lines) or kind=='BEGIN','data before begin')
                    lines.append(line)
                    require(len(lines)<=count+2,'oversize chunk')
                    if kind=='END':break
                raw.extend(validate_chunk(lines,session,token,offset,count,s['count'],s['crc'],capacity=s['capacity']))
                break
            except ProtocolError:
                if attempt+1==retries:raise
                # Fresh token rejects any delayed bytes from the failed request.
        else:raise ProtocolError('chunk retries exhausted')
    c={'format':'TGFR1','status':s,'data_hex':raw.hex()}
    validate_capture(c)
    final=status(port)
    require(all(final[k]==s[k] for k in ('magic','version','size','capacity','session','count','crc','state','start','end','reason','error')),'session changed during retrieval')
    return c

def save_capture(path,c):
    validate_capture(c)
    p=Path(path);p.parent.mkdir(parents=True,exist_ok=True)
    # Never leave a final-looking incomplete capture or overwrite prior evidence.
    temporary=None
    try:
        with tempfile.NamedTemporaryFile(mode='w',dir=p.parent,prefix='.'+p.name+'.partial-',delete=False) as f:
            temporary=Path(f.name);json.dump(c,f);f.write('\n');f.flush();os.fsync(f.fileno())
        os.link(temporary,p)  # atomic publish, O_EXCL semantics; no prior evidence overwritten
    finally:
        if temporary is not None:temporary.unlink(missing_ok=True)

def main():
    p=argparse.ArgumentParser(description=__doc__)
    sub=p.add_subparsers(dest='command',required=True)
    for name in ('record','status','clear','fetch','freeze'):
        q=sub.add_parser(name);q.add_argument('--port',required=True)
        if name in ('fetch','freeze'):q.add_argument('--session',type=int,required=True)
        if name=='fetch':
            q.add_argument('--output',required=True);q.add_argument('--wait',type=float,default=None)
    q=sub.add_parser('verify');q.add_argument('capture');q.add_argument('--jsonl')
    q=sub.add_parser('import-ram');q.add_argument('raw');q.add_argument('--status',required=True);q.add_argument('--output',required=True)
    a=p.parse_args()
    if a.command=='verify':
        c=json.loads(Path(a.capture).read_text());raw=validate_capture(c)
        if a.jsonl:
            with open(a.jsonl,'x') as f:
                for row in decoded(raw):f.write(json.dumps(row)+'\n')
        print(f"VERIFIED session={c['status']['session']} records={len(raw)//SIZE} bytes={len(raw)} crc32={zlib.crc32(raw):08x}")
        return
    if a.command=='import-ram':
        s=json.loads(Path(a.status).read_text());raw=Path(a.raw).read_bytes()
        save_capture(a.output,{'format':'TGFR1','status':s,'data_hex':raw.hex()});return
    port=Port(a.port)
    try:
        if a.command=='fetch':
            c=fetch(port,a.session,a.wait);save_capture(a.output,c)
            print(f"VERIFIED session={a.session} records={c['status']['count']} saved={a.output}")
        elif a.command=='freeze':print(json.dumps(freeze(port,a.session)))
        elif a.command=='record':
            before=status(port);require(before['state']!=1,'already recording; use status/fetch, not restart')
            # A lost start ACK must not silently start a second recording.
            try:s=status(port,'RECORD')
            except ProtocolError:
                s=status(port)
                require(s['session']!=before['session'] and s['state'] in (1,2),'record start not confirmed')
            require(s['session']!=before['session'] and s['state'] in (1,2),'record start not confirmed')
            deadline=time.monotonic()+2
            while s['state']==1 and s['count']==0 and time.monotonic()<deadline:
                time.sleep(.05);s=status(port)
            require(s['count']>0,'recorder active but no actual IMU polls observed; inspect status; do not reset')
            print(json.dumps(s));print(f"READY session={s['session']} duration_ms={duration_ms(s)}; reader may close now",flush=True)
        else:print(json.dumps(status(port,'CLEAR' if a.command=='clear' else 'STATUS')))
    finally:port.close()

if __name__=='__main__':
    try:main()
    except (ProtocolError,OSError,ValueError) as e:raise SystemExit(f'NOT ACCEPTED: {e}')
