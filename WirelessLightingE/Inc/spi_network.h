/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
#ifndef INC_SPI_NETWORK_H_
#define INC_SPI_NETWORK_H_


#include "flash.h"
#define RN8302_READ_POINT_COUNT   26

extern float io[2][RN8302_READ_POINT_COUNT];

void ParameterVerification(SPI_HandleTypeDef *hspi,sRN8302FirmParaFile_TypeDef *RN8302FirmPara);
void RN8302_Init(SPI_HandleTypeDef *hspi, sRN8302FirmParaFile_TypeDef RN8302FirmPara);
void RN8302SpiScanData(void);

#endif /* INC_SPI_NETWORK_H_ */
