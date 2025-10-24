#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_control_screen;
typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
    lv_obj_t* programmed_temperature;
    lv_obj_t* enabled;
    lv_obj_t* actual_temperature;
    lv_obj_t* duty;
    lv_obj_t* v_bat;
    
} UI_CONTROL;

void ui_control_init(void);