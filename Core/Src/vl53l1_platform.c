/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"
#include "i2c.h"
#include <string.h>
#include <time.h>
#include <math.h>

/* Timeout for I2C transactions (ms) */
#define VL53L1_I2C_TIMEOUT_MS 1000

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	HAL_StatusTypeDef hal_status;
	hal_status = HAL_I2C_Mem_Write(&hi2c3, dev, index, I2C_MEMADD_SIZE_16BIT, pdata, (uint16_t)count, VL53L1_I2C_TIMEOUT_MS);
	return (hal_status == HAL_OK) ? 0 : -1;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	HAL_StatusTypeDef hal_status;
	hal_status = HAL_I2C_Mem_Read(&hi2c3, dev, index, I2C_MEMADD_SIZE_16BIT, pdata, (uint16_t)count, VL53L1_I2C_TIMEOUT_MS);
	return (hal_status == HAL_OK) ? 0 : -1;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	return VL53L1_WriteMulti(dev, index, &data, 1);
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	uint8_t buf[2];
	buf[0] = (uint8_t)(data >> 8);
	buf[1] = (uint8_t)(data & 0xFF);
	return VL53L1_WriteMulti(dev, index, buf, 2);
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	uint8_t buf[4];
	buf[0] = (uint8_t)(data >> 24);
	buf[1] = (uint8_t)((data >> 16) & 0xFF);
	buf[2] = (uint8_t)((data >> 8) & 0xFF);
	buf[3] = (uint8_t)(data & 0xFF);
	return VL53L1_WriteMulti(dev, index, buf, 4);
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	return VL53L1_ReadMulti(dev, index, data, 1);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	uint8_t buf[2];
	if (VL53L1_ReadMulti(dev, index, buf, 2) != 0) return -1;
	*data = (uint16_t)((buf[0] << 8) | buf[1]);
	return 0;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	uint8_t buf[4];
	if (VL53L1_ReadMulti(dev, index, buf, 4) != 0) return -1;
	*data = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | (uint32_t)buf[3];
	return 0;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	HAL_Delay((uint32_t)wait_ms);
	return 0;
}
