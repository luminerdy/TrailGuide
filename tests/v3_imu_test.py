"""Host-only QMI8658 behavioral tests with a fake Wire bus; never devices."""
import os
from pathlib import Path
import subprocess
import unittest

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT/'build/v3-s3-bringup/imu'


class ImuHostTest(unittest.TestCase):
    def test_real_owned_wrapper_with_host_only_fake_bus(self):
        OUT.mkdir(parents=True, exist_ok=True)
        (OUT/'tmp').mkdir(exist_ok=True)
        env={**os.environ, 'TMPDIR':str(OUT/'tmp')}
        command=['g++','-std=c++17','-Wall','-Wextra','-Werror',
                 '-fsanitize=address,undefined','-fno-omit-frame-pointer','-g',
                 str(ROOT/'tests/v3_imu_test.cpp'),'-o',str(OUT/'imu-test')]
        subprocess.run(command, check=True, capture_output=True, env=env)
        result=subprocess.run([str(OUT/'imu-test')], capture_output=True, text=True, env=env, timeout=10)
        (OUT/'green-imu.log').write_text(result.stdout+result.stderr)
        self.assertEqual(result.returncode,0,result.stdout+result.stderr)
        self.assertIn('host-only',result.stdout)


if __name__ == '__main__':
    unittest.main()
