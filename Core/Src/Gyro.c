/*
 * Gyro.c
 *
 *  Created on: Apr 3, 2025
 *      Author: nana3231exc
 */

#include "Gyro.h"

static SPI_HandleTypeDef hspi;

static HAL_StatusTypeDef gyroHALStatus;

void gyro_init (){

	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_SPI5_CLK_ENABLE();

	GPIO_InitTypeDef pinf;

	pinf.Pin = GPIOF_SPI5_MISO_PIN | GPIOF_SPI5_MOSI_PIN | GPIOF_SPI5_SCK_PIN;
	pinf.Mode = GPIO_MODE_AF_PP;
	pinf.Pull = GPIO_NOPULL;
	pinf.Speed = GPIO_SPEED_FREQ_LOW;
	pinf.Alternate = GPIO_AF5_SPI5;
	HAL_GPIO_Init(GPIOF, &pinf);

	GPIO_InitTypeDef pinc;

	pinc.Pin = GPIOC_SPI5_NCS_MEMS;
	pinc.Mode = GPIO_MODE_OUTPUT_PP;
	pinc.Pull = GPIO_PULLDOWN;
	pinc.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &pinc);


  hspi.Instance = SPI5;
  hspi.Init.Mode = SPI_MODE_MASTER;
  hspi.Init.Direction = SPI_DIRECTION_2LINES;
  hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi.Init.NSS = SPI_NSS_SOFT;
  hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi.Init.CRCPolynomial = 10;



  if (HAL_SPI_Init(&hspi) != HAL_OK)
  {
	  gyro_verifyHal();
//		Error_Handler();
  }

}


void gyro_printDeviceId() {
	printf("The DataReturned is : %x \n", gyro_readRegister(WHO_AM_I));
}

void gyro_power() {
	uint8_t dataToSend =  0xF;// 0x8;
	gyro_configRegisters(CTRL_REG1, dataToSend);

	gyro_configRegisters(FIFO_CTRL_REG, 0);
	gyro_configRegisters(CTRL_REG4, 0b00100000);


	printf("Gyro is now powered on\n");
}

void gyro_getTempPrint() {
	printf("The temperature is %d°C \n", gyro_readRegister(OUT_TEMP) + 15);
}

void gyro_configRegisters(uint8_t reg, uint8_t dataToSend) {
	uint8_t commandToSend = (GYRO_WRITE | reg);
	uint16_t commandData = (dataToSend << 8) | commandToSend;
	uint16_t receivedData = 0x00;

	gyro_enableSlave();

	while(HAL_GPIO_ReadPin(GPIOC, GPIOF_SPI5_MISO_PIN) != GPIO_PIN_RESET);
	gyroHALStatus = HAL_SPI_TransmitReceive(&hspi, (uint8_t*) &commandData, (uint8_t*) &receivedData, 2, TESTING_TIMEOUT);

	gyro_verifyHal();

	gyro_disableSlave();
}

uint8_t gyro_readRegister(uint8_t reg){
	uint8_t commandToSend = (GYRO_READ | reg);
	uint16_t receivedData = 0x00;

	gyro_enableSlave();

	while(HAL_GPIO_ReadPin(GPIOC, GPIOF_SPI5_MISO_PIN) != GPIO_PIN_RESET);
	gyroHALStatus = HAL_SPI_TransmitReceive(&hspi, &commandToSend, (uint8_t*) &receivedData, 2, TESTING_TIMEOUT);

	gyro_verifyHal();

	uint16_t dataReturned = (0xFF00 & receivedData) >> 8;
	gyro_disableSlave();
	return dataReturned;
}


void gyro_verifyHal() {
	if (gyroHALStatus != HAL_OK) {
//		while (1);
	}
}

void gyro_enableSlave() {
	HAL_GPIO_WritePin(GPIOC, GPIOC_SPI5_NCS_MEMS, GPIO_PIN_RESET);
}

void gyro_disableSlave() {
	HAL_GPIO_WritePin(GPIOC, GPIOC_SPI5_NCS_MEMS, GPIO_PIN_SET);
}

int16_t gyro_getXAngularRate() {
	uint16_t xL = gyro_readRegister(OUT_X_L);
	uint16_t xH = gyro_readRegister(OUT_X_H);

	return (int16_t)(((uint16_t)(xH) << 8) | (uint16_t)(xL));;
}

int16_t get_filtered_xRate(int16_t bias) {
    return (int16_t) (gyro_getXAngularRate() - bias);
}
