#pragma once
#include "main.h"

#define PCNT_INPUT_PIN_01 GPIO_NUM_2
#define PCNT_INPUT_PIN_02 GPIO_NUM_13
#define PCNT_CTRL_PIN GPIO_NUM_1


#define SAMPLE_TIME_MS      1000

#define PCNT_BATTERY_SCAL_VALUE  1.0
#define PCNT_RTD_SCAL_VALUE      1.0
#define PCNT_TEMP_SCAL_VALUE       1.0
#define PCNT_DUTY_SCAL_VALUE       1.0

typedef struct 
{
    int16_t count01;
    int16_t count02;
    float temperature; //'C    
    int16_t freq; //in Hz
    float duty; //in %
    float rtd_volt; //in V
    float bat_volt; //in V
} PCNT_INFO;

typedef struct {
	uint16_t    adcRaw; // expected adcValue for specified temp
	int16_t      value; // if temperature (s10.5 format - 1/32 degree) at specified adcValue; position, just value, etc
} AdcTableStruct;


extern PCNT_INFO pcnt_info;


void pcnt_init(void);
void pcnt_start(void);
void pcnt_stop(void);
void pcnt_get_value();
float pcnt_convert_temperature(float voltage);