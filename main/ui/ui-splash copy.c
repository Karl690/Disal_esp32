#include "ui.h"
#include "ui-splash.h"
lv_obj_t* ui_splash_screen;

lv_obj_t* label;
bool isToggle = false;
void ui_splash_button_cb(lv_event_t* e)
{
	isToggle = !isToggle;
	lv_label_set_text(label, isToggle ? "TOUCHED" : "NO TOUCH");
}

///////////////////// SCREENS ////////////////////
void ui_splash_screen_init(void)
{
	ui_splash_screen = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_splash_screen, LV_OBJ_FLAG_SCROLLABLE); /// Flagsui_splash_screen = lv_obj_create(NULL);
	lv_obj_clear_flag(ui_splash_screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_bg_color(ui_splash_screen, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui_splash_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	
	label = ui_create_label(ui_splash_screen, "NO TOUCH", &mono_regualr_20);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);
	lv_obj_t *btn = ui_create_button(ui_splash_screen, "TEST1", 80, 40, 3, &mono_regualr_20, ui_splash_button_cb, NULL);
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* banner = ui_create_label(ui_splash_screen, "HYREL 2025", &mono_regualr_20);
	lv_obj_align(banner, LV_ALIGN_BOTTOM_MID, 0, -20);
}
