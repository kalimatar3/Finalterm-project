#include "stm32f10x.h"   // Thu vi?n STM32F10x
#include "i2c.h"
#include "lcd.h"

#define I2C1_CLOCK_SPEED        100000
#define I2C_OWN_ADDRESS         0xA0
#define I2C_ADDRESS             0x50

int main(void)
{		
		// B?t d?ng h? cho GPIO PortC
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	
		// C?u hình chân PC13 là d?u ra t?c d? trung bình (10MHz)
		GPIOC->CRH |= GPIO_CRH_MODE13;   // Ð?t MODE13[1:0] là 01 (tuong ?ng v?i t?c d? 10MHz)
		GPIOC->CRH &= ~GPIO_CRH_CNF13;   // Ð?t CNF13[1:0] là 00 (tuong ?ng v?i ch? d? push-pull)
	
		i2c_init();
		lcd_init();			 
   while (1)
		 {		
			 lcd_put_cur(0,0);
			 lcd_send_string("tao da khoc");
			 delay_ms(1000);
        // B?t LED
        GPIOC->BSRR |= GPIO_BSRR_BS13;    // Ðua chân PC13 v? m?c Logic Low (0V)
				delay_ms(1000);
				// T?t LED
        GPIOC->BSRR |= GPIO_BSRR_BR13;    // Ðua chân PC13 v? m?c Logic High (3.3V)
				delay_ms(1000);
			} 
}
