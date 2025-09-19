#include "i2c.h"
#include "main.h"

bool sensorReady = 0;
bool transmitterReady = 0;
I2C_HandleTypeDef hI2C;

void writeRegister(uint8_t addr, uint8_t regAddr, uint8_t value){
	uint8_t data[2] = {regAddr, value};
	HAL_I2C_Master_Transmit_DMA(&hI2C, (addr << 1), data, 2);
	while(!sensorReady);
	sensorReady = 0;
}

void readRegister(uint8_t addr, uint8_t regAddr, uint8_t *result, uint16_t size){
	uint8_t data[1] = {regAddr};
	HAL_I2C_Master_Sequential_Transmit_DMA(&hI2C, (addr << 1), data, 1, I2C_FIRST_FRAME);
	while(!sensorReady);
	sensorReady = 0;
	HAL_I2C_Master_Sequential_Receive_DMA(&hI2C, (addr << 1) | (0x01), result, size, I2C_LAST_FRAME);
	while(!transmitterReady);
	transmitterReady = 0;
}

void writeRegisters(uint8_t addr, uint8_t regAddr, uint8_t *value, uint16_t size){
	for(int i = 0; i < size; i++){
		writeRegister(addr, regAddr+i, value[i]);
	}
}

//PB6 = SCL1, PB7 = SDA1
void I2C_Init(void){
	hI2C.Instance = I2C1;
	hI2C.Init.OwnAddress2 = 0;
	hI2C.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hI2C.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hI2C.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hI2C.Init.OwnAddress1 = 0;
	hI2C.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hI2C.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hI2C.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	//hI2C.Init.Timing = 0x00100D19;
	hI2C.Init.Timing = 0x00100D14;
	if (HAL_I2C_Init(&hI2C) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_I2CEx_ConfigAnalogFilter(&hI2C, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
	{
		Error_Handler();
	}

    if (HAL_I2CEx_ConfigDigitalFilter(&hI2C, 0) != HAL_OK)
    {
    	Error_Handler();
    }
}
