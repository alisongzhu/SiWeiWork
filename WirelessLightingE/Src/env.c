/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
 
#include "env.h"
#include "flash.h"
#include "device.h"
/* Private macros ------------------------------------------------------------*/
 
/* Private types -------------------------------------------------------------*/
 
/* Private constants ---------------------------------------------------------*/
 const MeterConfig_ParameterTypeDef env_DefautPara={
                                             .MeterConfig_flag=0xffff,
	                                          .RN8302FirmPara={
	                                        		  .VRmsConst[0] =1,
													  .VRmsConst[1] =1,
													  .VRmsConst[2] =1,
													  .IRmsConst[0] =1,
													  .IRmsConst[1] =1,
													  .IRmsConst[2] =1,
													  .PRmsConst[0] =1,
													  .PRmsConst[1] =1,
													  .PRmsConst[2] =1,
													/*Sets the default parameter threshold*/
														.VoltageThreshould=VOLTTHRESHOLDUINT,
														.CurrentThreshould =CURRETHESHOLDUINT,
														.PowerThreshould =1,
														.FrequencyThreshould =1,
														.VoltageDeadZone[0]=50,
														.VoltageDeadZone[1]=50,
														.VoltageDeadZone[2]=50,
														.CurrentDeadZone[0]=0.01,
														.CurrentDeadZone[1]=0.01,
														.CurrentDeadZone[2]=0.01,
														.PowerDeadZone[0]=1,
														.PowerDeadZone[1]=1,
														.PowerDeadZone[2]=1,
														.IGain[0]=0,
														.IGain[1]=0,
														.IGain[2]=0,
														.VGain[0]=0,
														.VGain[1]=0,
														.VGain[2]=0,
														.HFConst1=0xd8d6,
																							 
																						 },
																			
 };
/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
uint8_t ParaRegAddressInChip[PARA_IN_RN8302_COUNT] = {0}; 
//MeterConfig_ParameterTypeDef DevConfig_Parameter;
/* Private function prototypes -----------------------------------------------*/
 

/* Private functions ---------------------------------------------------------*/
 
 
/* Exported functions --------------------------------------------------------*/
//void envUserParaLoad(void)
//{
//	LoadFlashConfig(&DevConfig_Parameter);/*Load default parameters */
//	if(DevConfig_Parameter.Config_flag==0xFF)
//	{
//		DevConfig_Parameter=env_DefautPara;
//
//	}
//
//
//}
