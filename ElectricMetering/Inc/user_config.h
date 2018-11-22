/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
#ifndef INC_USER_CONFIG_H_
#define INC_USER_CONFIG_H_
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "RN8302.h"




typedef struct{
	 uint8_t Config_flag;
	 uint8_t MD_Adrr;
	 uint8_t Bsp_EquipmentType;
	 uint8_t Firmware_version;
	 sRN8302FirmParaFile_TypeDef RN8302FirmPara[2];
}Config_ParameterTypeDef;

struct sSystemProtcolFrame{
	uint32_t destinationAddress[3];
	uint32_t SourceAddress[3];
	uint32_t Command;
	Config_ParameterTypeDef Config_Parameter;
};


void SaveFlashConfig(Config_ParameterTypeDef *Config_Parameter ); 
void LoadFlashConfig(Config_ParameterTypeDef *Config_Parameter);
void UserConfig_Init(void);

#endif /* INC_USER_CONFIG_H_ */
