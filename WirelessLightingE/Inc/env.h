/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
#ifndef INC_ENV_H_
#define INC_ENV_H_

#include <stdint.h>
#include "flash.h"

#define PARA_IN_RN8302_COUNT    10




#define  Standard_Voltage     45531566//30198988//30358256//38100000//38447786//44000000()
#define  VIN_STA              220
#define  Standard_Current     25165824//25197551// 18874368//62914560//18874368//18000000
#define  CURRENT_STA          (1.5)//5 //1.5
#define  VOLTTHRESHOLDUINT    1 //(Standard_Voltage /VIN_STA  )     //  threshold unit 1V
#define  CURRETHESHOLDUINT    0.1f// (Standard_Current/(CURRENT_STA*10 ))  //  threshold unit 0.1A

extern uint8_t ParaRegAddressInChip[PARA_IN_RN8302_COUNT];
extern MeterConfig_ParameterTypeDef DevConfig_Parameter;

extern void envUserParaLoad(void);

#endif /* INC_ENV_H_ */
