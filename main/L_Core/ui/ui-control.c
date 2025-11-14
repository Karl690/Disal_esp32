#include "ui.h"
#include "pcnt/pcnt.h"
#include "ui-control.h"
#include "taskmanager.h"
lv_obj_t* ui_control_screen;
UI_CONTROL ui_control;

LV_FONT_DECLARE(mono_regualr_16);

const uint8_t ui_control_data_size = 1;
uint32_t ui_control_current_time;
uint32_t ui_control_prev_time ;
uint8_t ui_control_click_count = 0;

void ui_control_refresh()
{
	sprintf(ui_temp_buffer, "%.2f V", pcnt_info.bat_volt);
	lv_label_set_text(ui_control.v_bat, ui_temp_buffer);
	
	sprintf(ui_temp_buffer, "%.2f 'C", pcnt_info.temperature);
	lv_label_set_text(ui_control.actual_temperature, ui_temp_buffer);//pcnt_info.temperature
	
	sprintf(ui_temp_buffer, "%d", pcnt_info.duty);
	lv_label_set_text(ui_control.duty, ui_temp_buffer);
}

void ui_control_click_cb(lv_event_t* e) {
	lv_obj_t* obj = lv_event_get_target_obj(e);
	if (obj == ui_control.enabled) {
		systemconfig.pcnt.enabled = systemconfig.pcnt.enabled == 1 ? 0 : 1;
		if (systemconfig.pcnt.enabled) EnableCounter();
		else DisableCounter();
		ui_show_messagebox(systemconfig.pcnt.enabled ? MESSAGEBOX_INFO: MESSAGEBOX_ERROR, systemconfig.pcnt.enabled ? "Counte is enabled" : "Counter is disabled" , 1000);
		ui_helpers_button_color(ui_control.enabled, systemconfig.pcnt.enabled == 1? 0x00ff00 : 0xff0000, UI_FOREGROUND_COLOR, 0);
	}
}

void ui_control_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;
	if (key_code == LV_KEY_ENTER) {
		tone_play(6000, 10);
		ui_control_current_time = SliceCnt;
		if (ui_control_current_time - ui_control_prev_time < 5000) {
			ui_control_click_count ++;
		} else {
			ui_control_click_count = 0;
		}
		ui_control_prev_time = ui_control_current_time;
		
		if (ui_control_click_count > 1) {
			save_configuration();
			ui_show_messagebox(MESSAGEBOX_INFO, "Save successfully.", 1000);
			ui_control.focus_index = ui_control_data_size;
			lv_obj_set_y(ui_control.focus, -20 );
			lv_obj_set_style_text_color(ui_control.title, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
			ui_control_click_count = 0;
			return;
		}
		if (ui_control.focus_index == ui_control_data_size) {
			ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
			return;
		}
		if (ui_control.select_index == ui_control.focus_index) {
			ui_control.select_index = -1;
			lv_obj_set_style_text_color(ui_control.focus, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
		} else {
			ui_control.select_index = ui_control.focus_index;		
			lv_obj_set_style_text_color(ui_control.focus, lv_color_hex(UI_ITEM_SELECT_FG_COLOR), LV_PART_MAIN);
		}
	} else { // rotary event
		tone_play(direction == 1? 2000: 3000, 10);
		if (ui_control.select_index == -1) {
			// change focus
			ui_control.focus_index += direction;
			if (ui_control.focus_index < 0)	 ui_control.focus_index = ui_control_data_size;
			else if (ui_control.focus_index > ui_control_data_size) ui_control.focus_index = 0;
			lv_obj_set_style_text_color(ui_control.title, lv_color_hex(UI_ITEM_NORMAL_FG_COLOR), LV_PART_MAIN);
			lv_obj_set_style_text_color(ui_control.focus, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
			switch (ui_control.focus_index)
			{
			case 0:
				lv_obj_set_pos(ui_control.focus, 10, lv_obj_get_y(ui_control.programmed_temperature));
				break;
			case 1:
				lv_obj_set_y(ui_control.focus, -20 );
				lv_obj_set_style_text_color(ui_control.title, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
				break;
			}
		} else {
			// change value
			switch (ui_control.focus_index)
			{
			case 0: {
					int temp = systemconfig.pcnt.programmed_temperature;
					temp += direction;
					if (temp < 0) temp = 0;
					else if (temp > 100) temp = 100;
					sprintf(ui_temp_buffer, "%d 'c", temp);
					lv_label_set_text_fmt(ui_control.programmed_temperature, "%s", ui_temp_buffer);
					systemconfig.pcnt.programmed_temperature = temp;
				}
				break;
			}
		}
	}
}

void ui_control_init(void)
{
	ui_control_screen = ui_helpers_create_screen();
	lv_group_t* group = lv_group_create();
	lv_obj_set_user_data(ui_control_screen, &ui_control);
	lv_obj_add_event_cb(ui_control_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_control.focus_index = 0;
	ui_control.select_index = -1;

	uint16_t x = 0, y = 15, step = 32;	

	lv_obj_t* obj = ui_helpers_create_label(ui_control_screen, "TEMP CNTRL", &lv_font_montserrat_20);
	lv_obj_set_pos(obj, 50, y);	
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_ITEM_NORMAL_FG_COLOR), LV_PART_MAIN);
	lv_group_add_obj(group, obj);
	ui_control.group = group;
	ui_control.group_obj = obj;
	ui_control.title = obj;
	lv_obj_add_event_cb(obj, ui_control_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	
	y += step;

	obj = ui_helpers_create_label(ui_control_screen, LV_SYMBOL_PLAY, &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_ITEM_FOCUS_FG_COLOR), LV_PART_MAIN);
	ui_control.focus = obj;
	
	lv_obj_t* label = ui_helpers_create_label(ui_control_screen, "TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(label, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(label, 45, y);
	obj = ui_helpers_create_label(ui_control_screen, "-", &lv_font_montserrat_14);
	ui_control.actual_temperature = obj;
	lv_obj_align_to(obj, label, LV_ALIGN_LEFT_MID, 90, 0);

	y += step;
	label = ui_helpers_create_label(ui_control_screen, "PRG TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(label, lv_color_hex(0x84ff00), LV_PART_MAIN);
	lv_obj_set_pos(label, 35, y);
	obj = ui_helpers_create_label(ui_control_screen, "", &lv_font_montserrat_14);
	lv_label_set_text_fmt(obj, "%d 'C", systemconfig.pcnt.programmed_temperature);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_set_style_pad_all(obj, 3, LV_PART_MAIN);
	// lv_obj_set_width(obj, 60);
	lv_group_focus_obj(obj);
	lv_obj_align_to(obj, label, LV_ALIGN_LEFT_MID, 90, 0);
	ui_control.programmed_temperature = obj;

	lv_obj_set_pos(ui_control.focus, 10, y);
	
	
	y += step;
	label = ui_helpers_create_label(ui_control_screen, "Duty:", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(label, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(label, 20, y);
	obj = ui_helpers_create_label(ui_control_screen, "-", &lv_font_montserrat_14);
	ui_control.duty = obj;
	lv_obj_align_to(obj, label, LV_ALIGN_LEFT_MID, 90, 0);
	
	y += step;
	label = ui_helpers_create_label(ui_control_screen, "V BAT:", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(label, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(label, 40, y);
	obj = ui_helpers_create_label(ui_control_screen, "-", &lv_font_montserrat_14);
	ui_control.v_bat = obj;
	lv_obj_align_to(obj, label, LV_ALIGN_LEFT_MID, 90, 0);

	y += step;
	obj = ui_helpers_create_button(ui_control_screen, "ON", 80, 40, 5, &lv_font_montserrat_14, ui_control_click_cb, NULL);
	ui_helpers_button_color(obj, 0x00ff00, UI_FOREGROUND_COLOR, 0);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, y);
	ui_control.enabled = obj;

}
