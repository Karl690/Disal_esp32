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
#include "gpio/pinout.h"
#include "config.h"
#include "tone/tone.h"
extern const char *TAG;
extern SYSTEMCONFIG systemconfig;
bool load_configuration();
bool save_configuration();