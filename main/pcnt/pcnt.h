#pragma once
#include "main.h"

#define PCNT_INPUT_PIN GPIO_NUM_2
#define PCNT_CTRL_PIN GPIO_NUM_1
#define PCNT_UNIT PCNT_UNIT_0
#define SAMPLE_TIME_MS 1000

typedef struct 
{
    int16_t count;
    float temperature; //'C    
    int16_t freq; //in Hz
    uint8_t enabled; //0: disable, 1: enable
} PCNT_INFO;

extern PCNT_INFO pcnt_info;

void pcnt_init(void);
void pcnt_start(void);
void pcnt_stop(void);
void pcnt_get_value();