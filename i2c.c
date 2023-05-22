#include "stm32f10x.h"

#define I2C1_CLOCK_SPEED        100000
#define I2C_OWN_ADDRESS         0xA0
#define I2C_ADDRESS             0x50


#define I2C_SLAVE_ADDRESS  0x50
void i2c_init(void)
{
    // B?t d?ng h? cho GPIOB, I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // C?u hình chân GPIO cho SCL và SDA, d?t chúng là ch? d? d?u ra open-drain (50MHz)
    GPIOB->CRL &= ~(GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_CNF6_1 | GPIO_CRL_CNF7_1);
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_MODE7);

    // C?u hình và kh?i t?o module I2C1
    I2C1->CR1 = 0;
    I2C1->CR1 |= I2C_CR1_SWRST;               // Ğ?t module I2C 1 vào tr?ng thái reset
    I2C1->CR1 &= ~I2C_CR1_SWRST;              // Ğua I2C1 ra kh?i tr?ng thái reset
    I2C1->CR2 |= 0x08;                        // Thi?t l?p t?c d? clock
    I2C1->CCR = 0x28;                         // Thi?t l?p giá tr? CCR cho 100kHz
    I2C1->TRISE = 9;                         // Thi?t l?p tr? TRISE cho 100kHz
    I2C1->CR1 |= I2C_CR1_ACK;                 // Cho phép s? d?ng ACK
    I2C1->CR1 |= I2C_CR1_PE;                  // Kích ho?t module I2C1
}

void i2c_start(void)
{
    // Ğua I2C1 vào ch? d? start, ch? d?n khi ho?t d?ng xong
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

void i2c_stop(void)
{
    // Ğua I2C1 vào ch? d? stop, ch? d?n khi ho?t d?ng xong
    I2C1->CR1 |= I2C_CR1_STOP;
    while (I2C1->CR1 & I2C_CR1_STOP);
}

void i2c_writeByte(uint8_t data)
{
    // Ghi d? li?u lên module I2C1
    I2C1->DR = data;
    // Ğ?i d?n khi d? li?u du?c g?i xong
    while (!(I2C1->SR1 & I2C_SR1_BTF));
}

void i2c_sendAddress(uint8_t address, uint8_t direction)
{
    // G?i d?a ch? c?a slave, d?t dúng hu?ng d? ghi hay d?c
    I2C1->DR = (address << 1) | direction;

    // Ğ?i d?n khi d?a ch? du?c g?i xong
    while (!(I2C1->SR1 & I2C_SR1_ADDR));   // Chú ı: SR1 và SR2 ph?i du?c d?c tru?c khi ghi vào DR

    // Ğ?c SR2 d? dua I2C1 ra kh?i tr?ng thái nh?n d?a ch?
    (void) I2C1->SR2;
}

uint8_t i2c_readByte(uint8_t last)
{
    uint8_t data = 0x00;

    // Thi?t l?p module I2C1 vào ch? d? d?c d? li?u
    I2C1->CR1 &= ~(I2C_CR1_ACK);
    if (last) {
        I2C1->CR1 |= I2C_CR1_STOP;
    }

    // Ğ?i d?n khi d? li?u du?c nh?n xong
    while (!(I2C1->SR1 & I2C_SR1_RXNE));

    // Ğ?c d? li?u và tr? v?
    data = I2C1->DR;

    return data;
}

void i2c_write(uint8_t address, uint8_t* data, uint16_t length)
{
    uint16_t i;
		i2c_start();
    // Chuy?n module I2C1 v? ch? d? ghi
    i2c_sendAddress(address, 0);
    // Ghi t?ng byte d? li?u
    for (i = 0; i < length - 1; i++) {
        i2c_writeByte(data[i]);
    }
    // K?t thúc truy?n d? li?u, dua module I2C1 ra kh?i tr?ng thái ghi
    i2c_stop();
}

void i2c_read(uint8_t address, uint8_t* data, uint16_t length)
{
    uint16_t i;

    // Chuy?n module I2C1 v? ch? d? d?c
    i2c_sendAddress(address, 1);

    // Ğ?c t?ng byte d? li?u
    for (i = 0; i < (length - 1); i++) {
        data[i] = i2c_readByte(0);
    }

    // Ğ?c byte d? li?u cu?i cùng và k?t thúc vi?c nh?n d? li?u
    data[length - 1] = i2c_readByte(1);
    i2c_stop();
}
