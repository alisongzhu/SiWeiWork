/*
 * iic.h
 *
 *  Created on: 2018Äê7ÔÂ11ÈÕ
 *      Author: pt
 */

#ifndef IIC_H_
#define IIC_H_
#include "main.h"
#include "stm32f1xx_hal.h"
extern uint8_t iic_FRAM_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
extern uint8_t iic_FRAM_BufferRead(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToRead);

#endif /* IIC_H_ */
