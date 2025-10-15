#include "ui.h"
#include "ui-helpers.h"
#include "ui-splash.h"
#include "ui-home.h"
#include "ui-pcnt.h"

ui_msg_t ui_msgbox;
ui_top_bar_t ui_top_bar;
lv_obj_t* ui_active_screen;
SCREEN_TYPE ui_current_screen;
SCREEN_TYPE ui_previous_screen;
char ui_temp_buffer[256];

void ui_transform_screen(SCREEN_TYPE screen, lv_scr_load_anim_t anim, uint16_t time)
{
	ESP_LOGI("UI", "Transform screen %d -> %d", ui_current_screen, screen);
	if (lv_obj_is_visible(ui_msgbox.window)) lv_obj_add_flag(ui_msgbox.window, LV_OBJ_FLAG_HIDDEN);
	ui_previous_screen = ui_current_screen;
	switch ((uint8_t)screen)
	{
	case SCREEN_HOME:
		ui_active_screen = ui_home_screen;
		break;
	case SCREEN_PCNT:
		ui_active_screen = ui_pcnt_screen;
		break;
	default:
		return;
	}
	ui_current_screen = screen;
	lv_obj_set_parent(ui_msgbox.window, ui_active_screen);
	lv_scr_load_anim(ui_active_screen, anim, time, 0, false);
}
void ui_messagebox_delay_timer(lv_timer_t * timer)
{
	lv_obj_add_flag(ui_msgbox.window, LV_OBJ_FLAG_HIDDEN);
}
void ui_show_messagebox(MESSAGEBOX_TYPE type, const char* msg, uint16_t delay)
{
	lv_color_t color = lv_color_hex(UI_MSG_INFO_COLOR);
	if (type == MESSAGEBOX_WARNING) {
		color = lv_color_hex(UI_MSG_WARNING_COLOR);
	}
	else if (type == MESSAGEBOX_ERROR) {
		color = lv_color_hex(UI_MSG_ERROR_COLOR);
	}
	else {
		
	}
	lv_obj_set_style_bg_color(ui_msgbox.window, color, LV_PART_MAIN);
	lv_label_set_text(ui_msgbox.label, msg);
	
	if (delay > 0)
	{	
		lv_timer_t * timer = lv_timer_create(ui_messagebox_delay_timer, delay, ui_msgbox.window);
		lv_timer_set_repeat_count(timer, 1);		
	}
	lv_obj_set_width(ui_msgbox.window, LV_SIZE_CONTENT);
	lv_obj_clear_flag(ui_msgbox.window, LV_OBJ_FLAG_HIDDEN);
}
void ui_create_messagebox()
{
	ui_msgbox.window = lv_obj_create(lv_scr_act());
	lv_obj_clear_flag(ui_msgbox.window, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_border_width(ui_msgbox.window, 0, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(ui_msgbox.window, 240, LV_PART_MAIN);
	lv_obj_set_size(ui_msgbox.window, LV_PCT(80), 40);
	ui_msgbox.label = ui_helpers_create_label(ui_msgbox.window, (char*)"", &lv_font_montserrat_14);
	lv_obj_set_style_text_color(ui_msgbox.label, lv_color_hex(UI_MSG_FORECOLOR), LV_PART_MAIN);
	lv_obj_center(ui_msgbox.window);	
	lv_obj_add_flag(ui_msgbox.window, LV_OBJ_FLAG_HIDDEN);
}
void ui_create_top_bar()
{
	lv_obj_t* obj = lv_obj_create(lv_scr_act());
	lv_obj_set_size(obj, SCREEN_WIDTH, 30);
	lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_opa(obj, 150, LV_PART_MAIN);
	lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
	ui_top_bar.panel = obj;
    uint16_t x = 70, y = 2;
	obj = ui_helpers_create_label(ui_top_bar.panel, "", &lv_font_montserrat_24);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, 70, 20);
	ui_top_bar.notification = obj;
	
    x += 30;
	obj = ui_helpers_create_label(ui_top_bar.panel, LV_SYMBOL_WIFI, &lv_font_montserrat_24);
	lv_obj_set_pos(obj, x, y);
	ui_top_bar.wifi = obj;
	
    x += 30;
	obj = ui_helpers_create_label(ui_top_bar.panel, LV_SYMBOL_BLUETOOTH, &lv_font_montserrat_24);
	lv_obj_set_pos(obj, x, y);
	ui_top_bar.bluetooth = obj;
	
    x = SCREEN_WIDTH - 200;
	obj = ui_helpers_create_label(ui_top_bar.panel, LV_SYMBOL_CHARGE, &lv_font_montserrat_24);
	lv_obj_set_pos(obj, x, y);
	ui_top_bar.charge = obj;
	
    x += 30;
	obj = ui_helpers_create_label(ui_top_bar.panel, "100%% " LV_SYMBOL_BATTERY_FULL, &lv_font_montserrat_24);
	lv_obj_set_pos(obj, x, y);
	//lv_obj_set_width(obj, 70);
	lv_obj_set_style_text_align(obj, LV_TEXT_ALIGN_RIGHT, 0);
	ui_top_bar.battery = obj;
	lv_obj_add_flag(ui_top_bar.panel, LV_OBJ_FLAG_HIDDEN);
	
}

void ui_gesture_event_handler(lv_event_t * e)
{
	lv_dir_t gesture_dir = lv_indev_get_gesture_dir(lv_indev_get_act());

	switch (gesture_dir) {
	case LV_DIR_LEFT:
		ui_transform_screen(ui_previous_screen, LV_SCR_LOAD_ANIM_OVER_LEFT, 100);
		break;
	case LV_DIR_RIGHT:
		if (ui_current_screen != SCREEN_HOME)
		 	ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_OVER_LEFT, 100);
		break;
	case LV_DIR_TOP:
		// if (ui_current_screen != SCREEN_WATCH)
		// 	ui_transform_screen(SCREEN_W	ATCH, LV_SCR_LOAD_ANIM_OVER_TOP, 100);
		break;
	case LV_DIR_BOTTOM:
		// if (ui_current_screen != SCREEN_HOME)
		// 	ui_transform_screen(SCREEN_HOME, LV_SCR_LOAD_ANIM_OVER_TOP, 100);
		// break;
	default:
		break;
	}
}


void ui_update_timer(lv_timer_t * timer)
{
	
}
void ui_init( void )
{
	lv_disp_t *dispp = lv_disp_get_default();
	lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
	ui_splash_init();
	ui_home_init();
	ui_pcnt_init();
    ui_create_messagebox();
	// ui_create_top_bar();
	
	// lv_scr_load_anim(ui_splash_screen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
	lv_disp_load_scr(ui_splash_screen);
	
	// // Switch to the main application if OTA has not been started
	// vTaskDelay(pdMS_TO_TICKS(2000));
	
	// lv_scr_load_anim(ui_home_screen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
	lv_timer_create(ui_update_timer, 1000, NULL);
}


