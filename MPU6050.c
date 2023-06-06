#include "MPU6050.h"
#include "i2c.h"

int16_t Accel_X_RAW = 0;
int16_t Accel_Y_RAW = 0;
int16_t Accel_Z_RAW = 0;
int16_t Gyro_X_RAW = 0;
int16_t Gyro_Y_RAW = 0;
int16_t Gyro_Z_RAW = 0;
double curAccelZ = 0, preAccelZ = 0;
unsigned int thresholdCount = 0, stepCount = 0;

void MPU_Write (uint8_t Address, uint8_t Reg, uint8_t Data)
{
	i2c_start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	I2C_Write (Data);
	i2c_stop ();
}

void MPU_Read (uint8_t Address, uint8_t Reg, uint8_t *buffer, uint8_t size)
{
	i2c_start ();
	I2C_Address (Address);
	I2C_Write (Reg);
	i2c_start ();  // repeated start
	I2C_Read (Address+0x01, buffer, size);
	i2c_stop ();
}

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;

	// check device ID WHO_AM_I

	MPU_Read (MPU6050_ADDR, WHO_AM_I_REG, &check, 1);

	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0;
		MPU_Write (MPU6050_ADDR, PWR_MGMT_1_REG, Data);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		MPU_Write(MPU6050_ADDR, SMPLRT_DIV_REG, Data);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ± 2g
		Data = 0x00;
		MPU_Write(MPU6050_ADDR, ACCEL_CONFIG_REG, Data);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ± 250 °/s
		Data = 0x00;
		MPU_Write(MPU6050_ADDR, GYRO_CONFIG_REG, Data);
	}

}

double MPU6050_Read_AccelX(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_XOUT_H register

	MPU_Read (MPU6050_ADDR, ACCEL_XOUT_H_REG, Rec_Data, 2);

	Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);
	
	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/

	return Accel_X_RAW/16384.0 * 10;
}

double MPU6050_Read_AccelY(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_YOUT_H register

	MPU_Read (MPU6050_ADDR, ACCEL_YOUT_H_REG, Rec_Data, 2);

	Accel_Y_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register              ****/

	return Accel_Y_RAW/16384.0 * 10;
}

double MPU6050_Read_AccelZ(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from ACCEL_ZOUT_H register

	MPU_Read (MPU6050_ADDR, ACCEL_ZOUT_H_REG, Rec_Data, 2);

	Accel_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);


	return Accel_Z_RAW/16384.0 * 10;
}

double MPU6050_Read_GyroX(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_XOUT_H register

	MPU_Read (MPU6050_ADDR, GYRO_XOUT_H_REG, Rec_Data, 2);

	Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);


	return Gyro_X_RAW/131.0;
}

double MPU6050_Read_GyroY(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_YOUT_H register

	MPU_Read (MPU6050_ADDR, GYRO_YOUT_H_REG, Rec_Data, 2);

	Gyro_Y_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);


	return Gyro_Y_RAW/131.0;
}

double MPU6050_Read_GyroZ(void)
{
	uint8_t Rec_Data[2];

	// Read 2 BYTES of data starting from GYRO_ZOUT_H register

	MPU_Read (MPU6050_ADDR, GYRO_ZOUT_H_REG, Rec_Data, 2);

	Gyro_Z_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data [1]);


	return Gyro_Z_RAW/131.0;
}

int MPU6050_Counter(void) 
{
	curAccelZ = MPU6050_Read_AccelZ();
	
	if(__fabs(curAccelZ - preAccelZ) > 0.6)
		thresholdCount++;
	
	if(thresholdCount > 5) {
		stepCount++;
		thresholdCount = 0;
	}
	
	preAccelZ = curAccelZ;
	
	return stepCount;
}
