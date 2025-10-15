#include "ui.h"
#include "pcnt/pcnt.h"
#include "ui-pcnt.h"

lv_obj_t* ui_pcnt_screen;
UI_PCNT ui_pcnt;

void ui_pcnt_timer_cb(lv_timer_t * timer)
{
	if (pcnt_info.enabled)
	{
		ui_helpers_label_set_nmuber(ui_pcnt.freq, pcnt_info.count);
		ui_helpers_label_set_nmuber(ui_pcnt.actual_temperature, (int)pcnt_info.temperature);
	}
}

void ui_pcnt_event_switch_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	int* enabled = (int*)lv_event_get_user_data(e);
	if (obj && enabled)
	{
		*enabled = lv_obj_has_state(obj, LV_STATE_CHECKED) ? 1 : 0;
		if (*enabled) {
			pcnt_start();
			ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Enabled", 1000);
		}
		else {
			pcnt_stop();
			ui_show_messagebox(MESSAGEBOX_INFO, "PCNT Disabled", 1000);
		}
	}
}

void ui_pcnt_event_click_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	bool b = (bool)lv_obj_get_user_data(obj);
	ESP_LOGI("UI", "PCNT Clicked %d", b);
	b = !b;
	lv_obj_set_user_data(obj, (void*)b);
	lv_obj_set_style_border_color(obj, lv_color_hex(b?0xff0000: 0x0), LV_PART_MAIN);
}

void ui_pcnt_event_rotary_cb(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_target_obj(e);
	ESP_LOGI(TAG, "PCNT Rotary %d", code);
	
	if (code == LV_EVENT_ROTARY)
	{
		int diff = lv_event_get_rotary_diff(e);
		int temp = ui_helpers_label_get_nmuber(obj);
		temp += diff;
		if (temp < 0) temp = 0;
		if (temp > 100) temp = 100;
		ui_helpers_label_set_nmuber(obj, temp);
		sprintf(ui_temp_buffer, "%d'c", temp);
		lv_label_set_text(obj, ui_temp_buffer);
		pcnt_info.temperature = (float)temp;
	}
}
void ui_pcnt_init(void)
{
	ui_pcnt_screen = ui_helpers_create_screen();
	lv_obj_add_event_cb(ui_pcnt_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);

	uint16_t x = 0, y = 15, step = 35;	

	lv_obj_t* obj = ui_helpers_create_label(ui_pcnt_screen, "PLUS COUNT", &lv_font_montserrat_20);
	lv_obj_set_pos(obj, 50, y + 5);

	y += step;

	obj = ui_helpers_create_label(ui_pcnt_screen, "PRG TEMP", &lv_font_montserrat_14);
	lv_obj_set_pos(obj, 40, y + 5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "30'c", &lv_font_montserrat_14);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_set_style_border_width(obj, 1, LV_PART_MAIN);
	lv_obj_set_style_border_color(obj, lv_color_hex(0xff0000), LV_PART_MAIN);
	lv_obj_set_style_pad_all(obj, 3, LV_PART_MAIN);
	lv_obj_set_width(obj, 60);
	lv_group_t* group = lv_group_get_default();
	
	lv_group_add_obj(group, obj);
	lv_obj_add_event_cb(obj, ui_pcnt_event_click_cb, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(obj, ui_pcnt_event_rotary_cb, LV_EVENT_ROTARY, NULL);
	lv_obj_set_pos(obj, 140, y + 5);
	ui_pcnt.programmed_temperature = obj;
	
	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "FREQ ", &lv_font_montserrat_14);
	lv_obj_set_pos(obj, 20, y + 5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0Hz", &lv_font_montserrat_14);
	ui_pcnt.freq = obj;
	lv_obj_set_pos(obj, 140, y);

	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "TEMP ", &lv_font_montserrat_14);
	lv_obj_set_pos(obj, 15, y + 5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0'C", &lv_font_montserrat_14);
	ui_pcnt.actual_temperature = obj;
	lv_obj_set_pos(obj, 140, y);

	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "Duty ", &lv_font_montserrat_14);
	lv_obj_set_pos(obj, 30, y + 5);
	obj = ui_helpers_create_label(ui_pcnt_screen, "0%", &lv_font_montserrat_14);
	ui_pcnt.duty = obj;
	lv_obj_set_pos(obj, 140, y);

	y += step;
	obj = ui_helpers_create_label(ui_pcnt_screen, "ENABLE ", &lv_font_montserrat_14);
	lv_obj_set_pos(obj, 55, y + 5);
	obj = lv_switch_create(ui_pcnt_screen);
	lv_obj_add_event_cb(obj, ui_pcnt_event_switch_cb, LV_EVENT_VALUE_CHANGED, &pcnt_info.enabled);
	lv_group_add_obj(group, obj);
	lv_group_focus_obj(obj);
	lv_obj_add_event_cb(obj, ui_pcnt_event_rotary_cb, LV_EVENT_ROTARY, NULL);
	lv_obj_set_pos(obj, 140, y);
	ui_pcnt.enabled = obj;

	lv_timer_create(ui_pcnt_timer_cb, 1000, NULL);
}
