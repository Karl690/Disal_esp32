// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.1.1
// LVGL VERSION: 8.2.0
// PROJECT: SquareLine_Project

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ui.h"
#include "ui_helpers.h"
//#include "images/resource_img.h"

#include "ui-splash.h"

lv_obj_t* ui_create_screen()
{
	lv_obj_t* screen = lv_obj_create(NULL);
	lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_bg_color(screen, lv_color_hex(UI_BACKGROUND_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	return screen;
}


lv_obj_t* ui_create_label(lv_obj_t* parent, const char* text, const lv_font_t* font)
{
	lv_obj_t* obj = lv_label_create(parent);	
	lv_obj_set_width(obj, LV_SIZE_CONTENT); /// 1
	lv_obj_set_height(obj, LV_SIZE_CONTENT); /// 1
	lv_label_set_recolor(obj, true);	
	lv_label_set_text_fmt(obj, "%s", text);	
	lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
	return obj;
}

lv_obj_t* ui_create_panel(lv_obj_t* parent, uint32_t color, bool scrollable)
{
	lv_obj_t* panel = lv_obj_create(parent);
	lv_obj_set_style_pad_all(panel, 0, LV_PART_MAIN);
	lv_obj_set_style_border_width(panel, 0, LV_PART_MAIN);
	if (!scrollable)
	{
		lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
	}
	lv_obj_set_style_bg_color(panel, lv_color_hex(color), LV_PART_MAIN);
	return panel;
}

void ui_textarea_set_readonly(lv_obj_t* obj, bool b)
{
	lv_textarea_set_cursor_click_pos(obj, !b); // Disable cursor positioning by click
	if (b)
	{
		lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE); // D	
	}
	else
	{
		lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE); // D	
	}
	
	
}

void ui_change_button_color(lv_obj_t* button, uint32_t bg, uint32_t fg)
{
	lv_obj_t* label = lv_obj_get_child(button, 0);
	if (label)	lv_obj_set_style_text_color(label, lv_color_hex(fg), LV_PART_MAIN);
	lv_obj_set_style_bg_color(button, lv_color_hex(bg), LV_PART_MAIN);
}

void ui_change_button_text(lv_obj_t* button, const char* text)
{
	lv_obj_t* label = lv_obj_get_child(button, 0);
	if (label)	lv_label_set_text(label, text);
}
lv_obj_t* ui_create_button(lv_obj_t* parent,
	const char* text,
	uint16_t w,
	uint16_t h, 
	uint16_t radius,
	const lv_font_t* font,
	lv_event_cb_t event_button_handler, 
	void* event_data)
{
	lv_obj_t* button = lv_btn_create(parent);
	
	lv_obj_set_width(button, w); /// 1
	lv_obj_set_height(button, h); /// 1
	
	//lv_obj_add_flag(button, LV_OBJ_FLAG_SCROLL_ON_FOCUS); /// Flags
	lv_obj_add_flag(button, LV_OBJ_FLAG_EVENT_BUBBLE);
	// lv_obj_clear_flag(button, LV_OBJ_FLAG_SCROLLABLE); /// Flags
	lv_obj_set_style_radius(button, radius, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(button, lv_color_hex(UI_BUTTON_NORMAL_BG_COLOR), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(button, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(button, 1, LV_PART_MAIN);
	
	lv_obj_t* label = lv_label_create(button);	
	//lv_obj_set_width(label, w-4); /// 1
	//lv_obj_set_pos(label, 0, 0);
	//lv_obj_set_height(label, h); /// 1
	lv_obj_center(label);
	//lv_obj_set_align(label, LV_ALIGN_CENTER);
	
	lv_obj_set_style_text_color(label, lv_color_hex(UI_BUTTON_NORMAL_FG_COLOR), LV_PART_MAIN);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	
	//lv_label_set_recolor(label, true);
	lv_label_set_text_fmt(label, text);
	lv_obj_set_style_text_font(label, font, LV_PART_MAIN | LV_STATE_DEFAULT);
	if (event_button_handler)	lv_obj_add_event_cb(button, event_button_handler, LV_EVENT_CLICKED, event_data);	
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
	
	return button;
}


void InitUI(void)
{
	lv_disp_t *dispp = lv_disp_get_default();
	lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
	lv_disp_set_theme(dispp, theme);
	ui_splash_screen_init();

	
	lv_scr_load_anim(ui_splash_screen, LV_SCR_LOAD_ANIM_FADE_ON, 300, 0, false);
}