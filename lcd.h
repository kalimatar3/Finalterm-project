#include "stm32f10x.h"   // Thu vi?n STM32F10x
void delay_ms(uint32_t ms);
void lcd_init(void);
void lcd_send_cmd (char cmd);
void lcd_send_data (char data);
void lcd_put_cur(int row, int col);
void lcd_send_string (char *str);
