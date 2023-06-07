#include "stm32f10x.h"   // Thu vi?n STM32F10x
#include "i2c.h"
#include "MPU6050.h"
#include "lcd.h"
#include <stdio.h>
#include "step.h"
#include "GPIO.h"
char buffer3[4];
int step;
int mode;
void EXTI_A9_config(void)
{
		RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    // Config EXTI line 8 tuong ?ng v?i ch�n A8
    AFIO->EXTICR[2] &= ~(AFIO_EXTICR3_EXTI9);
    AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PA;

    // Reset c? pending
    EXTI->PR |= EXTI_PR_PR9;

    // Ch?n s? ki?n ng?t c?nh xu?ng (falling edge)
    EXTI->FTSR |= EXTI_FTSR_TR9;

    // V� hi?u h�a s? ki?n ng?t c?nh l�n (rising edge)
    EXTI->RTSR &= ~(EXTI_FTSR_TR9);

    // Cho ph�p ng?t line 8
    EXTI->IMR |= EXTI_IMR_MR9;

    // V� hi?u h�a ng?t line 8 li�n quan d?n s? ki?n
    EXTI->EMR &= ~(EXTI_EMR_MR9);

    // K�ch ho?t ng?t line 8 ? NVIC
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI9_5_IRQHandler(void) {
    if ( (EXTI->PR & EXTI_PR_PR9)) 
		{ // Ki?m tra c? ng?t EXTI8 d� du?c set hay chua
				 mode = (mode +1) %2;
			delay_ms(100);
        EXTI->PR = EXTI_PR_PR9; // X�a c? ng?t EXTI8 b?ng c�ch ghi l?i gi� tr? 1 l�n bit 8 c?a EXTI_PR
    }
}
int main(void)
{		
		int count = 0;
		InitGPIO();
		i2c_init();
		MPU6050_Init();
		lcd_init();
		EXTI_A9_config();

	
		while (1)
		 {				 	 
			sprintf(buffer3,"%d",count);
			delay_ms(50);
			lcd_put_cur(0,0);
			lcd_send_data(mode + 48);

			lcd_put_cur(1,0);
			lcd_send_string("Steps : ");
			lcd_put_cur(1,9);
			lcd_send_string(buffer3);		
			if(mode == 0)
			{
			count = MPU6050_Counter();
			}
		} 
}