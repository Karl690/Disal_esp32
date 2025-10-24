//#define USE_OPC
#pragma once
#include <stdint.h>

#define SDCARD_MOUNT_POINT "/sd-card"
#define SYSTEM_CONFIG_FILE SDCARD_MOUNT_POINT"/config.ini"

#define CMD_MAX_SIZE 50
#define RX_BUF_SIZE  0x100
#define TX_BUF_SIZE  0x100


typedef struct
{
	uint8_t ssid[32];
	uint8_t password[32];
	uint8_t autoconnect;
	uint8_t status; //1: connected, 0: disconnected
	uint8_t connnected; //1: connected to ap
	uint8_t ip[30];
	char server_ip[50];
	char hostname[20];
} WIFI_CONFIG;

typedef struct
{
	uint8_t autostart; 
	uint8_t status; //1: connected, 0: disconnected	
	uint8_t server_enabled;
	uint8_t client_enabled;
} BLUETOOTH_CONFIG;
typedef struct
{
	uint8_t automount; 
	uint8_t status; //1: connected, 0: disconnected	
} SDCARD_CONFIG;

typedef struct 
{
	uint8_t pcnt_pin_01;
	uint8_t pcnt_pin_02;
	uint8_t enabled;
	uint8_t ctrl_pin;
	uint16_t programmed_temperature;
	float temp_scale;
	float battery_scale;
	float rtd_scale;
	float duty_scale;
} PCNT_CONFIG;

typedef struct
{
	uint8_t timerReload1;
	uint8_t timerReload2;
	uint8_t timerRetry;
} SECS_CONFIG;

typedef struct 
{
	uint8_t initialized;
	WIFI_CONFIG wifi;
	BLUETOOTH_CONFIG bluetooth;
	SDCARD_CONFIG sdcard;
	PCNT_CONFIG pcnt;
}SYSTEMCONFIG;


typedef enum
{
	VALUE_TYPE_INT,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_STRING,
	VALUE_TYPE_BOOL,
} VALUE_TYPE;
