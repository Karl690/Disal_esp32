
#include "ledc.h"

void ledc_init() {
    ledc_timer_config_t ledc_timer_tone = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_TONE,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_tone));

    ledc_timer_config_t ledc_timer_pwm = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER_PWM_CTL,
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer_pwm));

    // Configure LEDC channel
    ledc_channel_config_t ledc_tone_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_TONE,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_TONE,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_tone_channel));

     // Configure LEDC channel
    ledc_channel_config_t ledc_pwm_channel = {
        .gpio_num       = ControlOutput_PIN,
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_PWM_CTL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_PWM_CTL,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_pwm_channel));
}

void ledc_tone(uint32_t freq, uint32_t duration_ms) {
    ledc_set_freq(LEDC_MODE, LEDC_TIMER_TONE, freq);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_TONE, LEDC_DUTY));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_TONE));
    vTaskDelay(duration_ms / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_TONE, 0));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_TONE));
}

void ledc_set_pwm(uint32_t freq, uint16_t duty) {
    ledc_set_freq(LEDC_MODE, LEDC_TIMER_PWM_CTL, freq);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_PWM_CTL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_PWM_CTL));
}

