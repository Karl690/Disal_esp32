#include "main.h"
#include "devices/display.h"
#include "ui/ui.h"
#include "wifi/wifi.h"
const char *TAG = "HYREL";
bool IsInitialized = false;

char temp_string[256];
extern "C" void app_main(void)
{
	//esp_log_level_set(TAG, ESP_LOG_DEBUG); // enable DEBUG logs for this App
    //Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	IsInitialized = false;
	wifi_init();
	
	InitLCDAndLVGL();
	InitUI();
	IsInitialized = true;
}
