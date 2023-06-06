#include "stm32f10x.h"
#include "i2c.h"

#define LCD_ADD 0x27

void delay_ms(uint32_t ms) 
	{
	while (ms--) 
		{
				volatile uint32_t x = 14000;
				while (x--) ;
		}
	}
void lcd_send_cmd (char cmd)
{
	i2c_start();
	i2c_sendAddress(LCD_ADD,0);
	char data_u, data_l;
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	i2c_writeByte(data_u|0x0C);  //en=1, rs=0
	i2c_writeByte(data_u|0x08);  //en=0, rs=0
	i2c_writeByte(data_l|0x0C);  //en=1, rs=0
	i2c_writeByte(data_l|0x08);  //en=0, rs=0
	i2c_stop();
}
void lcd_send_data (char data)
{
	i2c_start();
	i2c_sendAddress(LCD_ADD,0);
	char data_u, data_l;
	//uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	i2c_writeByte(data_u|0x0D);  //en=1, rs=0
	i2c_writeByte(data_u|0x09);  //en=0, rs=0
	i2c_writeByte(data_l|0x0D);  //en=1, rs=0
	i2c_writeByte(data_l|0x09);  //en=0, rs=0
	i2c_stop();
	//i2c_write(LCD_ADD,data_t,4);
}
void lcd_init (void)
{
	// 4 bit initialisation
	lcd_send_cmd (0x30);
	delay_ms(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	delay_ms(1);  // wait for >100us
	lcd_send_cmd (0x30); // kich hoat lcd
	delay_ms(10);
	lcd_send_cmd (0x20);  // 4bit mode
	delay_ms(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	delay_ms(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	delay_ms(1);
	lcd_send_cmd (0x01);  // clear display
	delay_ms(1);
	delay_ms(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	delay_ms(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}	
void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }
    lcd_send_cmd (col);
}
void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}
