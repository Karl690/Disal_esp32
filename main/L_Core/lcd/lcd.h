#pragma once

extern uint8_t lcd_brightness;
void lcd_init();
void lcd_set_brightness(uint8_t level);
int lcd_get_brightness(void);
void lcd_task_suspend(void);
void lcd_task_resume(void);
