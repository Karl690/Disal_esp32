#include "ui.h"
#include "ui-ble.h"
#include "ble/ble.h"
#include "taskmanager.h"
lv_obj_t* ui_ble_screen;

UI_BLE ui_ble;
const uint8_t ui_ble_data_size = 1;
uint32_t ui_ble_current_time;
uint32_t ui_ble_prev_time ;
uint8_t ui_ble_click_count = 0;

void ui_ble_click_cb(lv_event_t* e) {
	lv_obj_t* obj = lv_event_get_current_target_obj(e);
	if (obj == ui_ble.enabled) {
		// systemconfig.bluetooth.server_enabled = systemconfig.bluetooth.server_enabled == 1 ? 0 : 1;
		// if (systemconfig.bluetooth.server_enabled == 1) {
		// 	ui_show_messagebox(MESSAGEBOX_INFO, "BLE Enabled", 1000);
		// }
		// else {
		// 	ui_show_messagebox(MESSAGEBOX_ERROR, "BLE Disabled", 1000);
		// }
		// ui_helpers_button_color(ui_ble.enabled, systemconfig.bluetooth.server_enabled == 1? 0x00ff00 : 0xff0000, UI_FOREGROUND_COLOR, 0);
		// ui_helpers_button_text(ui_ble.enabled, systemconfig.bluetooth.server_enabled == 1 ? "ON" : "OFF");
	} else if (obj == ui_ble.ping) {
		ui_temp_buffer[0] = 0x7;// PING CODE
		ble_server_send_data((uint8_t*)ui_temp_buffer, 1);
	}
	
}

void ui_ble_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;

	if (key_code == LV_KEY_ENTER) {
		tone_play(6000, 10);
		ui_ble_current_time = SliceCnt;
		if (ui_ble_current_time - ui_ble_prev_time < 5000) {
			ui_ble_click_count ++;
		} else {
			ui_ble_click_count = 0;
		}
		ui_ble_prev_time = ui_ble_current_time;
		
		if (ui_ble_click_count > 1) {
			save_configuration();
			ui_show_messagebox(MESSAGEBOX_INFO, "Save successfully.", 1000);
			ui_ble.focus_index = ui_ble_data_size;
			lv_obj_set_y(ui_ble.focus, -20 );
			lv_obj_set_style_text_color(ui_ble.title, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
			ui_ble_click_count = 0;
			return;
		}
		if (ui_ble.focus_index == ui_ble_data_size) {
			ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
			return;
		}
		if (ui_ble.select_index == ui_ble.focus_index) {
			ui_ble.select_index = -1;
			lv_obj_set_style_text_color(ui_ble.focus, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
		} else {
			ui_ble.select_index = ui_ble.focus_index;		
			lv_obj_set_style_text_color(ui_ble.focus, lv_color_hex(UI_ITEM_SELECT_FG_COLOR), LV_PART_MAIN);
		}
	} else {
		tone_play(direction == 1? 2000: 3000, 60);
		if (ui_ble.select_index == -1) { // change focus
			ui_ble.focus_index += direction;
			if (ui_ble.focus_index < 0)	 ui_ble.focus_index = ui_ble_data_size;
			else if (ui_ble.focus_index > ui_ble_data_size) ui_ble.focus_index = 0;
			lv_obj_set_style_text_color(ui_ble.title, lv_color_hex(UI_ITEM_NORMAL_FG_COLOR), LV_PART_MAIN);
			lv_obj_set_style_text_color(ui_ble.focus, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
			switch (ui_ble.focus_index)
			{
			case 0:
				lv_obj_set_pos(ui_ble.focus, 25, lv_obj_get_y(ui_ble.name));
				break;
			// case 1:
			// 	lv_obj_set_pos(ui_ble.focus, 10, lv_obj_get_y(ui_ble.enabled) + 8);
			// 	break;
			case 1:
				lv_obj_set_y(ui_ble.focus, -20);
				lv_obj_set_style_text_color(ui_ble.title, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
				break;
			}
		} else {
			// change value
			switch (ui_ble.focus_index)
			{
			case 0: {
					int temp = systemconfig.bluetooth.address_number;
					temp += direction;
					if (temp < 0) temp = 0;
					else if (temp > 100) temp = 100;
					ble_update_name(temp);
					lv_label_set_text_fmt(ui_ble.name, "%s", ble_get_name());
					systemconfig.bluetooth.address_number = temp;
				}
				break;
			// case 1:
			// 	systemconfig.bluetooth.server_enabled = systemconfig.bluetooth.server_enabled == 1 ? 0 : 1;
			// 	if (systemconfig.bluetooth.server_enabled == 1) {
			// 		ble_server_enable();
			// 		ui_show_messagebox(MESSAGEBOX_INFO, "BLE Enabled", 1000);
			// 	}
			// 	else {
			// 		ble_server_disable();
			// 		ui_show_messagebox(MESSAGEBOX_INFO, "BLE Disabled", 1000);
			// 	}
			// 	ui_helpers_button_color(ui_ble.enabled, systemconfig.bluetooth.server_enabled == 1? 0x00ff00 : 0xff0000, UI_FOREGROUND_COLOR, 0);
			// 	ui_helpers_button_text(ui_ble.enabled, systemconfig.bluetooth.server_enabled == 1 ? "ON" : "OFF");
			// 	break;
			}
		}
	}
}

void ui_ble_timer_cb(lv_timer_t* t) {
	ui_helpers_label_set_nmuber(ui_ble.rcv, ble_server_total_received);
	ui_helpers_label_set_nmuber(ui_ble.xmt, ble_server_total_sent);
}

void ui_ble_init(void)
{
	ui_ble_screen = ui_helpers_create_screen();
	lv_obj_set_user_data(ui_ble_screen, &ui_ble);
	lv_obj_add_event_cb(ui_ble_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_ble.focus_index = 1;
	ui_ble.select_index = -1;

	lv_group_t* group = lv_group_create();
	lv_obj_t* obj = ui_helpers_create_label(ui_ble_screen, LV_SYMBOL_BLUETOOTH " BLE", &lv_font_montserrat_20);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 10);
	ui_ble.title = obj;
	
	obj = ui_helpers_create_label(ui_ble_screen, LV_SYMBOL_PLAY, &lv_font_montserrat_12);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
	lv_obj_add_event_cb(obj, ui_ble_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_group_add_obj(group, obj);
	ui_ble.group = group;
	ui_ble.group_obj = obj;
	ui_ble.focus = obj;

	uint16_t x = 30, y = 50, step = 40;
	obj = ui_helpers_create_label(ui_ble_screen, ble_get_name(), &lv_font_montserrat_16);
	lv_obj_set_style_text_color(obj, lv_color_hex(0x84ff00), LV_PART_MAIN);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	lv_obj_set_pos(ui_ble.focus, 25, y);
	ui_ble.name = obj;
	// y += step - 10;
	// obj = ui_helpers_create_button(ui_ble_screen, systemconfig.bluetooth.server_enabled == 1? "ON": "OFF", 160, 40, 5, &lv_font_montserrat_14, ui_ble_click_cb, NULL);
	// ui_helpers_button_color(obj, systemconfig.bluetooth.server_enabled == 1? 0x00ff00: 0xff0000, UI_FOREGROUND_COLOR, 0);
	// lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	// ui_ble.enabled = obj;
	y += step + 10;
	obj = ui_helpers_create_label(ui_ble_screen, "RCV#:", &lv_font_montserrat_14);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, -70, y);
	obj = ui_helpers_create_label(ui_ble_screen, "0", &lv_font_montserrat_14);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 30, y);
	ui_ble.rcv = obj;
	y += step - 10;
	obj = ui_helpers_create_label(ui_ble_screen, "XMT#:", &lv_font_montserrat_14);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, -60, y);
	obj = ui_helpers_create_label(ui_ble_screen, "0", &lv_font_montserrat_14);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 30, y);
	y += step - 10;
	ui_ble.xmt = obj;
	obj = ui_helpers_create_button(ui_ble_screen, "PING", 120, 40, 5, &lv_font_montserrat_14, ui_ble_click_cb, NULL);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_ble.ping = obj;
	lv_timer_create(ui_ble_timer_cb, 1000, NULL);
}
