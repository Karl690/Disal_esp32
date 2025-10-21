#include <driver/pcnt.h>
#include <driver/gpio.h>
#include "pcnt.h"

PCNT_INFO pcnt_info = {0, 0, 0, 0, 0, 0};
void pcnt_init(void) {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << PCNT_INPUT_PIN_01) | (1ULL << PCNT_INPUT_PIN_02),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_config_t ctrl_conf = {
        .pin_bit_mask = (1ULL << PCNT_CTRL_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&ctrl_conf);
    gpio_set_level((gpio_num_t)PCNT_CTRL_PIN, 1); // turn on

    pcnt_config_t pcnt_config = {
        .pulse_gpio_num = PCNT_INPUT_PIN_01,  // plus input pin
        .ctrl_gpio_num = PCNT_PIN_NOT_USED,  // control pin (not used)
        .lctrl_mode = PCNT_MODE_KEEP,      // low level control mode
        .hctrl_mode = PCNT_MODE_KEEP,      // high level control mode
        .pos_mode = PCNT_COUNT_INC,        //  increase count on up edge
        .neg_mode = PCNT_COUNT_DIS,        // increase count on down edge
        .counter_h_lim = 32767,            // max count value
        .unit = PCNT_UNIT_0,
        .channel = PCNT_CHANNEL_0
    };

    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    pcnt_config.pulse_gpio_num = PCNT_INPUT_PIN_02;  // plus input pin
    
    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));

    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_1));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_1));
    // ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT));
}

void pcnt_start() {
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT_1));
}

void pcnt_stop() {
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT_0));
}

void pcnt_get_value() {
    if (systemconfig.pcnt.enabled == 0) return;
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT_0, &pcnt_info.count01));
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT_1, &pcnt_info.count02));
    pcnt_info.bat_volt = (float)pcnt_info.count01 * PCNT_BATTERY_SCAL_VALUE;
    pcnt_info.rtd_volt = (float)pcnt_info.count02 * PCNT_RTD_SCAL_VALUE;
    pcnt_info.temperature = pcnt_convert_temperature(pcnt_info.rtd_volt);
    float deltaTemp = systemconfig.pcnt.programmed_temperature - pcnt_info.temperature;
    if (deltaTemp != 0) {
        pcnt_info.duty = deltaTemp * PCNT_K_CONSTANT;
        // gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 1); // turn on
    } else {
        pcnt_info.duty = 0;
        gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 0); // turn off
    }
    
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_0));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT_1));
}


float pcnt_convert_temperature(float voltage) {
    // Simple linear conversion for demonstration purposes
    // Replace with actual conversion logic as needed
    return voltage * PCNT_K_CONSTANT;
}