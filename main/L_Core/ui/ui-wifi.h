#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_wifi_screen;

typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
    int8_t select_index;
    lv_obj_t* title;
    lv_obj_t* ip;
} UI_WIFI;

void ui_wifi_init(void);
