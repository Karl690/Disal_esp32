#pragma once

#include "main.h"
#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sys.h"


#define WIFI_SSID "chaoyou"
#define WIFI_PASSWORD "88889999"


void wifi_init(void);

void wifi_start();
void wifi_stop();