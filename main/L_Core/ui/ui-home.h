#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_home_screen;

typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
    lv_obj_t* buttons[6];
} UI_HOME;

void ui_home_init(void);
