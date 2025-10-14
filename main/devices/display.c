#include "display.h"
#include <lvgl.h>
/*** Setup screen resolution for LVGL ***/
static const uint16_t screenWidth =  SCREEN_WIDTH;// TFT_WIDTH;
static const uint16_t screenHeight = SCREEN_HEIGHT;// TFT_HEIGHT;

static LGFX lcd; // declare display variable

static TaskHandle_t g_lvgl_task_handle;

bool force_touch = 0;
bool force_touched = 0;
uint16_t force_touchx, force_touchy;

esp_err_t InitLCDAndLVGL()
{
	// Setting display to landscape
	
	lcd.init();
#ifdef PORTRAIT
	
#else
	//if (lcd.width() < lcd.height()) lcd.setRotation(lcd.getRotation() ^ 2);
	lcd.setRotation(3);
#endif
	lcd.setColorDepth(16);	
	lcd.setBrightness(128);
	//lcd.fillScreen(TFT_BLACK);
	lv_init();
	lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    
	/* LVGL : Setting up buffer to use for display */
#if defined(LVGL_DOUBLE_BUFFER)
	//    EXT_RAM_BSS_ATTR lv_color_t * buf1 = (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));
	//     EXT_RAM_BSS_ATTR lv_color_t * buf2 = (lv_color_t *)malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));

	lv_color_t * buf1 = (lv_color_t *)heap_caps_malloc(SCREEN_WIDTH * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	lv_color_t * buf2 = (lv_color_t *)heap_caps_malloc(SCREEN_WIDTH * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	lv_display_set_buffers(disp, buf1, buf2, SCREEN_WIDTH * BUFF_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
	// lv_disp_draw_buf_init(&draw_buf, buf1, buf2, screenWidth * BUFF_SIZE);    
#else
	// EXT_RAM_BSS_ATTR 
	//lv_color_t * buf1 = (lv_color_t *)lv_mem_alloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t));
	lv_color_t * buf1 = (lv_color_t *)heap_caps_malloc(screenWidth * BUFF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
	lv_display_set_buffers(disp, buf1, NULL, SCREEN_WIDTH * BUFF_SIZE * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif
    lv_display_set_flush_cb(disp, display_flush);
	ESP_LOGI(TAG, "Deep color -----------------: %d", LV_COLOR_DEPTH);
	lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchpad_read);
	/*** LVGL : Setup & Initialize the display device driver ***/
	// static lv_disp_drv_t disp_drv;
	// lv_disp_drv_init(&disp_drv);
	// disp_drv.hor_res = screenWidth;
	// disp_drv.ver_res = screenHeight;
	// disp_drv.flush_cb = display_flush;
	// disp_drv.draw_buf = &draw_buf;
	// disp_drv.sw_rotate = 1;
	// disp = lv_disp_drv_register(&disp_drv);

	//*** LVGL : Setup & Initialize the input device driver ***
	// static lv_indev_drv_t indev_drv;
	// lv_indev_drv_init(&indev_drv);
	// indev_drv.type = LV_INDEV_TYPE_POINTER;
	// indev_drv.read_cb = touchpad_read;
	// lv_indev_drv_register(&indev_drv);

	/* Create and start a periodic timer interrupt to call lv_tick_inc */
	const esp_timer_create_args_t lv_periodic_timer_args = {
		.callback = &lv_tick_task,
		.arg = NULL,
		.dispatch_method = ESP_TIMER_TASK,
		.name = "periodic_gui",
		.skip_unhandled_events = true
	};
	esp_timer_handle_t lv_periodic_timer;
	ESP_ERROR_CHECK(esp_timer_create(&lv_periodic_timer_args, &lv_periodic_timer));
	ESP_ERROR_CHECK(esp_timer_start_periodic(lv_periodic_timer, LV_TICK_PERIOD_MS * 1000));

	int err = xTaskCreatePinnedToCore(gui_task, "lv gui", 1024 * 8, NULL, 5, &g_lvgl_task_handle, 0);
	if (!err)
	{
		//ESP_LOGE(TAG, "Create task for LVGL failed");
		if (lv_periodic_timer) esp_timer_delete(lv_periodic_timer);
		return ESP_FAIL;
	}
	return ESP_OK;
}

// Display callback to flush the buffer to screen
void display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
/// void display_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
	uint32_t w = (area->x2 - area->x1 + 1);
	uint32_t h = (area->y2 - area->y1 + 1);
	lcd.startWrite();
	lcd.setAddrWindow(area->x1, area->y1, w, h);
	lcd.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)px_map);
	lcd.endWrite();
	
	lv_disp_flush_ready(disp);
}

/* Setting up tick task for lvgl */
void lv_tick_task(void *arg)
{
	(void)arg;
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

void gui_task(void *args)
{
	while (1) {
		vTaskDelay(pdMS_TO_TICKS(10));
		lv_task_handler();
	}
}


// Touchpad callback to read the touchpad
void touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
// void touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
	uint16_t touchX = 0, touchY = 0;
	bool touched = 0;
	if (force_touch) 
	{
		// Manually issue a touch event
		touched = force_touched;
		touchX = force_touchx;
		touchY = force_touchy;
		
	}
	else
	{
		touched = lcd.getTouch(&touchX, &touchY);
	}
	
	
	if (!touched)
	{
		data->state = LV_INDEV_STATE_REL;
	}
	else
	{
		data->state = LV_INDEV_STATE_PR;

		// Set the coordinates
		data->point.x = touchX;
		data->point.y = touchY;
	}
	force_touch = false;
}
