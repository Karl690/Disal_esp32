#pragma once
#include "lvgl.h"


extern lv_obj_t *ui_ble_screen;

typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
    int8_t select_index;
    lv_obj_t* title;
    lv_obj_t* name;
    lv_obj_t* enabled;
    lv_obj_t* xmt;
    lv_obj_t* rcv;
    lv_obj_t* ping;
} UI_BLE;

void ui_ble_init(void);
