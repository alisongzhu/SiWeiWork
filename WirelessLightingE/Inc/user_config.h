/*
 * user_config.h
 *
 *  Created on: 2018Äê7ÔÂ14ÈÕ
 *      Author: pt
 */

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "RN8302.h"
#include "device.h"

extern uint8_t RecConfigParaSize;

struct sSystemProtcolFrame{
	uint32_t destinationAddress[3];
	uint32_t SourceAddress[3];
	uint32_t Command;
	Device_Config_DataTypeDef Config_Parameter;
	uint16_t Reserve;
	uint16_t FrameCRC;

};

void UserHandle(void);

void UserConfig_Init(void);



#endif /* USER_CONFIG_H_ */
