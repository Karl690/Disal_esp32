#include "ui.h"
#include "ui-home.h"
lv_obj_t* ui_home_screen;

UI_HOME ui_home;

void ui_home_button_event_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	SCREEN_TYPE screen = (SCREEN_TYPE)(uintptr_t)lv_event_get_user_data(e);
	if (screen <= SCREEN_DIAGNOSTICS) {
		ui_transform_screen(screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
	}
		
}

void ui_home_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;

	if (key_code == LV_KEY_ENTER) {
		if (ui_home.focus_index != 0 && ui_home.focus_index <= SCREEN_DIAGNOSTICS) {
			ui_transform_screen((SCREEN_TYPE)(ui_home.focus_index), LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
		}
		return;
	} else {
		tone_play(direction == 1? 2000: 3000, 60);
		if (ui_home.focus_index != 0)
			ui_helpers_button_color(ui_home.buttons[ui_home.focus_index], UI_BUTTON_NORMAL_BR_COLOR, UI_BUTTON_NORMAL_FG_COLOR, UI_BUTTON_NORMAL_BR_COLOR);	

		ui_home.focus_index += direction;
		if (ui_home.focus_index >= 6) ui_home.focus_index = 1;
		else if (ui_home.focus_index <= 0) ui_home.focus_index = 5;
		
		if (ui_home.focus_index != 0)
			ui_helpers_button_color(ui_home.buttons[ui_home.focus_index], UI_BUTTON_ACTIVE_BG_COLOR, UI_BUTTON_ACTIVE_FG_COLOR, UI_BUTTON_ACTIVE_BR_COLOR);	
		ESP_LOGI("UI", "Home focus index %d", ui_home.focus_index);
	}
	
}

void ui_home_init(void)
{
	ui_home_screen = ui_helpers_create_screen();
	lv_obj_set_user_data(ui_home_screen, &ui_home);
	lv_obj_add_event_cb(ui_home_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_home.focus_index = 1;

	lv_group_t* group = lv_group_create();
	ui_home.group = group;
	lv_obj_t* obj = ui_helpers_create_label(ui_home_screen, LV_SYMBOL_HOME " HOME", &lv_font_montserrat_20);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_add_event_cb(obj, ui_home_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_group_add_obj(group, obj);
	
	uint16_t x = 0, y = 40, step = 40;

	obj = ui_helpers_create_button(ui_home_screen, "CONTROL", 150, 35, 8, &lv_font_montserrat_16, NULL, (void*)SCREEN_CONTROL);
	ui_helpers_button_color(obj, UI_BUTTON_ACTIVE_BG_COLOR, UI_BUTTON_ACTIVE_FG_COLOR, UI_BUTTON_ACTIVE_BR_COLOR);	
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_home.buttons[1] = obj;
	
	y += step;
	obj = ui_helpers_create_button(ui_home_screen, "SETTINGS", 205, 35, 8, &lv_font_montserrat_16, NULL, (void*)SCREEN_CONTROL);	
	ui_helpers_button_color(obj, UI_BUTTON_NORMAL_BG_COLOR, UI_BUTTON_NORMAL_FG_COLOR, UI_BUTTON_NORMAL_BR_COLOR);	
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_home.buttons[2] = obj;
	y += step;
	obj = ui_helpers_create_button(ui_home_screen, "DIAGNOSTIC", 206, 35, 8, &lv_font_montserrat_16, NULL, (void*)SCREEN_CONTROL);	
	ui_helpers_button_color(obj, UI_BUTTON_NORMAL_BG_COLOR, UI_BUTTON_NORMAL_FG_COLOR, UI_BUTTON_NORMAL_BR_COLOR);	
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_home.buttons[3] = obj;
	y += step;
	obj = ui_helpers_create_button(ui_home_screen, "BLE", 165, 35, 8, &lv_font_montserrat_16, NULL, (void*)SCREEN_CONTROL);	
	ui_helpers_button_color(obj, UI_BUTTON_NORMAL_BG_COLOR, UI_BUTTON_NORMAL_FG_COLOR, UI_BUTTON_NORMAL_BR_COLOR);	
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_home.buttons[4] = obj;
	y += step;
	obj = ui_helpers_create_button(ui_home_screen, "WIFI", 80, 35, 8, &lv_font_montserrat_16, NULL, (void*)SCREEN_CONTROL);	
	ui_helpers_button_color(obj, UI_BUTTON_NORMAL_BG_COLOR, UI_BUTTON_NORMAL_FG_COLOR, UI_BUTTON_NORMAL_BR_COLOR);	
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_home.buttons[5] = obj;
	y += step;
}
