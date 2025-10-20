#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_pcnt_screen;
typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    lv_obj_t* programmed_temperature;
    lv_obj_t* enabled;
    lv_obj_t* freq;
    lv_obj_t* actual_temperature;
    lv_obj_t* duty;
    uint8_t  focus_index;
} UI_PCNT;

void ui_pcnt_init(void);