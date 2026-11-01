import importlib.util
import os
from pathlib import Path
import struct
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]

class V2UploaderTest(unittest.TestCase):
    def module(self):
        path = ROOT / 'tools/v2_ota_upload.py'
        self.assertTrue(path.exists(), 'C6-safe OTA helper is not implemented')
        spec = importlib.util.spec_from_file_location('v2_ota_tool', path)
        assert spec is not None and spec.loader is not None
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        return mod

    def test_only_c6_application_images_fit_the_existing_slots(self):
        mod = self.module()
        image = bytearray(256)
        image[0] = 0xe9
        struct.pack_into('<H', image, 12, 13)
        struct.pack_into('<I', image, 32, 0xabcd5432)
        mod.validate_image(bytes(image))
        for offset, value in [(0, 0), (12, 0), (32, 0)]:
            bad = bytearray(image); bad[offset] = value
            with self.assertRaises(ValueError): mod.validate_image(bytes(bad))
        with self.assertRaises(ValueError): mod.validate_image(bytes(0x330001))
        with self.assertRaises(ValueError): mod.validate_image(b'')

    def test_status_rejects_malformed_metadata(self):
        mod = self.module()
        from unittest.mock import patch
        with patch('urllib.request.build_opener') as opener:
            response = opener.return_value.open.return_value.__enter__.return_value
            response.read.return_value = b'[]'
            with self.assertRaises(ValueError): mod.get_status('192.0.2.10')
            response.read.return_value = b'{"device":"another-device"}'
            with self.assertRaises(ValueError): mod.get_status('192.0.2.10')

    def test_callback_uses_an_available_port_instead_of_reusing_3232(self):
        mod = self.module()
        self.assertTrue(hasattr(mod, 'callback_port'), 'No safe callback port selection')
        import socket
        port = mod.callback_port('127.0.0.1')
        self.assertTrue(1 <= port <= 65535)
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as check:
            check.bind(('127.0.0.1', port))

    def test_private_password_file_required_without_disclosing_it(self):
        mod = self.module()
        with tempfile.TemporaryDirectory(dir=os.environ.get('TMPDIR')) as folder:
            path = Path(folder) / 'secrets.h'
            path.write_text('#define TRAILGUIDE_OTA_PASSWORD "unit-test-ota-password-only"\n')
            path.chmod(0o600)
            self.assertEqual('unit-test-ota-password-only', mod.read_password(path))
            path.chmod(0o644)
            with self.assertRaises(ValueError): mod.read_password(path)
            path.chmod(0o600); path.write_text('#define OTHER "test"\n')
            with self.assertRaises(ValueError): mod.read_password(path)
