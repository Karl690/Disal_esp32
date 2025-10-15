#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_pcnt_screen;
typedef struct {
    lv_obj_t* programmed_temperature;
    lv_obj_t* enabled;
    lv_obj_t* freq;
    lv_obj_t* actual_temperature;
    lv_obj_t* duty;
} UI_PCNT;

void ui_pcnt_init(void);