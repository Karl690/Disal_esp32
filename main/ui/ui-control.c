#include "ui.h"
#include "pcnt/pcnt.h"
#include "ui-control.h"

lv_obj_t* ui_control_screen;
UI_CONTROL ui_control;

LV_FONT_DECLARE(mono_regular_24);

void ui_control_timer_cb(lv_timer_t * timer)
{
	if (systemconfig.pcnt.enabled)
	{
		lv_label_set_text_fmt(ui_control.v_bat, "%dV", pcnt_info.bat_volt);
		lv_label_set_text_fmt(ui_control.actual_temperature, "%d˚C", pcnt_info.temperature);
		lv_label_set_text_fmt(ui_control.duty, "%d%%", pcnt_info.duty);
	}
}

void ui_control_event_click_cb(lv_event_t* e)
{
	tone_play(6000, 200);
	systemconfig.pcnt.enabled = systemconfig.pcnt.enabled == 1 ? 0 : 1;
	if (systemconfig.pcnt.enabled == 1) {
		pcnt_start();
		ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Enabled", 1000);
	}
	else {
		pcnt_stop();
		ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Disabled", 1000);
	}
	ui_helpers_button_color(ui_control.enabled, systemconfig.pcnt.enabled == 1? 0x00ff00 : 0xff0000, UI_FOREGROUND_COLOR, 0);
	ui_helpers_button_text(ui_control.enabled, systemconfig.pcnt.enabled == 1 ? "ON" : "OFF");
}

void ui_control_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;


	if (key_code != LV_KEY_RIGHT && key_code != LV_KEY_LEFT) return;
	
	switch (ui_control.focus_index)
	{
	case 0:	{
		tone_play(direction == 1? 2000: 3000, 60);

		int temp = systemconfig.pcnt.programmed_temperature;
		temp += direction;
		if (temp < 0) temp = 0;
		else if (temp > 100) temp = 100;
		lv_label_set_text_fmt(ui_control.programmed_temperature, "%d˚C", temp);
		systemconfig.pcnt.programmed_temperature = temp;
		save_configuration();
	}
	break;
	case 1: {
		tone_play(6000, 200);
		systemconfig.pcnt.enabled = systemconfig.pcnt.enabled == 1 ? 0 : 1;
		if (systemconfig.pcnt.enabled == 1) {
			pcnt_start();
			ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Enabled", 1000);
		}
		else {
			pcnt_stop();
			ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Disabled", 1000);
		}
		ui_helpers_button_color(ui_control.enabled, systemconfig.pcnt.enabled == 1? 0x00ff00 : 0xff0000, UI_FOREGROUND_COLOR, 0);
		ui_helpers_button_text(ui_control.enabled, systemconfig.pcnt.enabled == 1 ? "ON" : "OFF");
	}
	break;
	case 2:
		ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
		break;
	default:
		break;
	}

}

void ui_control_event_encoder_click_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	ui_control.focus_index++;
	if (ui_control.focus_index > 2) ui_control.focus_index = 0;
	tone_play(6000, 200);
	switch (ui_control.focus_index)
	{
	case 0:
		lv_obj_set_y(ui_control.focus, lv_obj_get_y(ui_control.programmed_temperature) + 8);
		break;
	case 1:
		lv_obj_set_y(ui_control.focus, lv_obj_get_y(ui_control.enabled) + 8);
		break;
	case 2:
		lv_obj_set_y(ui_control.focus, -20 );
		break;
	}
}

void ui_control_init(void)
{
	ui_control_screen = ui_helpers_create_screen();
	lv_group_t* group = lv_group_create();
	lv_obj_set_user_data(ui_control_screen, &ui_control);
	lv_obj_add_event_cb(ui_control_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_control.focus_index = 0;

	uint16_t x = 0, y = 15, step = 32;	

	lv_obj_t* obj = ui_helpers_create_label(ui_control_screen, "TEMP CNTRL", &lv_font_montserrat_20);
	lv_obj_set_pos(obj, 50, y + 5);	
	lv_group_add_obj(group, obj);
	ui_control.group = group;
	ui_control.group_obj = obj;
	
	lv_obj_add_event_cb(obj, ui_control_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(obj, ui_control_event_encoder_click_cb, LV_EVENT_RELEASED, NULL);

	y += step;

	obj = ui_helpers_create_label(ui_control_screen, LV_SYMBOL_PLAY, &lv_font_montserrat_12);
	ui_control.focus = obj;
	
	obj = ui_helpers_create_label(ui_control_screen, "TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_control_screen, "0˚C", &mono_regular_24);
	ui_control.actual_temperature = obj;
	lv_obj_set_pos(obj, 150, y);

	y += step;
	obj = ui_helpers_create_label(ui_control_screen, "PRG TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(0x84ff00), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_control_screen, "", &mono_regular_24);
	lv_label_set_text_fmt(obj, "%d ˚C", systemconfig.pcnt.programmed_temperature);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_set_style_pad_all(obj, 3, LV_PART_MAIN);
	// lv_obj_set_width(obj, 60);
	lv_group_focus_obj(obj);
	lv_obj_set_pos(obj, 150, y);
	ui_control.programmed_temperature = obj;

	lv_obj_set_pos(ui_control.focus, 35, y + 5);
	
	
	y += step;
	obj = ui_helpers_create_label(ui_control_screen, "Duty:", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_control_screen, "0%", &mono_regular_24);
	ui_control.duty = obj;
	lv_obj_set_pos(obj, 150, y);
	lv_timer_create(ui_control_timer_cb, 1000, NULL);

	y += step;
	obj = ui_helpers_create_label(ui_control_screen, "V BAT:", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_control_screen, "0V", &mono_regular_24);
	ui_control.v_bat = obj;
	lv_obj_set_pos(obj, 150, y);
	
	y += step;
	obj = ui_helpers_create_button(ui_control_screen, systemconfig.pcnt.enabled == 1? "ON": "OFF", 100, 40, 5, &lv_font_montserrat_14, ui_control_event_click_cb, NULL);
	ui_helpers_button_color(obj, systemconfig.pcnt.enabled == 1? 0x00ff00: 0xff0000, UI_FOREGROUND_COLOR, 0);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -20);
	ui_control.enabled = obj;

	lv_timer_create(ui_control_timer_cb, 1000, NULL);
}
