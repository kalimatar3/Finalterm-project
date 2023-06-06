#include "stm32f10x.h"

void i2c_init(void)
{
    // B?t d?ng h? cho GPIOB, I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // C?u hình chân GPIO cho SCL và SDA, d?t chúng là ch? d? d?u ra open-drain (50MHz)
    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_MODE7);

    // C?u hình và kh?i t?o module I2C1
    I2C1->CR1 |= I2C_CR1_SWRST;               // Ð?t module I2C 1 vào tr?ng thái reset
    I2C1->CR1 &= ~I2C_CR1_SWRST;              // Ðua I2C1 ra kh?i tr?ng thái reset
    I2C1->CR2 |= 0x08;                        // Thi?t l?p t?c d? clock
    I2C1->CCR = 0x28;                         // Thi?t l?p giá tr? CCR cho 100kHz
    I2C1->TRISE = 9;                         // Thi?t l?p tr? TRISE cho 100kHz
    I2C1->CR1 |= I2C_CR1_PE;                  // Kích ho?t module I2C1
}

void i2c_start(void)
{
    // Ðua I2C1 vào ch? d? start, ch? d?n khi ho?t d?ng xong
    I2C1->CR1 |= I2C_CR1_START | I2C_CR1_ACK ;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c_stop(void)
{
    // Ðua I2C1 vào ch? d? stop, ch? d?n khi ho?t d?ng xong
    I2C1->CR1 |= I2C_CR1_STOP ;
    //while (I2C1->CR1 & I2C_CR1_STOP);
}

void i2c_writeByte(uint8_t data)
{
	  while (!(I2C1->SR1 & I2C_SR1_TXE));
    // Ghi d? li?u lên module I2C1
    I2C1->DR = data;
    // Ð?i d?n khi d? li?u du?c g?i xong
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void i2c_sendAddress(uint8_t address, uint8_t direction)
{
    // G?i d?a ch? c?a slave, d?t dúng hu?ng d? ghi hay d?c
    I2C1->DR = (address << 1) | direction;
    // Ð?i d?n khi d?a ch? du?c g?i xong
     while (!(I2C1->SR1 & (1u<<1) ) );   // Chú ý: SR1 và SR2 ph?i du?c d?c tru?c khi ghi vào DR
		uint8_t temp = I2C1->SR1 | I2C1->SR2;
}

void I2C_Write(uint8_t data) {
	while(!(I2C1->SR1 & (1u<<7))) {};  // Wait for TxE bit to set
  I2C1->DR = data;
  while(!(I2C1->SR1 & (1u<<2)));  // Wait for BTF bit to set
}

void I2C_Address(uint8_t address) {
	I2C1->DR = address;  // Send the address
  while (!(I2C1->SR1 & (1u<<1)));  // Wait for ADDR bit to set
  uint8_t temp = I2C1->SR1 | I2C1->SR2;  // Read SR1 and SR2 to clear the ADDR bit
}

void I2C_Read(uint8_t address, uint8_t *buffer, uint8_t size) {
	uint8_t remaining = size;
	
/**** STEP 1 ****/	
	if(size == 1)
	{
		/**** STEP 1-a ****/	
		I2C1->DR = address;  //  Send the address
		while (!(I2C1->SR1 & (1u<<1)));  // Wait for ADDR bit to set
		
		/**** STEP 1-b ****/	
		I2C1->CR1 &= ~I2C_CR1_ACK;  // Clear the ACK bit 
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // Read SR1 and SR2 to clear the ADDR bit.... EV6 condition
		I2C1->CR1 |= I2C_CR1_STOP;  // Stop I2C

		/**** STEP 1-c ****/	
		while (!(I2C1->SR1 & (1u<<6)));  // Wait for RxNE to set
		
		/**** STEP 1-d ****/	
		buffer[size-remaining] = I2C1->DR;  // Read the data from the DATA REGISTER
		
	}

/**** STEP 2 ****/		
	else 
	{
		/**** STEP 2-a ****/
		I2C1->DR = address;  // Send the address
		while (!(I2C1->SR1 & (1u<<1)));  // Wait for ADDR bit to set
		
		/**** STEP 2-b ****/
		uint8_t temp = I2C1->SR1 | I2C1->SR2;  // Read SR1 and SR2 to clear the ADDR bit
		
		while (remaining > 2)
		{
			/**** STEP 2-c ****/
			while (!(I2C1->SR1 & (1u<<6)));  // Wait for RxNE to set
			
			/**** STEP 2-d ****/
			buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer			
			
			/**** STEP 2-e ****/
			I2C1->CR1 |= I2C_CR1_ACK;  // Set the ACK bit to Acknowledge the data received
			
			remaining--;
		}
		
		// Read the SECOND LAST BYTE
		while (!(I2C1->SR1 & (1u<<6)));  // Wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;
		
		/**** STEP 2-f ****/
		I2C1->CR1 &= ~I2C_CR1_ACK;  // Clear the ACK bit 
		
		/**** STEP 2-g ****/
		I2C1->CR1 |= I2C_CR1_STOP;  // Stop I2C
		
		remaining--;
		
		// Read the Last BYTE
		while (!(I2C1->SR1 & (1u<<6)));  // Wait for RxNE to set
		buffer[size-remaining] = I2C1->DR;  // copy the data into the buffer
	}
}