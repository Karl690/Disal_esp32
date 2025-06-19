/*
MIT License

Copyright (c) 2022 Sukesh Ashok Kumar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
// SD card Working / enable it below
#define SD_SUPPORTED 

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#define LCD_MOSI_PIN 5
#define LCD_MISO_PIN -1
#define LCD_SCLK_PIN 6
#define LCD_DC_PIN   4
#define LCD_CS_PIN   7
#define LCD_RST_PIN  8
#define LCD_BUSY_PIN -1
#define LCD_BL_PIN   9

#define TP_I2C_SCL      12
#define TP_I2C_SDA      11
#define TP_INT          14

class LGFX : public lgfx::LGFX_Device {
	lgfx::Panel_GC9A01 _panel_instance;
	lgfx::Bus_SPI _bus_instance;
	lgfx::Light_PWM _light_instance;
	lgfx::Touch_FT5x06  _touch_instance;
public:
	LGFX(void)
	{
		{
			auto cfg = _bus_instance.config();

			cfg.spi_host = SPI3_HOST;
			cfg.spi_mode = 0;
			cfg.freq_write = 80000000;
			cfg.freq_read = 16000000;
			cfg.spi_3wire = true;
			cfg.use_lock = true;
			cfg.dma_channel = SPI_DMA_CH_AUTO;
			cfg.pin_sclk = LCD_SCLK_PIN;
			cfg.pin_mosi = LCD_MOSI_PIN;
			cfg.pin_miso = LCD_MISO_PIN;
			cfg.pin_dc = LCD_DC_PIN;

			_bus_instance.config(cfg);
			_panel_instance.setBus(&_bus_instance);
		}

		{
			auto cfg = _panel_instance.config();

			cfg.pin_cs = LCD_CS_PIN;
			cfg.pin_rst = LCD_RST_PIN;
			cfg.pin_busy = LCD_BUSY_PIN;

			cfg.panel_width = 240;
			cfg.panel_height = 240;
			cfg.offset_x = 0;
			cfg.offset_y = 0;
			cfg.offset_rotation = 0;
			cfg.dummy_read_pixel = 8;
			cfg.dummy_read_bits = 1;
			cfg.readable = true;
			cfg.invert = true;
			cfg.rgb_order = false;
			cfg.dlen_16bit = false;
			cfg.bus_shared = true;

			_panel_instance.config(cfg);
		}

		{
			auto cfg = _light_instance.config();

			cfg.pin_bl = LCD_BL_PIN;
			cfg.invert = false;
			cfg.freq = 44100;
			cfg.pwm_channel = 7;

			_light_instance.config(cfg);
			_panel_instance.setLight(&_light_instance);
		}
		
		{
			auto cfg = _touch_instance.config();
			cfg.x_min = 0;
			cfg.x_max = 239;
			cfg.y_min = 0;
			cfg.y_max = 239;
			cfg.pin_int = TP_INT; // Interrupt pin (optional)
			cfg.bus_shared = true;
			cfg.offset_rotation = 0;
			cfg.i2c_port = 0; // I2C port (0 or 1)
			cfg.i2c_addr = 0x38; // FT3267 I2C address
			cfg.pin_sda = TP_I2C_SDA; // I2C SDA
			cfg.pin_scl = TP_I2C_SCL; // I2C SCL
			cfg.freq = 400000; // I2C frequency
			_touch_instance.config(cfg);
			_panel_instance.setTouch(&_touch_instance);
		}

		// {
		//     // ???LCD ????RST ????
		//     auto cfg = _touch_instance.config();

		//     cfg.x_min = 0;
		//     cfg.x_max = LCD_HEIGHT;
		//     cfg.y_min = 0;
		//     cfg.y_max = LCD_WIDTH;
		//     cfg.pin_int = TOUCH_IRQ;
		//     cfg.pin_rst = TOUCH_RST;
		//     cfg.bus_shared = true;
		//     cfg.offset_rotation = 0;

		//     cfg.i2c_port = TOUCH_I2C_PORT;
		//     cfg.i2c_addr = TOUCH_I2C_ADDR;
		//     cfg.pin_sda = TOUCH_SDA;
		//     cfg.pin_scl = TOUCH_SCL;
		//     cfg.freq = 100000;

		//     _touch_instance.config(cfg);
		//     _panel_instance.setTouch(&_touch_instance);
		// }

		setPanel(&_panel_instance);
	}
};
