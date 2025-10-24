#include "ui.h"
#include "ui-settings.h"
lv_obj_t* ui_settings_screen;
UI_SETTINGS ui_settings;
UI_SETTINGS_DATA_ITEM ui_settings_data[] = {
	{"Temp Scale", &systemconfig.pcnt.temp_scale, VALUE_TYPE_FLOAT},
	{"BAT Scale", &systemconfig.pcnt.battery_scale, VALUE_TYPE_FLOAT},
	{"Duty Scale", &systemconfig.pcnt.duty_scale, VALUE_TYPE_FLOAT},
};
#define UI_SETTINGS_ITEM_SIZE 3
UI_SETTINGS_ITEM ui_settings_ui_items[UI_SETTINGS_ITEM_SIZE];
const uint8_t ui_settings_data_size = sizeof(ui_settings_data) / sizeof(UI_SETTINGS_DATA_ITEM);
///////////////////// SCREENS ////////////////////

void ui_settings_update_focus_items() {
	for (int i = 0, x = ui_settings.start_index; i < UI_SETTINGS_ITEM_SIZE; i ++, x ++) {
		UI_SETTINGS_ITEM* item = &ui_settings_ui_items[i];
		if (x >= ui_settings_data_size) {
			item->data_item = NULL;
			lv_label_set_text(item->title, "");
			lv_label_set_text(item->value, "");
		} else {
			item->data_item = &ui_settings_data[x];
			lv_label_set_text(item->title, item->data_item->title);
			switch (item->data_item->type)
			{
			case VALUE_TYPE_INT:
				lv_label_set_text_fmt(item->value, "%d", *(int*)item->data_item->value_ptr);
				break;
			case VALUE_TYPE_FLOAT:
				sprintf(ui_temp_buffer, "%.2f", *(float*)item->data_item->value_ptr);
				lv_label_set_text(item->value, ui_temp_buffer);
				break;
			case VALUE_TYPE_BOOL:
				lv_label_set_text_fmt(item->value, "%s", *(bool*)item->data_item->value_ptr == 1? "ON": "OFF");
				break;
			default:
				lv_label_set_text_fmt(item->value, "%s", item->data_item->value_ptr);
				break;
			}
		}
	}
	lv_label_set_text(ui_settings.up_focus, ui_settings.start_index == 0 ? "": LV_SYMBOL_UP);
	lv_label_set_text(ui_settings.down_focus, ui_settings.start_index > 0 && ui_settings.focus_index < ui_settings_data_size -1 ? LV_SYMBOL_DOWN: "");
	lv_obj_set_style_text_color(ui_settings.title, lv_color_hex(ui_settings.focus_index == ui_settings_data_size? 0x00ff00: 0xffffff), LV_PART_MAIN); 
}

void ui_settings_update_item(UI_SETTINGS_ITEM* ui_item, uint8_t status) {
	switch (status) {
	case 0: // NORMAL
		lv_obj_set_style_bg_color(ui_item->panel, lv_color_hex(UI_ITEM_NORMAL_BG_COLOR), LV_PART_MAIN);
		lv_obj_set_style_border_color(ui_item->panel, lv_color_hex(UI_ITEM_NORMAL_BR_COLOR), LV_PART_MAIN);
		break;
	case 1: // FOCUS
		lv_obj_set_style_bg_color(ui_item->panel, lv_color_hex(UI_ITEM_FOCUS_BG_COLOR), LV_PART_MAIN);
		lv_obj_set_style_border_color(ui_item->panel, lv_color_hex(UI_ITEM_FOCUS_BR_COLOR), LV_PART_MAIN);
		break;
	case 2: // SELECT
		lv_obj_set_style_bg_color(ui_item->panel, lv_color_hex(UI_ITEM_SELECT_BG_COLOR), LV_PART_MAIN);
		lv_obj_set_style_border_color(ui_item->panel, lv_color_hex(UI_ITEM_SELECT_BR_COLOR), LV_PART_MAIN);
		break;
	}
}

void ui_settings_update_value(UI_SETTINGS_ITEM* ui_item, int direction) {
	UI_SETTINGS_DATA_ITEM *item = &ui_settings_data[ui_settings.focus_index];
	int a;
	float b;
	bool c;
	switch (item->type)
	{
	case VALUE_TYPE_INT:
		*(int*)item->value_ptr += direction;
		lv_label_set_text_fmt(ui_item->value, "%d", *(int*)item->value_ptr);
		break;
	case VALUE_TYPE_FLOAT:
		*(float*)item->value_ptr += direction;
		sprintf(ui_temp_buffer, "%.2f", *(float*)(item->value_ptr));
		lv_label_set_text(ui_item->value, ui_temp_buffer);
		break;
	case VALUE_TYPE_BOOL:
		*(bool*)item->value_ptr = *(bool*)(item->value_ptr)? false: true;
		lv_label_set_text(ui_item->value, *(bool*)(item->value_ptr) ? "ON": "OFF");
		break;
	default:
		break;
	};
	save_configuration();
}

void ui_settings_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;

	if (key_code == LV_KEY_ENTER) {
		tone_play(6000, 200);
		if (ui_settings.select_index == ui_settings.focus_index) {
			ui_settings.select_index = -1;
		} else {
			ui_settings.select_index = ui_settings.focus_index;
			// ui_settings_update_item(&ui_settings_ui_items[ui_settings.focus_index - ui_settings.start_index], 2);	
		}
		if (ui_settings.focus_index != ui_settings_data_size) {
			ui_settings_update_item(&ui_settings_ui_items[ui_settings.focus_index -ui_settings.start_index], ui_settings.select_index == -1? 1: 2);	
		}
		return;
	} else {
		tone_play(direction == 1? 2000: 3000, 60);
		if (ui_settings.select_index >= 0) {
			if (ui_settings.select_index < ui_settings_data_size) {

			}
			if (ui_settings.select_index == ui_settings_data_size) {
				ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
				return;
			} else {
				// change value
				if (ui_settings.focus_index == ui_settings_data_size) {
					ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
				} else {
					ui_settings_update_value(&ui_settings_ui_items[ui_settings.focus_index - ui_settings.start_index], direction);
				}
			}
		} else {
			// change focus
			if (ui_settings.focus_index != ui_settings_data_size) {
				ui_settings_update_item(&ui_settings_ui_items[ui_settings.focus_index - ui_settings.start_index], 0);
			}
			
			ui_settings.focus_index += direction;
			if (ui_settings.focus_index > ui_settings_data_size ) ui_settings.focus_index = 0;
			else if (ui_settings.focus_index < 0) ui_settings.focus_index = ui_settings_data_size;

			if (ui_settings.focus_index >= UI_SETTINGS_ITEM_SIZE) 
				ui_settings.start_index = ui_settings.focus_index - UI_SETTINGS_ITEM_SIZE + 1;
			else ui_settings.start_index = 0;

			if (ui_settings.focus_index < ui_settings_data_size) {
				ui_settings_update_item(&ui_settings_ui_items[ui_settings.focus_index - ui_settings.start_index], 1);
			} else {
				ui_settings.start_index = ui_settings_data_size - UI_SETTINGS_ITEM_SIZE;
			}
			ui_settings_update_focus_items();
		}
	}
}

void ui_settings_init(void)
{
	ui_settings_screen = ui_helpers_create_screen();
	ui_settings.select_index = -1;
	ui_settings.start_index = 0;
	ui_settings.focus_index = 0;
	ui_settings.start_index = 0;
	ui_settings_screen = ui_helpers_create_screen();
	lv_obj_set_user_data(ui_settings_screen, &ui_settings);
	lv_obj_add_event_cb(ui_settings_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);

	lv_group_t* group = lv_group_create();
	ui_settings.group = group;
	lv_obj_t* obj = ui_helpers_create_label(ui_settings_screen, LV_SYMBOL_SETTINGS " Settings", &lv_font_montserrat_20);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 15);
	lv_obj_add_event_cb(obj, ui_settings_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_group_add_obj(group, obj);
	ui_settings.title = obj;
	
	uint16_t x = 0, y = 40, step = 45;
	uint8_t size = sizeof(ui_settings_data) / sizeof(UI_SETTINGS_DATA_ITEM);
	lv_obj_t* container = ui_helpers_create_panel(ui_settings_screen, 0x00000, false);
	lv_obj_set_size(container, lv_obj_get_width(ui_settings_screen) - 20, lv_obj_get_height(ui_settings_screen) - 90);
	lv_obj_set_pos(container, 10, 60);
	obj = ui_helpers_create_label(ui_settings_screen, LV_SYMBOL_UP, &lv_font_montserrat_12);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 45);
	ui_settings.up_focus = obj;
	obj = ui_helpers_create_label(ui_settings_screen, LV_SYMBOL_DOWN, &lv_font_montserrat_12);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -40);
	ui_settings.down_focus = obj;

	lv_obj_t* panel;
	y = 0;
	char buf[64];
	int xx[UI_SETTINGS_ITEM_SIZE] = {20, 5, 20};
	int ww[UI_SETTINGS_ITEM_SIZE] = {185, 210, 185};
	for (uint8_t i = 0; i < UI_SETTINGS_ITEM_SIZE; i++) {
		UI_SETTINGS_ITEM *ui_item = &ui_settings_ui_items[i];
		panel = ui_helpers_create_panel(container, 0x111111, false);
		lv_obj_set_style_border_width(panel, 1, LV_PART_MAIN);
		lv_obj_set_style_border_color(panel, lv_color_hex(0x222222), LV_PART_MAIN);
		
		lv_obj_set_size(panel, ww[i], 35);
		lv_obj_align(panel, LV_ALIGN_TOP_MID, 0, y);
		ui_item->panel = panel;
		obj = ui_helpers_create_label(panel, "Title: ", &lv_font_montserrat_16);
		lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);
		ui_item->title = obj;
		obj = ui_helpers_create_label(panel, "0.0", &lv_font_montserrat_20);
		lv_obj_align(obj, LV_ALIGN_RIGHT_MID, -20, 0);
		ui_item->value = obj;
		y += step;
	}

	ui_settings_update_focus_items();
}
