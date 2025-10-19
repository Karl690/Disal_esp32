#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_home_screen;

typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    uint8_t  focus_index;
} UI_HOME;

void ui_home_init(void);
