"""Constructor failure cleanup with mocked syscalls; no device/PTY access."""
from contextlib import ExitStack
from copy import deepcopy
import fcntl
from pathlib import Path
import sys
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import v3_flight_recorder as recorder


class PortCleanupTest(unittest.TestCase):
    def test_every_post_open_failure_closes_and_preserves_exception(self):
        for failing in ('flock', 'tcgetattr', 'tcsetattr', 'tcflush'):
            for close_fails in (False, True):
                with self.subTest(failing=failing, close_fails=close_fails), ExitStack() as stack:
                    failure = RuntimeError('setup failed at ' + failing)
                    opened = stack.enter_context(patch.object(recorder.os, 'open', return_value=42))
                    closed = stack.enter_context(patch.object(recorder.os, 'close', side_effect=OSError('close failed') if close_fails else None))
                    mocks = {'flock': stack.enter_context(patch.object(fcntl, 'flock'))}
                    for name in ('tcgetattr', 'tcsetattr', 'tcflush'):
                        mocks[name] = stack.enter_context(patch.object(recorder.termios, name))
                    mocks['tcgetattr'].return_value = [0, 0, 0, 0, 0, 0, [0] * 32]
                    mocks[failing].side_effect = failure
                    with self.assertRaises(RuntimeError) as caught:
                        recorder.Port('/not-a-device')
                    self.assertIs(caught.exception, failure)
                    closed.assert_called_once_with(42)
                    opened.assert_called_once_with('/not-a-device', recorder.os.O_RDWR | recorder.os.O_NOCTTY | recorder.os.O_NONBLOCK)

    def test_success_keeps_lock_and_original_setup_contract(self):
        with ExitStack() as stack:
            stack.enter_context(patch.object(recorder.os, 'open', return_value=42))
            closed = stack.enter_context(patch.object(recorder.os, 'close'))
            lock = stack.enter_context(patch.object(fcntl, 'flock'))
            stack.enter_context(patch.object(recorder.termios, 'tcgetattr', return_value=[1, 1, 1, 1, 1, 1, [0] * 32]))
            configured = stack.enter_context(patch.object(recorder.termios, 'tcsetattr'))
            flushed = stack.enter_context(patch.object(recorder.termios, 'tcflush'))
            port = recorder.Port('/not-a-device')
            self.assertEqual(port.fd, 42)
            self.assertEqual(port.buffer, b'')
            closed.assert_not_called()
            lock.assert_called_once_with(42, fcntl.LOCK_EX | fcntl.LOCK_NB)
            attributes = configured.call_args.args[2]
            self.assertEqual(attributes[:4], [0, 0, recorder.termios.CLOCAL | recorder.termios.CREAD | recorder.termios.CS8, 0])
            self.assertEqual(attributes[4:6], [recorder.termios.B115200] * 2)
            flushed.assert_called_once_with(42, recorder.termios.TCIFLUSH)
            port.close()
            closed.assert_called_once_with(42)

    def test_failed_open_does_not_close_unowned_descriptor(self):
        with patch.object(recorder.os, 'open', side_effect=OSError('open failed')), patch.object(recorder.os, 'close') as closed:
            with self.assertRaises(OSError):
                recorder.Port('/not-a-device')
            closed.assert_not_called()


if __name__ == '__main__':
    unittest.main()
