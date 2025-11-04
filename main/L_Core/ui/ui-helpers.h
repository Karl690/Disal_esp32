#pragma once
#include "lvgl.h"

lv_obj_t* ui_helpers_create_screen();
lv_obj_t* ui_helpers_create_panel(lv_obj_t* parent, uint32_t color, bool scrollable);
lv_obj_t* ui_helpers_create_label(lv_obj_t* parent, const char* text, const lv_font_t* font);
lv_obj_t* ui_helpers_create_textarea(lv_obj_t* parent, uint8_t w, uint8_t h, bool mulitiline);
lv_obj_t* ui_helpers_create_button(lv_obj_t* parent,
    const char* text,
    uint16_t w,
    uint16_t h, 
    uint16_t radius,
    const lv_font_t* font,
    lv_event_cb_t event_button_handler, 
    void* event_data);

void ui_helpers_button_set_opa(lv_obj_t* button, uint8_t opa);
void ui_helpers_button_color(lv_obj_t* button, uint32_t bg, uint32_t fg, uint32_t br);
void ui_helpers_button_text(lv_obj_t* button, const char* text);
void ui_helpers_textarea_set_nmuber(lv_obj_t* obj, int number);
void ui_helpers_label_set_nmuber(lv_obj_t* obj, int number);
int ui_helpers_textarea_get_nmuber(lv_obj_t* obj);
int ui_helpers_label_get_nmuber(lv_obj_t* obj);

void ui_helpers_set_switch_value(lv_obj_t* obj, bool b);
bool ui_helpers_get_switch_value(lv_obj_t* obj);
