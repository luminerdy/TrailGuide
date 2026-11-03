"""Public fixtures are SYNTHETIC zeros/integers only, never human traces."""
import importlib.util
from pathlib import Path
import sys
import unittest
import zlib
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))

class Protocol(unittest.TestCase):
 def test_actual_manual_cpp_header_rows_crc(self):
  from v3_flight_recorder import fields,validate_chunk,validate_capture,decoded
  path=ROOT/'build/v3-0.1.12/recorder.manual.synthetic.wire'
  if not path.exists():self.skipTest('run focused C++ recorder test first')
  lines=[s for s in path.read_text().splitlines() if s]
  s=fields(lines.pop(0),'STATUS');self.assertEqual(s['reason'],3);self.assertEqual(s['capacity'],8192)
  raw=validate_chunk(lines,77,905,0,1,1,s['crc'],capacity=s['capacity'])
  self.assertEqual(validate_capture(dict(format='TGFR1',status=s,data_hex=raw.hex())),raw)
  self.assertEqual(list(decoded(raw))[0]['poll_ms'],51)
 def test_missing_tool(self):
  self.assertTrue((ROOT/'tools/v3_flight_recorder.py').exists(),'exact-validating capture utility missing')
 def test_roundtrip_corrupt_incomplete_stale_and_retry(self):
  if not (ROOT/'tools/v3_flight_recorder.py').exists():self.fail('exact-validating capture utility missing')
  from v3_flight_recorder import validate_chunk, validate_capture, ProtocolError
  data=bytes(range(40));crc=zlib.crc32(data)
  begin=f'TGFR1 BEGIN session=9 token=7 offset=0 count=1 total=1 size=40 magic=31524654 version=1 crc={crc:08x} fullcrc={crc:08x}'
  row=f'TGFR1 DATA session=9 token=7 index=0 hex={data.hex()}'
  end=f'TGFR1 END session=9 token=7 offset=0 count=1 crc={crc:08x}'
  lines=[begin,row,end]
  self.assertEqual(validate_chunk(lines,9,7,0,1,1,crc),data)
  for bad in [lines[:-1],[begin,end],[begin,row,row,end],[begin,row.replace('hex=00','hex=01'),end],[begin.replace('version=1','version=2'),row,end],[begin,row,end.replace('count=1','count=2')],[begin,row.replace('index=0','index=1'),end]]:
   with self.assertRaises(ProtocolError):validate_chunk(bad,9,7,0,1,1,crc)
  # A prior request's valid chunk is NOT the current transaction.
  with self.assertRaises(ProtocolError):validate_chunk(lines,9,8,0,1,1,crc)
  self.assertEqual(validate_chunk(lines,9,7,0,1,1,crc),data)
  status={'magic':0x31524654,'version':1,'size':40,'capacity':2304,'session':9,'state':2,'count':1,'crc':crc,'reason':1,'start':0,'end':45000,'error':0}
  capture={'format':'TGFR1','status':status,'data_hex':data.hex()}
  self.assertEqual(validate_capture(capture),data)
  for field,value in [('count',2),('crc',crc^1),('state',1),('size',44),('magic',0)]:
   with self.assertRaises(ProtocolError):validate_capture({**capture,'status':{**status,field:value}})
 def test_actual_cpp_writer_and_decoder(self):
  from v3_flight_recorder import fields,validate_chunk,decoded
  path=ROOT/'build/v3-0.1.12/recorder.synthetic.wire'
  if not path.exists():self.skipTest('run tools/test_v3_app.py first for C++ writer artifact')
  lines=[s for s in path.read_text().splitlines() if s]
  begin=fields(lines[0],'BEGIN')
  raw=validate_chunk(lines,1,9876,0,1,1,begin['fullcrc'])
  row=list(decoded(raw))[0]
  self.assertEqual(row['raw_gyro'],[0,0,32767]);self.assertEqual(row['clipped_axes'],32)
  self.assertEqual(row['poll_ms'],20);self.assertEqual(row['flags'],64)
 def test_fetch_retries_corrupt_chunk_and_ignores_stale_tokens(self):
  from v3_flight_recorder import fetch,ProtocolError
  data=bytes(range(40));crc=zlib.crc32(data)
  s={'magic':0x31524654,'version':1,'size':40,'capacity':2304,'session':9,'state':2,'count':1,'crc':crc,'reason':1,'start':0,'end':45000,'error':0}
  class SyntheticPort:
   def __init__(self):self.lines=[];self.dumps=0
   def send(self,command):
    w=command.split();token=int(w[-1])
    if w[0]=='STATUS':
     text=' '.join(f'{k}={v:08x}' if k in ('crc','magic') else f'{k}={v}' for k,v in s.items())
     self.lines += [f'TGFR1 STATUS token=0 {text}',f'TGFR1 STATUS token={token} {text}']
    else:
     self.dumps+=1
     self.lines += [f'TGFR1 END session=8 token=0 offset=0 count=1 crc={crc:08x}',f'TGFR1 BEGIN session=9 token={token} offset=0 count=1 total=1 size=40 magic=31524654 version=1 crc={crc:08x} fullcrc={crc:08x}',f'TGFR1 DATA session=9 token={token} index=0 hex={(bytes(40) if self.dumps==1 else data).hex()}',f'TGFR1 END session=9 token={token} offset=0 count=1 crc={crc:08x}']
   def line(self,deadline):
    if not self.lines:raise ProtocolError('synthetic incomplete response')
    return self.lines.pop(0)
  port=SyntheticPort();c=fetch(port,9);self.assertEqual(c['data_hex'],data.hex());self.assertEqual(port.dumps,2)
  with self.assertRaises(ProtocolError):fetch(SyntheticPort(),10)
 def test_new_and_legacy_capacity_manual_reason_and_bounds(self):
  from v3_flight_recorder import validate_status,validate_capture,validate_chunk,ProtocolError
  for capacity in (2304,8192):
   data=bytes(40);crc=zlib.crc32(data)
   s=dict(magic=0x31524654,version=1,size=40,capacity=capacity,session=9,state=2,count=1,crc=crc,reason=3,start=0,end=30,error=0)
   self.assertEqual(validate_capture(dict(format='TGFR1',status=s,data_hex=data.hex())),data)
   with self.assertRaises(ProtocolError):validate_status({**s,'count':capacity+1})
   offset=capacity-1
   lines=[f'TGFR1 BEGIN session=9 token=7 offset={offset} count=1 total={capacity} size=40 magic=31524654 version=1 crc={crc:08x} fullcrc={crc:08x}',f'TGFR1 DATA session=9 token=7 index={offset} hex={data.hex()}',f'TGFR1 END session=9 token=7 offset={offset} count=1 crc={crc:08x}']
   self.assertEqual(validate_chunk(lines,9,7,offset,1,capacity,crc,capacity=capacity),data)
   with self.assertRaises(ProtocolError):validate_chunk(lines,9,7,offset,1,capacity,crc,capacity=2304 if capacity==8192 else 1)
  with self.assertRaises(ProtocolError):validate_status({**s,'capacity':8193})
 def test_freeze_lost_ack_retries_same_session_and_timeout_race(self):
  from v3_flight_recorder import freeze,ProtocolError
  for race in (False,True,'unexecuted'):
   class Port:
    def __init__(self):self.commands=[];self.lines=[];self.attempts=0;self.state=1
    def send(self,command):
     self.commands.append(command);w=command.split();token=int(w[-1])
     if w[0]=='FREEZE':
      self.attempts+=1
      if self.attempts==1 and race=='unexecuted':return
      self.state=2
      if self.attempts==1:return
     text=f'magic=31524654 version=1 size=40 capacity=8192 session=9 state={self.state} count=1 crc=00000000 reason={1 if race is True else 3} start=0 end=120000 error=0'
     self.lines.append(f'TGFR1 STATUS token={token} {text}')
    def line(self,deadline):
     if not self.lines:raise ProtocolError('synthetic lost ACK')
     return self.lines.pop(0)
   port=Port();s=freeze(port,9);self.assertEqual(s['reason'],1 if race is True else 3)
   self.assertTrue(all(c.startswith(('FREEZE 9 ','STATUS ')) for c in port.commands))
   self.assertNotIn('RECORD',' '.join(port.commands))
   self.assertEqual(port.attempts,2 if race=='unexecuted' else 1)
   with self.assertRaises(ProtocolError):freeze(Port(),0)
 def test_wait_and_ready_duration_follow_advertised_capacity(self):
  from v3_flight_recorder import duration_ms,default_wait,fetch,ProtocolError
  for cap,duration in ((2304,45000),(8192,120000)):
   self.assertEqual(duration_ms({'capacity':cap}),duration)
   self.assertEqual(default_wait({'capacity':cap}),duration/1000+15)
  for wait in (-1,float('nan'),float('inf')):
   with self.assertRaises(ProtocolError):fetch(None,9,wait=wait)
 def test_ready_and_freeze_cli_without_hardware(self):
  import v3_flight_recorder as client
  from unittest.mock import patch
  from contextlib import redirect_stdout
  from io import StringIO
  for capacity,duration in ((2304,45000),(8192,120000)):
   base=dict(magic=0x31524654,version=1,size=40,capacity=capacity,session=9,state=0,count=0,crc=0,reason=0,start=0,end=0,error=0)
   output=StringIO()
   with patch.object(sys,'argv',['recorder','record','--port','SYNTHETIC']),patch.object(client,'Port'),patch.object(client,'status',side_effect=[base,{**base,'session':10,'state':1,'count':1}]),redirect_stdout(output):client.main()
   self.assertIn(f'READY session=10 duration_ms={duration}',output.getvalue())
  with patch.object(sys,'argv',['recorder','freeze','--port','SYNTHETIC','--session','9']),patch.object(client,'Port'),patch.object(client,'freeze',return_value={'session':9}) as freeze,redirect_stdout(StringIO()):
   client.main();self.assertEqual(freeze.call_args.args[1],9)
 def test_unsigned_metadata_rejected_before_capture_publication(self):
  from v3_flight_recorder import validate_status,validate_capture,save_capture,ProtocolError
  import tempfile
  for capacity in (2304,8192):
   data=bytes(40);crc=zlib.crc32(data)
   base=dict(magic=0x31524654,version=1,size=40,capacity=capacity,session=9,state=2,count=1,crc=crc,reason=3,start=0,end=30,error=0,token=1,ready=1)
   with tempfile.TemporaryDirectory() as tmp:
    target=Path(tmp)/'must-not-exist.json'
    for field in ('start','end','error','reason','token'):
     for value in (-1,4294967296,True,1.5):
      with self.subTest(capacity=capacity,field=field,value=value):
       bad={**base,field:value};capture=dict(format='TGFR1',status=bad,data_hex=data.hex())
       with self.assertRaises(ProtocolError):validate_status(bad)
       with self.assertRaises(ProtocolError):validate_capture(capture)
       with self.assertRaises(ProtocolError):save_capture(target,capture)
       self.assertFalse(target.exists())
    for value in (-1,2,True,1.5):
     with self.assertRaises(ProtocolError):validate_status({**base,'ready':value})
    rollover={**base,'session':4294967295,'start':4294967295,'end':0,'error':4294967295,'token':4294967295}
    self.assertEqual(validate_capture(dict(format='TGFR1',status=rollover,data_hex=data.hex())),data)
 def test_final_header_cannot_change_capture_metadata(self):
  from v3_flight_recorder import fetch,ProtocolError
  for changed in ('capacity','start','end','reason','error'):
   class Port:
    calls=0
    def send(self,command):
     self.calls+=1;token=command.split()[-1]
     s=dict(magic='31524654',version=1,size=40,capacity=8192,session=9,state=2,count=0,crc='00000000',reason=3,start=10,end=20,error=0)
     if self.calls==2:s[changed]=dict(capacity=2304,start=11,end=21,reason=1,error=3)[changed]
     self.response=f'TGFR1 STATUS token={token} '+ ' '.join(f'{k}={v}' for k,v in s.items())
    def line(self,deadline):return self.response
   with self.assertRaises(ProtocolError,msg=changed):fetch(Port(),9)
if __name__=='__main__':unittest.main()
