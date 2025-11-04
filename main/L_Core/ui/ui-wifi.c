#include "ui.h"
#include "ui-wifi.h"
#include "taskmanager.h"

lv_obj_t* ui_wifi_screen;

UI_WIFI ui_wifi;
const uint8_t ui_wifi_data_size = 1;
uint32_t ui_wifi_current_time;
uint32_t ui_wifi_prev_time ;
uint8_t ui_wifi_click_count = 0;

void ui_wifi_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;

	if (key_code == LV_KEY_ENTER) {
		tone_play(6000, 10);
		ui_wifi_current_time = SliceCnt;
		if (ui_wifi_current_time - ui_wifi_prev_time < 5000) {
			ui_wifi_click_count ++;
		} else {
			ui_wifi_click_count = 0;
		}
		ui_wifi_prev_time = ui_wifi_current_time;
		
		if (ui_wifi_click_count > 1) {
			save_configuration();
			ui_show_messagebox(MESSAGEBOX_INFO, "Save successfully.", 1000);
			ui_wifi.focus_index = ui_wifi_data_size;
			lv_obj_set_y(ui_wifi.focus, -20 );
			lv_obj_set_style_text_color(ui_wifi.title, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
			ui_wifi_current_time = 0;
			return;
		}
		if (ui_wifi.focus_index == 1) {
			ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
			return;
		}
		
	} else {
		
	}
}

void ui_wifi_init(void)
{
	ui_wifi_screen = ui_helpers_create_screen();
	lv_obj_set_user_data(ui_wifi_screen, &ui_wifi);
	lv_obj_add_event_cb(ui_wifi_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_wifi.focus_index = 1;

	lv_group_t* group = lv_group_create();
	lv_obj_t* obj = ui_helpers_create_label(ui_wifi_screen, LV_SYMBOL_WIFI " WIFI", &lv_font_montserrat_20);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 10);
	lv_obj_add_event_cb(obj, ui_wifi_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_group_add_obj(group, obj);
	ui_wifi.group = group;
	ui_wifi.group_obj = obj;
	ui_wifi.focus = obj;
	
	uint16_t x = 30, y = 50, step = 30;
	obj = ui_helpers_create_label(ui_wifi_screen, (const char*)systemconfig.wifi.ssid, &lv_font_montserrat_16);
	lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	y += step;

	obj = ui_helpers_create_label(ui_wifi_screen, (const char*)systemconfig.wifi.ip, &lv_font_montserrat_16);
	lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
}
