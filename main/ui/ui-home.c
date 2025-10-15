#include "ui.h"
#include "ui-home.h"
lv_obj_t* ui_home_screen;

void ui_home_init(void)
{
	ui_home_screen = ui_helpers_create_screen();
	lv_obj_add_event_cb(ui_home_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);

	uint16_t x = 0, y = 15, step = 35;

	/// lv_timer_create(ui_splash_timer_cb, 1000, NULL);
	lv_obj_t* obj = ui_helpers_create_label(ui_home_screen, "Copyright 2025", &lv_font_montserrat_12);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -30);
}
