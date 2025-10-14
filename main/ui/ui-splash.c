#include "ui.h"
#include "ui-splash.h"
lv_obj_t* ui_splash_screen;

LV_IMG_DECLARE(img_hyrel_logo);

uint8_t ui_splash_wait_countdown = 2;
void ui_splash_timer_cb(lv_timer_t* timer)
 {
	if (lv_obj_is_visible(ui_splash_screen)) {
		ESP_LOGI("UI", "Splash wait %d", ui_splash_wait_countdown);
		if (ui_splash_wait_countdown == 0) ui_transform_screen(SCREEN_PCNT, LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 300);

		ui_splash_wait_countdown --;
	}
	
 }
void ui_splash_init(void)
{
	ui_splash_screen = ui_helpers_create_screen();
	lv_obj_add_event_cb(ui_splash_screen, ui_gesture_event_handler, LV_EVENT_GESTURE, NULL);
	
	lv_obj_t * img = lv_img_create(ui_splash_screen);
	lv_image_set_src(img, &img_hyrel_logo);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);

	lv_obj_t* obj = ui_helpers_create_label(ui_splash_screen, "Hyrel3D", &lv_font_montserrat_24);
	lv_obj_set_style_text_color(obj, lv_color_hex(0xff9a1c), LV_PART_MAIN);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -60);

	obj = ui_helpers_create_label(ui_splash_screen, "Copyright 2025", &lv_font_montserrat_12);
	lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, -30);
	
	/// lv_timer_create(ui_splash_timer_cb, 1000, NULL);
}
