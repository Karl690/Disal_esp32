
#pragma once

#include "main.h"
#include "driver/ledc.h"

#define LEDC_TIMER_TONE         LEDC_TIMER_0
#define LEDC_TIMER_PWM_CTL         LEDC_TIMER_1
#define LEDC_MODE          LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO     (PIN_BUZZER) // GPIO for buzzer
#define LEDC_CHANNEL_TONE      LEDC_CHANNEL_0
#define LEDC_CHANNEL_PWM_CTL   LEDC_CHANNEL_1
#define LEDC_DUTY_RES      LEDC_TIMER_10_BIT // 10-bit resolution
#define LEDC_DUTY          (512) // 50% duty cycle
#define LEDC_FREQUENCY     (440) // 440 Hz (A4 note)
#define TONE_DURATION_MS   (500) // 500 ms duration
#define BUTTON_GPIO        (4) // GPIO for button input
#define DEBOUNCE_TIME_MS   (50) // Debounce delay

void ledc_init();
void ledc_tone(uint32_t freq, uint32_t duration_ms);
void ledc_set_pwm(uint32_t freq, uint16_t duty);