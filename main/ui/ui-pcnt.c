#include "ui.h"
#include "ui-pcnt.h"
lv_obj_t* ui_pcnt_screen;

void ui_pcnt_init(void)
{
	ui_splash_screen = ui_helpers_create_screen();
	lv_obj_add_event_cb(ui_splash_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	
	lv_obj_t* obj = ui_helpers_create_label(ui_splash_screen, "Copyright 2025", &lv_font_montserrat_12);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -30);
	
	/// lv_timer_create(ui_splash_timer_cb, 1000, NULL);
}
