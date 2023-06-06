#include "stm32f10x.h" 

void InitGPIO()
	{
		//config A8 = Input
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; 
		GPIOA->CRH &= ~(0xF << 4);              
    GPIOA->CRH |= (0x8 << 4);   
		GPIOA->ODR = 0x01;
		
		GPIOA->CRH &= ~(0xF << 8);            
    GPIOA->CRH |= (0x8 << 8);   
		GPIOA->ODR = 0x01;

		
		//config LED
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
		GPIOC->CRH &= ~GPIO_CRH_CNF13;   
		GPIOC->CRH |= GPIO_CRH_MODE13;  
	}
