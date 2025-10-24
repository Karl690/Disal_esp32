// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: SquareLine_Project

#include "ui.h"
#include "ui-helpers.h"
#include <stdio.h>
#include <string.h>

char ui_temp_string[256];

lv_obj_t* ui_helpers_create_screen()
{
	lv_obj_t* screen = lv_obj_create(NULL);
	lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_bg_color(screen, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	// lv_obj_add_event_cb(screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	return screen;
}

lv_obj_t* ui_helpers_create_panel(lv_obj_t* parent, uint32_t color, bool scrollable)
{
	lv_obj_t* panel = lv_obj_create(parent);
	lv_obj_set_style_pad_all(panel, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
	if (!scrollable)
	{
		lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
	}
	lv_obj_set_style_bg_color(panel, lv_color_hex(color), LV_PART_MAIN);
	return panel;
}

lv_obj_t* ui_helpers_create_label(lv_obj_t* parent, const char* text, const lv_font_t* font)
{
	lv_obj_t* obj = lv_label_create(parent);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_FOREGROUND_COLOR), LV_PART_MAIN);
	lv_obj_set_width(obj, LV_SIZE_CONTENT); /// 1
	lv_obj_set_height(obj, LV_SIZE_CONTENT); /// 1
	lv_label_set_text_fmt(obj, "%s", text);	
	lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
	return obj;
}


void ui_event_edit_cb(lv_event_t* e)
{
#ifdef _USE_KEYBOARD
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	int mode = (int)lv_event_get_user_data(e);
	if (code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED)
	{
		if (keyboard != NULL)
		{
			lv_keyboard_set_textarea(keyboard, obj);
			if(mode)
			{
				lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUMBER);
			}
			else
			{
				lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
			}
			lv_obj_clear_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
		}
	}	
	else if (code == LV_EVENT_READY)
	{
		void* value = (void*)lv_obj_get_user_data(obj);
		lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);
		if (value) {
			if (mode)
			{
				int p = atoi(lv_textarea_get_text(obj));
				memcpy(value, &p, 4);
			}
			else
			{
				strcpy((char*)value, lv_textarea_get_text(obj));	
			}
		}
	}
#endif	
}
lv_obj_t* ui_helpers_create_textarea(lv_obj_t* parent, uint8_t w, uint8_t h, bool mulitiline)
{
	lv_obj_t* obj = lv_textarea_create(parent);
	// lv_obj_set_style_border_color(obj, lv_color_hex(UI_BORDER_COLOR), LV_PART_MAIN);
	lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
	lv_textarea_set_one_line(obj, mulitiline);
	lv_obj_set_style_pad_all(obj, 8, LV_PART_MAIN);
	lv_obj_set_size(obj, w, h);
	lv_obj_add_event_cb(obj, ui_event_edit_cb, LV_EVENT_ALL, NULL);
	return obj;
}
lv_obj_t* ui_helpers_create_button(lv_obj_t* parent,
		const char* text,
		uint16_t w,
		uint16_t h, 
		uint16_t radius,
		const lv_font_t* font,
		lv_event_cb_t event_button_handler, 
		void* event_data)
{
	lv_obj_t* button = lv_btn_create(parent);
	
	lv_obj_set_width(button, w); /// 1
	lv_obj_set_height(button, h); /// 1
	lv_obj_add_flag(button, LV_OBJ_FLAG_EVENT_BUBBLE);
	lv_obj_set_style_radius(button, radius, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(button, lv_color_hex(UI_BUTTON_NORMAL_BG_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(button, 128, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(button, 1, LV_PART_MAIN);
	lv_obj_set_style_border_width(button, 1, LV_PART_MAIN);
	lv_obj_set_style_border_color(button, lv_color_hex(UI_BUTTON_NORMAL_BR_COLOR), LV_PART_MAIN);
	lv_obj_set_style_shadow_width(button, 0, LV_PART_MAIN);
	
	lv_obj_t* label = lv_label_create(button);	
	lv_obj_center(label);
	
	lv_obj_set_style_text_color(label, lv_color_hex(UI_BUTTON_NORMAL_FG_COLOR), LV_PART_MAIN);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_label_set_text_fmt(label, text);
	lv_obj_set_style_text_font(label, font, LV_PART_MAIN | LV_STATE_DEFAULT);
	if (event_button_handler)	lv_obj_add_event_cb(button, event_button_handler, LV_EVENT_CLICKED, event_data);	
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	
	return button;
}

void ui_helpers_button_set_opa(lv_obj_t* button, uint8_t opa)
{
	lv_obj_set_style_bg_opa(button, opa, LV_PART_MAIN);
	lv_obj_set_style_border_opa(button, opa, LV_PART_MAIN);
}


void ui_helpers_button_color(lv_obj_t* button, uint32_t bg, uint32_t fg, uint32_t br)
{
	lv_obj_t* label = lv_obj_get_child(button, 0);
	if (label)	lv_obj_set_style_text_color(label, lv_color_hex(fg), LV_PART_MAIN);
	lv_obj_set_style_bg_color(button, lv_color_hex(bg), LV_PART_MAIN);
	lv_obj_set_style_border_color(button, lv_color_hex(br != 0? bg: br), LV_PART_MAIN);
}

void ui_helpers_button_text(lv_obj_t* button, const char* text)
{
	lv_obj_t* label = lv_obj_get_child(button, 0);
	if (label)	lv_label_set_text(label, text);
}

void ui_helpers_textarea_set_nmuber(lv_obj_t* obj, int number)
{
	if (!obj) return;
	sprintf(ui_temp_string, "%d", number);
	lv_textarea_set_text(obj, ui_temp_string);
}

void ui_helpers_label_set_nmuber(lv_obj_t* obj, int number)
{
	if (!obj) return;
	sprintf(ui_temp_string, "%d", number);
	lv_label_set_text(obj, ui_temp_string);
}

int ui_helpers_textarea_get_nmuber(lv_obj_t* obj)
{
	if (!obj) return 0;
	int number = atoi(lv_textarea_get_text(obj));
	return number;
}

int ui_helpers_label_get_nmuber(lv_obj_t* obj)
{
	if (!obj) return 0;
	int number = atoi(lv_label_get_text(obj));
	return number;
}

void ui_helpers_set_switch_value(lv_obj_t* obj, bool b)
{
	if (b) lv_obj_add_state(obj, LV_STATE_CHECKED);
	else lv_obj_clear_state(obj, LV_STATE_CHECKED);
}

bool ui_helpers_get_switch_value(lv_obj_t* obj)
{
	return lv_obj_has_state(obj, LV_STATE_CHECKED);
}
