"""Public fixtures are SYNTHETIC zeros/integers only, never human traces."""
import importlib.util
from pathlib import Path
import sys
import unittest
import zlib
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))

class Protocol(unittest.TestCase):
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
  path=ROOT/'build/v3-0.1.8/recorder.synthetic.wire'
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
if __name__=='__main__':unittest.main()
