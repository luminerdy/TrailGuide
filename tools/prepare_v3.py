#!/usr/bin/env python3
"""Prepare pinned original-family drivers with a reviewed derived DMA adapter.

All derived files stay in ignored build/v3-0.1.21/. The local ZIP
integrity pin is not a manufacturer's signature. Never uploads or opens serial.
"""
from pathlib import Path
import hashlib
import json
import shutil
import zipfile

ROOT = Path(__file__).resolve().parents[1]
PRIVATE = ROOT / 'build/v3-0.1.21'
ARCHIVE = ROOT / 'build/v3-s3-bringup/ESP32-S3-Touch-LCD-1.85-Demo.zip'
ARCHIVE_SHA256 = 'fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990'
DRIVERS = tuple(sorted({stem + suffix for stem in ('Display_ST77916', 'Touch_CST816', 'TCA9554PWR', 'I2C_Driver', 'LVGL_Driver') for suffix in ('.cpp', '.h')} | {'esp_lcd_st77916.c', 'esp_lcd_st77916.h'}))


def prepare(output=None):
    # Default is the next increment; refuse paths that could erase any accepted fallback evidence.
    private = Path(output).resolve() if output is not None else PRIVATE.resolve()
    protected = tuple((ROOT / f'build/v3-0.1.{n}').resolve() for n in (6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 18, 19))
    protected += tuple((ROOT/name).resolve() for name in ('build/v3-speaker-bench-0.1.15','build/v3-coaching-compass-port/target','build/v3-ui-shake-0.1.19/target','build/v3-shake-calibration-0.1.20/target','build/v3-global-gestures-0.1.21/target'))
    if any(private == root or root in private.parents for root in protected):
        raise ValueError('Installed 1.6/1.7/1.8/1.9/1.10/1.11/1.12/1.13/1.14 and speaker-bench roots and descendants are immutable')
    allowed = PRIVATE.resolve()
    if private != allowed and allowed not in private.parents:
        raise ValueError('Output must be inside ignored build/v3-0.1.21')
    digest = hashlib.sha256(ARCHIVE.read_bytes()).hexdigest()
    if digest != ARCHIVE_SHA256:
        raise ValueError('Manufacturer archive checksum mismatch; refusing preparation')
    sketch = private / 'prepared/trailguide_v3'
    libraries = private / 'libraries'
    config = private / 'config'
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
    for name in ('trailguide_v3.ino', 'ui.h', 'lv_conf.h', 'imu.h', 'model.h', 'gesture.h', 'shake.h', 'window_reveal.h', 'motion_diagnostics.h', 'flight_recorder.h', 'coaching_model.h', 'coaching_ui.h', 'sound_cue.h', 'chime.h', 'chime_tone.h'):
        data = (ROOT/'trailguide_v3'/name).read_bytes()
        (sketch/name).write_bytes(data)
        own_files[name] = hashlib.sha256(data).hexdigest()
    from build_v3_content import render
    generated = render(json.loads((ROOT/'content/cards.json').read_text()))
    (sketch/'cards_generated.h').write_text(generated)
    own_files['cards_generated.h'] = hashlib.sha256(generated.encode()).hexdigest()
    from build_v3_coaching import render as render_coaching
    # Resolve canonical references afresh; never trust an editable generated copy.
    coaching = render_coaching(json.loads((ROOT/'content/coaching.json').read_text()),
                               json.loads((ROOT/'content/cards.json').read_text()))
    (sketch/'coaching_generated.h').write_text(coaching)
    own_files['coaching_generated.h'] = hashlib.sha256(coaching.encode()).hexdigest()
    (config/'lv_conf.h').write_bytes((sketch/'lv_conf.h').read_bytes())
    # Explicit path beats the vendor's src/lv_conf.h without changing vendor code.
    flags = f'-I{config} -DLV_CONF_INCLUDE_SIMPLE -DLV_CONF_PATH={config}/lv_conf.h'
    fqbn = 'esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PartitionScheme=app3M_fat9M_16MB,PSRAM=opi,FlashMode=qio'
    command = ['arduino-cli', 'compile', '--jobs', '2', '--fqbn', fqbn,
               '--libraries', str(libraries), '--build-property', f'compiler.c.extra_flags={flags}',
               '--build-property', f'compiler.cpp.extra_flags={flags}',
               '--build-path', str(private/'compile-cache'), '--output-dir', str(private/'firmware'), str(sketch)]
    manifest = {'archive_sha256': digest, 'sketch': str(sketch), 'vendor_files': files,
                'original_vendor_files': original_files,
                'derived_driver_note': 'Display_ST77916.cpp only: omit unsafe startup color bars and synchronously drain queued color DMA before returning the LVGL buffer; four exact guarded substitutions. Other drivers and LVGL unchanged.',
                'library_files': library_files, 'own_files': own_files, 'libraries': str(libraries),
                'config_flags': flags, 'fqbn': fqbn, 'compile_command': command,
                'motion_diagnostics': {'command': 'MOTION\n', 'prefix': 'TG_MOTION1', 'default': 'off', 'duration_ms': 45000,
                    'observation': 'each fresh timestamp and validity/status/UI gate/phase transition; otherwise 40ms; dropped observations counted',
                    'transport': 'fixed 768-byte pending line/cursor; at most 16 input bytes and one capacity-limited <=256-byte write per service; HWCDC positive 1ms TX/mutex budget at setup and activation (1000Hz RTOS); bounded, not wait-free; no flush',
                    'gates_order': ['pointer', 'scrolling', 'options', 'reveal', 'eligible'],
                    'phases': ['Acquire', 'Ready', 'Turning', 'Inverted', 'Recover'],
                    'reject_bits': {'1': 'invalid', '2': 'age', '4': 'duplicate (UI skips healthy duplicate dispatch)', '8': 'gap', '16': 'nonfinite/clipped axis', '32': 'cooldown', '64': 'cycle timeout'},
                    'numeric_scales': 'e6 values /1000000; e3 values /1000; 2147483647 is nonfinite/conversion-overflow sentinel; integer-only formatting, no float printf',
                    'loss_evidence': 'bad/wait/phase/fire counters and last_bad time:before>after survive dropped lines; end=timeout summary'},
                'recovery_policy': 'unchanged from v3-0.1.6: Recover retains its old origin until fresh eligible stable rest qualifies; same-origin >=0.94 uses existing 600ms, all other directions including opposition use existing 1200ms; existing four-fresh-sample/stability/quality/freshness/cooldown/context gates apply; reacquisition clears old cycle/dwell and never draws. Ready/Turning/Inverted behavior unchanged.',
                'runtime_compatibility': 'unverified; no upload performed'}
    (private / 'preparation.json').write_text(json.dumps(manifest, indent=2) + '\n')
    return manifest


if __name__ == '__main__':
    print(json.dumps(prepare(), indent=2))
