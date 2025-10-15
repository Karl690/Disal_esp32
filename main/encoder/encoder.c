
#include "encoder.h"
#include "tone/tone.h"
volatile int32_t encoder_pos = 0;
volatile bool encoder_pressed = false;
volatile ENC_ACTION_TYPE encoder_action;
// Interrupt handler for encoder rotation
static void IRAM_ATTR encoder_isr_handler(void *arg) {
    int a = gpio_get_level(PIN_ENCODER_A);
    int b = gpio_get_level(PIN_ENCODER_B);
    if (a == b) {
        encoder_pos++; // Clockwise
        encoder_action = ENC_ACTION_CLOCKWISE;
    } else {
        encoder_pos--; // Counterclockwise
        encoder_action = ENC_ACTION_COUNTERCLOCKWISE;
    }

    
}


void encoder_init() {
    // Configure encoder pins (A, B)
    gpio_config_t enc_conf = {
        .pin_bit_mask = (1ULL << PIN_ENCODER_A) | (1ULL << PIN_ENCODER_B),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE // Trigger on both edges
    };
    gpio_config(&enc_conf);

    // Configure encoder button (GPIO 0)
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << PIN_ENCODER_BUTTON),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_conf);

    // Install ISR for encoder rotation
    gpio_install_isr_service(0);
    gpio_isr_handler_add(PIN_ENCODER_A, encoder_isr_handler, NULL);
}

int32_t encoder_read() {
    return encoder_pos;
}

bool encoder_get_status() {
    encoder_pressed = gpio_get_level(PIN_ENCODER_BUTTON) == 0 ? true : false;
    return encoder_pressed;
}

void encoder_check() {
    if (encoder_action == ENC_ACTION_NONE) return;
    if (encoder_action == ENC_ACTION_CLOCKWISE) tone_play(3000, 200);
    else tone_play(6000, 200);
    ESP_LOGI(TAG, "Clockwise rotary : %s", encoder_action == ENC_ACTION_CLOCKWISE? "Clockwise": "CounderClockwise");
    encoder_action = ENC_ACTION_NONE;

    int a = gpio_get_level(PIN_ENCODER_BUTTON);
    if (a == 0) {
        ESP_LOGI(TAG, "Press Button");
    }
}