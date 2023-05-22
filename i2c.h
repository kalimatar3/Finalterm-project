#include "stm32f10x.h"   // Thu vi?n STM32F10x
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_writeByte(uint8_t data);
void i2c_sendAddress(uint8_t address, uint8_t direction);
uint8_t i2c_readByte(uint8_t last);
void i2c_write(uint8_t address, uint8_t* data, uint16_t length);
void i2c_read(uint8_t address, uint8_t* data, uint16_t length);

void I2C_Init1(void);
void I2C_Send_Address1(uint8_t address, uint8_t direction);
