#pragma once
#include "esp_log.h"
#include "nvs_flash.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <esp_chip_info.h>
#include <esp_event.h>
#include <esp_flash.h>
#include <esp_ota_ops.h>
#include <soc/rtc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_timer.h"
#include <driver/gpio.h>
#include "config.h"
#include "tone/tone.h"

#define PIN_PWR_HOLDING     (gpio_num_t)46
#define PIN_PWR_WAKE_UP     (gpio_num_t)42

#define PIN_ENCODER_A       (gpio_num_t)41
#define PIN_ENCODER_B       (gpio_num_t)40
#define PIN_ENCODER_BUTTON  (gpio_num_t)42
#define PIN_TP_I2C_SCL      (gpio_num_t)12
#define PIN_TP_I2C_SDA      (gpio_num_t)11
#define PIN_TP_INT          (gpio_num_t)14

#define PIN_GROVE_I2C_SCL   (gpio_num_t)15
#define PIN_GROVE_I2C_SDA   (gpio_num_t)13

#define PIN_BUZZER          (gpio_num_t)3


extern const char *TAG;
extern SYSTEMCONFIG systemconfig;
bool load_configuration();
bool save_configuration();