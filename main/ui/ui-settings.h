#pragma once
#include "lvgl.h"


typedef struct {
    lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
    lv_obj_t* title;
    int8_t select_index;
    int8_t start_index;
    lv_obj_t* up_focus;
    lv_obj_t* down_focus;
} UI_SETTINGS;


typedef struct {
    char title[32];
    void* value_ptr;
    uint8_t type; //0: int, 1: float, 2: bool
} UI_SETTINGS_DATA_ITEM;

typedef struct {
    lv_obj_t* panel;
    lv_obj_t* title;
    lv_obj_t* value;
    UI_SETTINGS_DATA_ITEM* data_item;
} UI_SETTINGS_ITEM;

extern lv_obj_t *ui_settings_screen;


void ui_settings_init(void);