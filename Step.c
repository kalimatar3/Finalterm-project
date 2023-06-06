#include "stm32f10x.h"
#include "MPU6050.h"
#include "lcd.h"
#include <stdio.h>
#include "GPIO.h"

void	StepCouting(int *stepcount)
{
	float g = MPU6050_Read_AccelZ() * 10;
	if(g >= 1.1 * 96 || g <= 0.9 * 96)
		{
			*stepcount = *stepcount + 1;
			delay_ms(400);
		}
		else
			{
				delay_ms(400);
			}
}