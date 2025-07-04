#include "button.h"
#include "gpio/gpio.h"
void button_init() {
    ESP_LOGI(TAG, "button initalize");
    // gpio_set_mode(PIN_PWR_WAKE_UP, GPIO_INPUT);
    gpio_set_direction(PIN_PWR_WAKE_UP, GPIO_MODE_INPUT);
    gpio_pullup_en(PIN_PWR_WAKE_UP);
}

