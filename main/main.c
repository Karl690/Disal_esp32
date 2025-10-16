#include "main.h"
#include "ui/ui.h"
// #include "ledc/ledc.h"
#include "gpio/gpio.h"
#include "lcd/lcd.h"
// #include "tone/tone.h"
// #include "button/button.h"
#include "bsp/m5dial.h"
#include "wifi/wifi.h"
#include "encoder/encoder.h"
#include "pcnt/pcnt.h"
#include "task_manager/task_manager.h"

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
	// powerOn();
	lcd_init();
	bsp_display_lock(0);	
	ui_init();
	bsp_display_unlock();
	// wifi_init();
	
	// pcnt_init();
	
	IsInitialized = true;

	// tone_play(4000, 30);

	task_manager_init();
	// while (1) {
	// 	if (gpio_get_level(PIN_ENCODER_BUTTON) == 0) {
	// 		ESP_LOGI(TAG, "Button pressed");
	// 		// tone_play(7000, 1000);
	// 	} 
	// 	vTaskDelay(100 / portTICK_PERIOD_MS);
	// }
}


void powerOn() {
	gpio_num_t pin_pwr_holding = (gpio_num_t)PIN_PWR_HOLDING;

	gpio_reset_pin(pin_pwr_holding);
	gpio_set_direction(pin_pwr_holding, GPIO_MODE_OUTPUT);
	gpio_set_pull_mode(pin_pwr_holding, GPIO_PULLUP_PULLDOWN);

	/* Keep power on */
	gpio_set_level(pin_pwr_holding, 1);
}

void powerOff() {
	gpio_num_t pin_pwr_holding = (gpio_num_t)PIN_PWR_HOLDING;

	gpio_reset_pin(pin_pwr_holding);
	gpio_set_direction(pin_pwr_holding, GPIO_MODE_OUTPUT);
	gpio_set_pull_mode(pin_pwr_holding, GPIO_PULLUP_PULLDOWN);

	/* Keep power on */
	gpio_set_level(pin_pwr_holding, 0);
}