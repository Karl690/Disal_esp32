#include "ui.h"
#include "pcnt/pcnt.h"
#include "ui-pcnt.h"

lv_obj_t* ui_pcnt_screen;
UI_PCNT ui_pcnt;

LV_FONT_DECLARE(mono_regular_24);

void ui_pcnt_timer_cb(lv_timer_t * timer)
{
	if (pcnt_info.enabled)
	{
		lv_label_set_text_fmt(ui_pcnt.freq, "%dHz", pcnt_info.count);
		lv_label_set_text_fmt(ui_pcnt.actual_temperature, "%d˚C", pcnt_info.temperature);
	}
}

void ui_pcnt_event_switch_cb(lv_event_t* e)
{
	tone_play(6000, 200);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	bool enabled = lv_obj_has_state(ui_pcnt.enabled, LV_STATE_CHECKED);
	pcnt_info.enabled = enabled ? 1 : 0;
	if (enabled) {
		pcnt_start();
		ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Enabled", 1000);
	}
	else {
		pcnt_stop();
		ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Disabled", 1000);
	}
}

void ui_pcnt_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;


	if (key_code != LV_KEY_RIGHT && key_code != LV_KEY_LEFT) return;
	tone_play(2000, 200);
	
	switch (ui_pcnt.focus_index)
	{
	case 0:	{
			int temp = systemconfig.pcnt.programmed_temperature;
			temp += direction;
			if (temp < 0) temp = 0;
			else if (temp > 100) temp = 100;
			lv_label_set_text_fmt(ui_pcnt.programmed_temperature, "%d˚C", temp);
			systemconfig.pcnt.programmed_temperature = temp;
			save_configuration();
		}
		break;
	case 1:
		{
			bool enabled = lv_obj_has_state(ui_pcnt.enabled, LV_STATE_CHECKED);
			enabled = !enabled;
			ui_helpers_set_switch_value(ui_pcnt.enabled, enabled);
			pcnt_info.enabled = enabled ? 1 : 0;
			if (enabled) {
				pcnt_start();
				ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Enabled", 1000);
			}
			else {
				pcnt_stop();
				ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Disabled", 1000);
			}
		}
		break;
	case 2:
		ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
		break;
	default:
		break;
	}

}

void ui_pcnt_event_encoder_click_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	ui_pcnt.focus_index++;
	if (ui_pcnt.focus_index > 2) ui_pcnt.focus_index = 0;
	tone_play(6000, 200);
	switch (ui_pcnt.focus_index)
	{
	case 0:
		lv_obj_set_y(ui_pcnt.focus, lv_obj_get_y(ui_pcnt.programmed_temperature) + 8);
		break;
	case 1:
		lv_obj_set_y(ui_pcnt.focus, lv_obj_get_y(ui_pcnt.enabled) + 8);
		break;
	case 2:
		lv_obj_set_y(ui_pcnt.focus, -20 );
		break;
	}
}
void ui_pcnt_init(void)
{
	ui_pcnt_screen = ui_helpers_create_screen();
	lv_group_t* group = lv_group_create();
	lv_obj_set_user_data(ui_pcnt_screen, &ui_pcnt);
	lv_obj_add_event_cb(ui_pcnt_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	ui_pcnt.focus_index = 0;

	uint16_t x = 0, y = 15, step = 35;	

	lv_obj_t* obj = ui_helpers_create_label(ui_pcnt_screen, "PLUS COUNT", &lv_font_montserrat_20);
	lv_obj_set_pos(obj, 50, y + 5);	
	lv_group_add_obj(group, obj);
	ui_pcnt.group = group;
	ui_pcnt.group_obj = obj;
	
	lv_obj_add_event_cb(obj, ui_pcnt_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_obj_add_event_cb(obj, ui_pcnt_event_encoder_click_cb, LV_EVENT_RELEASED, NULL);

	y += step + 10;

	obj = ui_helpers_create_label(ui_pcnt_screen, LV_SYMBOL_PLAY, &lv_font_montserrat_12);
	lv_obj_set_pos(obj, 35, y + 5);
	ui_pcnt.focus = obj;
	
	obj = ui_helpers_create_label(ui_pcnt_screen, "PRG TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(0x84ff00), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "", &mono_regular_24);
	lv_label_set_text_fmt(obj, "%d ˚C", systemconfig.pcnt.programmed_temperature);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_set_style_pad_all(obj, 3, LV_PART_MAIN);
	lv_obj_set_width(obj, 60);
	lv_group_focus_obj(obj);
	lv_obj_set_pos(obj, 150, y);
	ui_pcnt.programmed_temperature = obj;
	
	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "ENABLE: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(0x84ff00), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = lv_switch_create(ui_pcnt_screen);
	lv_obj_add_event_cb(obj, ui_pcnt_event_switch_cb, LV_EVENT_VALUE_CHANGED, &pcnt_info.enabled);
	lv_obj_set_pos(obj, 150, y-4);
	ui_pcnt.enabled = obj;
	
	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "FREQ: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0Hz", &mono_regular_24);
	ui_pcnt.freq = obj;
	lv_obj_set_pos(obj, 150, y);

	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "TEMP: ", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0˚C", &mono_regular_24);
	ui_pcnt.actual_temperature = obj;
	lv_obj_set_pos(obj, 150, y);

	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "Duty:", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_LABEL_COLOR), LV_PART_MAIN);
	lv_obj_set_pos(obj, 55, y+5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0%", &mono_regular_24);
	ui_pcnt.duty = obj;
	lv_obj_set_pos(obj, 150, y);
	lv_timer_create(ui_pcnt_timer_cb, 1000, NULL);
}
