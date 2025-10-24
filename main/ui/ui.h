#pragma once

#include "lvgl.h"
#include "main.h"
#include "ui-helpers.h"

#define SCREEN_WIDTH				240
#define SCREEN_HEIGHT				240 

#define UI_BACKGROUND_COLOR			0x000000
#define UI_FOREGROUND_COLOR			0xFFFFFF
#define UI_BUTTON_NORMAL_BG_COLOR		0x000baff
#define UI_BUTTON_NORMAL_FG_COLOR		0x000000
#define UI_BUTTON_NORMAL_BR_COLOR		0x31c6fe
#define UI_BUTTON_ACTIVE_BG_COLOR		0xfc565e
#define UI_BUTTON_ACTIVE_BR_COLOR		0xfc565e
#define UI_BUTTON_ACTIVE_FG_COLOR		0xffffff

#define UI_LABEL_COLOR						0x888888

#define UI_ITEM_NORMAL_BG_COLOR			0x111111
#define UI_ITEM_NORMAL_BR_COLOR			0x222222

#define UI_ITEM_FOCUS_BG_COLOR			0x111111
#define UI_ITEM_FOCUS_BR_COLOR			0x003b72

#define UI_ITEM_SELECT_BG_COLOR			0x0356a3
#define UI_ITEM_SELECT_BR_COLOR			0x0573da

#define UI_MSG_INFO_COLOR					0x8DC63F
#define UI_MSG_WARNING_COLOR				0xFBB040
#define UI_MSG_ERROR_COLOR					0xF15A29
#define UI_MSG_FORECOLOR					0x0


typedef enum
{
	SCREEN_HOME,
	SCREEN_CONTROL,
	SCREEN_SETTINGS,
	SCREEN_DIAGNOSTICS,
	SCREEN_BLE,
	SCREEN_WIFI,
} SCREEN_TYPE;

typedef enum
{
	MESSAGEBOX_WARNING,
	MESSAGEBOX_INFO,
	MESSAGEBOX_ERROR,
}MESSAGEBOX_TYPE;

typedef struct
{
	lv_obj_t* window;
	lv_obj_t* label;
}ui_msg_t;

typedef struct
{
	lv_obj_t* panel;
	lv_obj_t* battery;
	lv_obj_t* device; //ble, usb
	lv_obj_t* charge; //ble, usb
	lv_obj_t* bluetooth;
	lv_obj_t* wifi;
	lv_obj_t* notification;
}ui_top_bar_t;

typedef struct {
	lv_group_t* group;
    lv_obj_t* group_obj;
    lv_obj_t* focus;
    int8_t  focus_index;
	lv_obj_t* title;
} ui_base_t;

extern SCREEN_TYPE ui_current_screen;
extern char ui_temp_buffer[256];
void ui_init( void );
void ui_transform_screen(SCREEN_TYPE screen, lv_scr_load_anim_t anim, uint16_t time);
void ui_show_messagebox(MESSAGEBOX_TYPE type, const char* msg, uint16_t delay);
void ui_gesture_event_handler(lv_event_t * e);