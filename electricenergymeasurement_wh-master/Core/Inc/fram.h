/*
 * fram.h
 *
 *  Created on: 2018��11��26��
 *      Author: pt
 */

#ifndef CORE_USER_FRAM_H_
#define CORE_USER_FRAM_H_

#include "stm32f1xx_hal.h"
extern uint8_t iic_FRAM_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite);
extern uint8_t iic_FRAM_BufferRead(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToRead);
extern void FRAM_iicInit(void);

#endif /* CORE_USER_FRAM_H_ */
