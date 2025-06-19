#pragma once
#include "esp_log.h"
#include "nvs_flash.h"

#include <inttypes.h>
#include <string.h>
#include <esp_chip_info.h>
#include <esp_event.h>
#include <esp_flash.h>
#include <esp_ota_ops.h>
#include <soc/rtc.h>
//#include <fmt/core.h>
//#include <fmt/format.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

extern const char *TAG;
