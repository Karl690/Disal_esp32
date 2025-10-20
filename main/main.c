#include "main.h"
#include "ui/ui.h"
#include "ledc/ledc.h"
#include "gpio/gpio.h"
#include "lcd/lcd.h"
#include "storage/partition.h"
#include "storage/nvs.h"
#include "bsp/m5dial.h"
#include "wifi/wifi.h"
#include "pcnt/pcnt.h"
#include "task_manager/task_manager.h"
#include "iot_button.h"
#include "button_gpio.h"
#include "utils/utils.h"

const char *TAG = "HYREL";
bool IsInitialized = false;
SYSTEMCONFIG systemconfig;
char temp_string[256];

extern "C" void app_main(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	IsInitialized = false;
	storage_nvs_init();
	load_configuration();

	lcd_init();
	bsp_display_lock(0);	
	ui_init();
	bsp_display_unlock();
	wifi_init();
	
	pcnt_init();
	ledc_init();
	tone_init();
	IsInitialized = true;
}

bool load_configuration()
{
	storage_nvs_get_blob(NVS_KEY_CONFIG, &systemconfig);
	if (!utils_valid_pins(systemconfig.pcnt.pcnt_pin_01)) systemconfig.pcnt.pcnt_pin_01 = PCNT_INPUT_PIN_01;
	if (!utils_valid_pins(systemconfig.pcnt.pcnt_pin_02)) systemconfig.pcnt.pcnt_pin_02 = PCNT_INPUT_PIN_02;
	if (!utils_valid_pins(systemconfig.pcnt.ctrl_pin)) systemconfig.pcnt.ctrl_pin = PCNT_CTRL_PIN;
	
	if (systemconfig.bluetooth.status && systemconfig.wifi.status) {
		// disable wifi if bluetooth is enabled
		systemconfig.bluetooth.status = 0;
	}
	if (systemconfig.wifi.ssid[0] == 0) {
		strcpy((char*)systemconfig.wifi.ssid, "your-ssid");
	}
	return true;
}

bool save_configuration()
{
	systemconfig.initialized = 1;
	return storage_nvs_set_blob(NVS_KEY_CONFIG, &systemconfig, sizeof(SYSTEMCONFIG));
}
