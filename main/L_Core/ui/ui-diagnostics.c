#include "ui.h"
#include "ui-diagnostics.h"
#include "pcnt/pcnt.h"
#include "K_Core/taskmanager.h"
#include "K_Core/display/DisplayList.h"
#include "RevisionHistory.h"

lv_obj_t* ui_diag_screen;
UI_DIAG ui_diag;

#define UI_DIAG_ITEM_SIZE 9
DisplayVariableInfo* ui_diag_selected_vars = LcdDiagVarsTable1;
UI_DIAG_ITEM ui_diag_ui_items[UI_DIAG_ITEM_SIZE];
const uint8_t ui_diag_display_size = 2;
///////////////////// SCREENS ////////////////////

void ui_diag_refresh()
{
	bool isDone = false;
	for (int i = 0; i < UI_DIAG_ITEM_SIZE; i ++) {
		DisplayVariableInfo* varInfo = &ui_diag_selected_vars[i];
		UI_DIAG_ITEM* item = &ui_diag_ui_items[i];
		if (!varInfo->VariablePointer) {
			isDone = true;
			item->data_item = NULL;
			lv_label_set_text(item->title, "");
			lv_label_set_text(item->value, "");
		} else if (!isDone){
			item->data_item = varInfo;
			lv_label_set_text(item->title, varInfo->Label);
			switch (varInfo->FuncType)
			{
			case FUNC_INT32:
				lv_label_set_text_fmt(item->value, "%d", *(int*)varInfo->VariablePointer);
				break;
			case FUNC_FLOAT:
				sprintf(ui_temp_buffer, "%.2f", *(float*)varInfo->VariablePointer);
				lv_label_set_text(item->value, ui_temp_buffer);
				break;
			case FUNC_BOOLEAN:
				lv_label_set_text_fmt(item->value, "%s", *(bool*)varInfo->VariablePointer == 1? "ON": "OFF");
				break;
			default:
				lv_label_set_text_fmt(item->value, "%s", varInfo->VariablePointer);
				break;
			}
		}
	}
}

void ui_diag_encoder_rotary_cb(lv_event_t* e)
{
	lv_obj_t * obj = lv_event_get_target_obj(e);
	lv_event_code_t code = lv_event_get_code(e);
	uint32_t * key = (uint32_t*)lv_event_get_param(e);
	uint32_t key_code = *key;
	int direction = (key_code == LV_KEY_RIGHT) ? 1 : -1;

	if (key_code == LV_KEY_ENTER) {
		ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300);
	} else {
		ui_diag.focus_index += direction;
		if (ui_diag.focus_index >= ui_diag_display_size ) ui_diag.focus_index = 0;
		else if (ui_diag.focus_index < 0) ui_diag.focus_index = ui_diag_display_size -1;

		switch (ui_diag.focus_index)
		{
		case 0:
			ui_diag_selected_vars = LcdDiagVarsTable1;
			break;
		case 1:
			ui_diag_selected_vars = LcdDiagVarsTable2;
		default:
			break;
		}
		ui_diag_refresh();
	}
}

void ui_diag_init(void)
{
	ui_diag_screen = ui_helpers_create_screen();
	ui_diag.select_index = -1;
	ui_diag.start_index = 0;
	ui_diag.focus_index = 0;
	ui_diag.start_index = 0;
	
	ui_diag_screen = ui_helpers_create_screen();
	lv_obj_set_user_data(ui_diag_screen, &ui_diag);
	lv_obj_add_event_cb(ui_diag_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);

	lv_group_t* group = lv_group_create();
	ui_diag.group = group;
	lv_obj_t* obj = ui_helpers_create_label(ui_diag_screen, LV_SYMBOL_WARNING " " MajorStep, &lv_font_montserrat_20);
	lv_obj_align(obj, LV_ALIGN_TOP_MID, 0, 15);
	lv_obj_set_style_text_color(obj, lv_color_hex(UI_ITEM_NORMAL_FG_COLOR), LV_PART_MAIN);
	lv_obj_add_event_cb(obj, ui_diag_encoder_rotary_cb, LV_EVENT_KEY, NULL);
	lv_group_add_obj(group, obj);
	ui_diag.title = obj;
	
	uint16_t x = 0, y = 40, step = 20;
	
	lv_obj_t* panel;
	
	char buf[64];
	int height = 20;
	int xx[UI_DIAG_ITEM_SIZE] = {35, 20, 15, 10, 8, 10, 20, 35, 55};
	int ww[UI_DIAG_ITEM_SIZE] = {170, 185, 185, 185, 185, 185, 185, 140, 120};
	for (uint8_t i = 0; i < UI_DIAG_ITEM_SIZE; i++) {
		UI_DIAG_ITEM *ui_item = &ui_diag_ui_items[i];
		panel = ui_helpers_create_panel(ui_diag_screen, 0x0, false);
		lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
		// lv_obj_set_style_border_color(panel, lv_color_hex(0x222222), LV_PART_MAIN);
		
		lv_obj_set_size(panel, ww[i], 20);
		lv_obj_set_pos(panel, xx[i], y);
		ui_item->panel = panel;
		obj = ui_helpers_create_label(panel, "", &lv_font_montserrat_12);
		lv_obj_set_style_text_color(obj, lv_color_hex(0xffffff), LV_PART_MAIN);
		lv_obj_align(obj, LV_ALIGN_LEFT_MID, 10, 0);
		ui_item->title = obj;
		obj = ui_helpers_create_label(panel, "", &lv_font_montserrat_12);
		lv_obj_set_style_text_color(obj, lv_color_hex(0xffff00), LV_PART_MAIN);
		lv_obj_align_to(obj, ui_item->title, LV_ALIGN_OUT_LEFT_MID, 100, 0);
		ui_item->value = obj;
		y += step;
	}

	ui_diag_refresh();
}
