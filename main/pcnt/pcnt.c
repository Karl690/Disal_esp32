#include <driver/pcnt.h>
#include <driver/gpio.h>
#include "pcnt.h"

PCNT_INFO pcnt_info = {0, 0, 0};
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
        .unit = PCNT_UNIT,
        .channel = PCNT_CHANNEL_0
    };

    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT));
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT));
    // ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT));
}

void pcnt_start(void) {
    ESP_LOGI("PCNT", "PCNT Start");
    ESP_ERROR_CHECK(pcnt_counter_resume(PCNT_UNIT));
}

void pcnt_stop(void) {
    ESP_LOGI("PCNT", "PCNT Stop");  
    ESP_ERROR_CHECK(pcnt_counter_pause(PCNT_UNIT));
}

void pcnt_get_value() {
    if (pcnt_info.enabled == 0) return;
    ESP_ERROR_CHECK(pcnt_get_counter_value(PCNT_UNIT, &pcnt_info.count));
    pcnt_info.temperature = (float)pcnt_info.count / 1000.0;
    if (pcnt_info.count < systemconfig.pcnt.programmed_temperature) {
        gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 1); // turn on
    } else {
        gpio_set_level((gpio_num_t)systemconfig.pcnt.ctrl_pin, 0); // turn off
    }
    ESP_ERROR_CHECK(pcnt_counter_clear(PCNT_UNIT));
}