#include "stm32f10x.h"
#include "i2c.h"

#define LCD_ADDR 0x27
#define RS 0x01    // Register select bit
#define RW 0x02    // Read/write bit
#define EN 0x04    // Enable bit

// H�m tr?
void delay_ms(uint32_t ms) {
    while (ms--) {
        volatile uint32_t x = 14000;
        while (x--) ;
    }
}

// H�m g?i 4 bit th�ng di?p l?n lu?t qua c�c bit 4-7 c?a byte d? li?u
void lcd_send_nibble(uint8_t data, uint8_t flags) {
    data |= flags;
	
	/*	uint8_t *data_t;
		char data_u, data_l;
		data_u = (data&0xf0);
		data_l = ((data<<4)&0xf0);
		data_t[0] = data_u|0x0D;  //en=1, rs=0
		data_t[1] = data_u|0x09;  //en=0, rs=0
		data_t[2] = data_l|0x0D;  //en=1, rs=0
		data_t[3] = data_l|0x09;  //en=0, rs=0
		i2c_write(0x27, data_t ,4); */

    GPIOB->ODR = data;
    GPIOB->ODR |= EN;   // High pulse on enable pin
    delay_ms(1);
    GPIOB->ODR &= ~EN;  // Low pulse on enable pin
}

// H�m g?i 8 bit th�ng di?p l?n lu?t qua 2 nibble
void lcd_send_byte(uint8_t data, uint8_t flags) {
    lcd_send_nibble(data & 0xF0, flags);
    lcd_send_nibble((data << 4) & 0xF0, flags);
    delay_ms(1);		
	
}

// H�m thi?t l?p ch? d? ho?t d?ng c?a m�n h�nh LCD1602
void lcd_init() {
    // Set mode for GPIOB6 and GPIOB7
//    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
//    GPIOB->CRL |= (GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1);
 //   GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_MODE7);

    // Send initial commands
    i2c_writeByte(0x30);
    delay_ms(10);
    i2c_writeByte(0x30);
    delay_ms(1);
    i2c_writeByte(0x30);
    delay_ms(1);
    i2c_writeByte(0x20);
    delay_ms(1);

    // Function set
   i2c_writeByte(0x28);
    // Display on/off control
    i2c_writeByte(0x08);
    i2c_writeByte(0x01); // Clear display
    // Entry mode set
    i2c_writeByte(0x06);
    // Display on/off control
    i2c_writeByte(0x0C);
}

// H�m xo� b? nh? hi?n th? c?a m�n h�nh LCD1602
void lcd_clear() {
    lcd_send_byte(0x01, 0);
}

// H�m d?t con tr? d?c/ghi b? nh? hi?n th? t?i v? tr� x, y
void lcd_set_cursor(uint8_t x, uint8_t y) {
    uint8_t address = 0;
    switch (y) {
        case 0:
            address = 0x00;
            break;
        case 1:
            address = 0x40;
            break;
        case 2:
            address = 0x14;
            break;
        case 3:
            address = 0x54;
            break;
    }
    address += x;
    i2c_writeByte(0x80 | address);
}

// H�m in k� t? l�n m�n h�nh LCD1602
void lcd_putc(uint8_t data) {
    lcd_send_byte(data, RS);
}

// H�m in chu?i l�n m�n h�nh LCD1602
void lcd_puts(char *str) {
    while (*str) {
        lcd_send_byte(*str++, RS);
    }
}

/* int main() {
    // Kh?i t?o d?ng h? cho MCU
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    // Kh?i t?o ch�n GPIOB6 v� GPIOB7 d? l�m ch�n SCL v� SDA c?a k?t n?i I2C
    lcd_init();

    // Thi?t l?p con tr? t?i h�ng 0, c?t 0
    lcd_set_cursor(0, 0);
    // In chu?i "Hello, world!"
    lcd_puts("Hello, world!");

    while (1) {
        // Your code here
    }
} */
