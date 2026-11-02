#!/usr/bin/env python3
"""Prepare pinned original-family drivers with a reviewed derived DMA adapter.

All derived files stay in ignored build/v3-s3-bringup/minimal. The local ZIP
integrity pin is not a manufacturer's signature. Never uploads or opens serial.
"""
from pathlib import Path
import hashlib
import json
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PRIVATE = ROOT / 'build/v3-s3-bringup/minimal'
ARCHIVE = ROOT / 'build/v3-s3-bringup/ESP32-S3-Touch-LCD-1.85-Demo.zip'
ARCHIVE_SHA256 = 'fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990'
DRIVERS = tuple(sorted({stem + suffix for stem in ('Display_ST77916', 'Touch_CST816', 'TCA9554PWR', 'I2C_Driver', 'LVGL_Driver') for suffix in ('.cpp', '.h')} | {'esp_lcd_st77916.c', 'esp_lcd_st77916.h'}))


def prepare():
    digest = hashlib.sha256(ARCHIVE.read_bytes()).hexdigest()
    if digest != ARCHIVE_SHA256:
        raise ValueError('Manufacturer archive checksum mismatch; refusing preparation')
    sketch = PRIVATE / 'prepared/trailguide_v3_bringup'
    libraries = PRIVATE / 'libraries'
    config = PRIVATE / 'config'
    for directory in (sketch, libraries, config):
        if directory.exists():
            shutil.rmtree(directory)
        directory.mkdir(parents=True)
    files = {}
    original_files = {}
    library_files = {}
    with zipfile.ZipFile(ARCHIVE) as archive:
        # Each read verifies ZIP CRC as well as the complete archive SHA-256 above.
        for name in DRIVERS:
            data = archive.read('Arduino/examples/LVGL_Arduino/' + name)
            original_files[name] = hashlib.sha256(data).hexdigest()
            if name == 'Display_ST77916.cpp':
                # Derive one reviewed adapter; original archive/extraction stay immutable.
                # IDF tx_param(-1,NULL,0) drains queued color DMA without sending a command.
                patches = (
                    (b'  test_draw_bitmap(panel_handle);\r\n',
                     b'  // TrailGuide: omit unsafe asynchronous startup color bars.\r\n'),
                    (b'esp_lcd_panel_handle_t panel_handle = NULL;\r\n',
                     b'esp_lcd_panel_handle_t panel_handle = NULL;\r\nstatic esp_lcd_panel_io_handle_t trailguide_panel_io = NULL;\r\n'),
                    (b'  esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle);\r\n',
                     b'  ESP_ERROR_CHECK(esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle));\r\n  trailguide_panel_io = io_handle;\r\n'),
                    (b'  esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color);',
                     b'  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color));\r\n  ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(trailguide_panel_io, -1, NULL, 0));'),
                )
                for old, new in patches:
                    if data.count(old) != 1:
                        raise ValueError('Unknown display source; refusing DMA safety patch')
                    data = data.replace(old, new)
            (sketch / name).write_bytes(data)
            files[name] = hashlib.sha256(data).hexdigest()
        prefix = 'Arduino/libraries/lvgl/'
        for name in sorted(archive.namelist()):
            if not name.startswith(prefix) or name.endswith('/'):
                continue
            relative = name[len(prefix):]
            if not (relative.startswith('src/') or relative in ('lvgl.h', 'library.properties', 'LICENCE.txt', 'demos/lv_demos.h')):
                continue
            data = archive.read(name)
            destination = libraries / 'lvgl' / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            destination.write_bytes(data)
            library_files[relative] = hashlib.sha256(data).hexdigest()
    own_files = {}
    for name in ('trailguide_v3_bringup.ino', 'ui.h', 'lv_conf.h'):
        data = (ROOT/'trailguide_v3_bringup'/name).read_bytes()
        (sketch/name).write_bytes(data)
        own_files[name] = hashlib.sha256(data).hexdigest()
    (config/'lv_conf.h').write_bytes((sketch/'lv_conf.h').read_bytes())
    # Explicit path beats the vendor's src/lv_conf.h without changing vendor code.
    flags = f'-I{config} -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH={config}/lv_conf.h'
    fqbn = 'esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
    command = ['arduino-cli', 'compile', '--jobs', '2', '--fqbn', fqbn,
               '--libraries', str(libraries), '--build-property', f'compiler.c.extra_flags={flags}',
               '--build-property', f'compiler.cpp.extra_flags={flags}',
               '--build-path', str(PRIVATE/'compile-cache'), '--output-dir', str(PRIVATE/'firmware'), str(sketch)]
    manifest = {'archive_sha256': digest, 'sketch': str(sketch), 'vendor_files': files,
                'original_vendor_files': original_files,
                'derived_driver_note': 'Display_ST77916.cpp only: omit unsafe startup color bars and synchronously drain queued color DMA before returning the LVGL buffer; four exact guarded substitutions. Other drivers and LVGL unchanged.',
                'library_files': library_files, 'own_files': own_files, 'libraries': str(libraries),
                'config_flags': flags, 'fqbn': fqbn, 'compile_command': command,
                'runtime_compatibility': 'unverified; no upload performed'}
    (PRIVATE / 'preparation.json').write_text(json.dumps(manifest, indent=2) + '\n')
    return manifest


if __name__ == '__main__':
    print(json.dumps(prepare(), indent=2))
