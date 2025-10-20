#include "main.h"
#include "lcd.h"
#include "bsp/m5dial.h"

uint8_t lcd_brightness = 5;
void lcd_init() {
    ESP_LOGI(TAG, "LCD initialized");
    
    bsp_display_cfg_t cfg = {
        .lvgl_port_cfg = ESP_LVGL_PORT_INIT_CONFIG(),
        .buffer_size = BSP_LCD_H_RES * BSP_LCD_V_RES,
    };

    lv_display_t * disp = bsp_display_start_with_config(&cfg);
    bsp_display_backlight_on();
    bsp_display_rotate(disp, LV_DISPLAY_ROTATION_90);

}

void lcd_task_suspend(void) {
    lvgl_port_stop();
}
void lcd_task_resume(void) {
    lvgl_port_resume();
}