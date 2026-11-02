"""Offline preparation contract; no serial or network access."""
import hashlib
import importlib.util
from pathlib import Path
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
PRIVATE = ROOT / 'build/v3-s3-bringup/minimal'
DRIVERS = {stem + suffix for stem in ('Display_ST77916', 'Touch_CST816', 'TCA9554PWR', 'I2C_Driver', 'LVGL_Driver') for suffix in ('.cpp', '.h')} | {'esp_lcd_st77916.c', 'esp_lcd_st77916.h'}

class PreparationTest(unittest.TestCase):
    def test_exact_pinned_offline_preparation(self):
        helper = ROOT / 'tools/prepare_v3_bringup.py'
        self.assertTrue(helper.exists(), 'Missing checksum-pinned minimal preparation helper')
        spec = importlib.util.spec_from_file_location('prepare_v3', helper)
        assert spec and spec.loader
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        manifest = mod.prepare()
        self.assertEqual(manifest['archive_sha256'], 'fa6cd4c47923d559e15571a20ec0d7029febee76e14e6afbd99760f9ad5d6990')
        sketch = Path(manifest['sketch'])
        own = {'trailguide_v3_bringup.ino', 'ui.h', 'lv_conf.h'}
        self.assertEqual({p.name for p in sketch.iterdir()}, DRIVERS | own)
        for name in own:
            self.assertEqual((sketch/name).read_bytes(), (ROOT/'trailguide_v3_bringup'/name).read_bytes())
        library = Path(manifest['libraries'])/'lvgl'
        self.assertIn('version=8.3.10', (library/'library.properties').read_text())
        self.assertEqual({p.name for p in Path(manifest['libraries']).iterdir()}, {'lvgl'})
        self.assertEqual([p.name for p in (library/'demos').iterdir()], ['lv_demos.h'])
        self.assertFalse((library/'examples').exists())
        flags = manifest['config_flags']
        self.assertIn('LV_CONF_PATH=', flags)
        self.assertNotIn('/lvgl/src', flags)
        self.assertIn('PartitionScheme=app3M_fat9M_16MB', manifest['fqbn'])
        self.assertIn('FlashMode=qio', manifest['fqbn'])
        self.assertIn('PSRAM=opi', manifest['fqbn'])
        lcd = (sketch/'Display_ST77916.h').read_text()
        touch = (sketch/'Touch_CST816.h').read_text()
        for pin in ('(40)', '(21)', '(46)', '(45)', '(42)', '(41)', '(-1)'):
            self.assertIn(pin, lcd)
        self.assertIn('CST816_RST_PIN        -1', touch)
        self.assertIn('Set_EXIO(EXIO_PIN2,Low)', (sketch/'Display_ST77916.cpp').read_text())
        self.assertIn('Set_EXIO(EXIO_PIN1,Low)', (sketch/'Touch_CST816.cpp').read_text())
        original = ROOT / 'build/v3-s3-bringup/vendor/Arduino/examples/LVGL_Arduino'
        for name in DRIVERS:
            expected = (original/name).read_bytes()
            if name == 'Display_ST77916.cpp':
                expected = expected.replace(b'  test_draw_bitmap(panel_handle);\r\n',
                    b'  // TrailGuide: omit unsafe asynchronous startup color bars.\r\n')
                expected = expected.replace(b'esp_lcd_panel_handle_t panel_handle = NULL;\r\n',
                    b'esp_lcd_panel_handle_t panel_handle = NULL;\r\nstatic esp_lcd_panel_io_handle_t trailguide_panel_io = NULL;\r\n')
                expected = expected.replace(b'  esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle);\r\n',
                    b'  ESP_ERROR_CHECK(esp_lcd_new_panel_st77916(io_handle, &panel_config, &panel_handle));\r\n  trailguide_panel_io = io_handle;\r\n')
                expected = expected.replace(b'  esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color);',
                    b'  ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, Xstart, Ystart, Xend, Yend, color));\r\n  ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(trailguide_panel_io, -1, NULL, 0));')
            self.assertEqual((sketch/name).read_bytes(), expected)
        self.assertNotIn(b'  test_draw_bitmap(panel_handle);', (sketch/'Display_ST77916.cpp').read_bytes(),
                         'Startup must not submit/rewrite/free the asynchronous demo DMA buffer')
        self.assertEqual(manifest, mod.prepare(), 'Prepared output is reproducible')

    def test_actual_window_draw_retains_buffer_until_dma_completion(self):
        # Compile the actual prepared window function against a queued-DMA seam.
        # This checks buffer ownership/order, not real hardware DMA timing.
        import subprocess
        spec = importlib.util.spec_from_file_location('prepare_v3_dma', ROOT/'tools/prepare_v3_bringup.py')
        assert spec and spec.loader
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        manifest = mod.prepare()
        source = (Path(manifest['sketch'])/'Display_ST77916.cpp').read_text()
        start = source.index('void LCD_addWindow(')
        end = source.index('uint8_t LCD_Backlight', start)
        function = source[start:end]
        harness = r'''#include <cassert>
#include <cstdint>
#include <stddef.h>
#define ESP_ERROR_CHECK(expr) assert((expr) == 0)
constexpr int EXAMPLE_LCD_WIDTH=360, EXAMPLE_LCD_HEIGHT=360;
void *panel_handle=reinterpret_cast<void*>(1);
void *trailguide_panel_io=reinterpret_cast<void*>(2);
bool pending=false; unsigned completed=0;
int esp_lcd_panel_draw_bitmap(void*,int,int,int,int,const void*) {
  assert(!pending); pending=true; return 0;
}
int esp_lcd_panel_io_tx_param(void *io,int cmd,const void *param,unsigned size) {
  assert(io==trailguide_panel_io && cmd==-1 && !param && size==0);
  assert(pending); pending=false; ++completed; return 0;
}
'''+function+r'''
int main() {
  for(unsigned i=0;i<100;++i) {
    uint16_t pixels[4]={0x1234,0x5678,0x9abc,0xdef0};
    LCD_addWindow(0,0,1,1,pixels);
    assert(!pending && "LCD_addWindow returned before color DMA completed");
    assert(pixels[0]==0x3412 && pixels[3]==0xf0de);
  }
  assert(completed==100);
}
'''
        cpp=PRIVATE/'dma-window-lifetime-test.cpp'
        exe=PRIVATE/'dma-window-lifetime-test'
        cpp.write_text(harness)
        subprocess.run(['g++','-std=c++17','-Wall','-Wextra','-Werror',str(cpp),'-o',str(exe)],check=True,capture_output=True)
        run=subprocess.run([str(exe)],capture_output=True,text=True)
        self.assertEqual(run.returncode,0,run.stderr)

    def test_bad_archive_rejected_without_changing_prepared_output(self):
        spec = importlib.util.spec_from_file_location('prepare_v3_bad', ROOT/'tools/prepare_v3_bringup.py')
        assert spec and spec.loader
        mod = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(mod)
        mod.prepare()
        marker = PRIVATE/'prepared/trailguide_v3_bringup/ui.h'
        before = marker.read_bytes()
        bad = PRIVATE/'bad-archive-test.zip'
        bad.write_bytes(b'Not the pinned archive')
        try:
            with patch.object(mod, 'ARCHIVE', bad), self.assertRaisesRegex(ValueError, 'checksum mismatch'):
                mod.prepare()
            self.assertEqual(before, marker.read_bytes())
        finally:
            bad.unlink()

    def test_minimal_startup_contract(self):
        path = ROOT / 'trailguide_v3_bringup/trailguide_v3_bringup.ino'
        self.assertTrue(path.exists(), 'Missing S3-only minimal startup')
        source = path.read_text()
        self.assertIn('#if !defined(CONFIG_IDF_TARGET_ESP32S3)', source)
        self.assertNotIn('while (!Serial)', source)
        for forbidden in ('WiFi', 'Wireless', 'Audio', 'MIC_', 'SD_', 'Gyro', 'ESP_SR', 'Preferences', 'EEPROM'):
            self.assertNotIn(forbidden, source)
        self.assertLess(source.index('I2C_Init();'), source.index('TCA9554PWR_Init('))
        self.assertLess(source.index('TCA9554PWR_Init('), source.index('CST816_Touch_Reset();'))
        self.assertLess(source.index('CST816_Touch_Reset();'), source.index('acknowledges(Wire1, CST816_ADDR)'))
        self.assertLess(source.index('LCD_Init();'), source.index('Lvgl_Init();'))
        self.assertLess(source.index('acknowledges(Wire1, CST816_ADDR)'), source.index('ui.begin();'))
        self.assertIn('ACK is not pixels or physical touch', source)
        self.assertIn('V3_HEARTBEAT', source)
        self.assertIn('uptime_ms=', source)
        self.assertIn('tapcount=', source)

if __name__ == '__main__':
    unittest.main()
