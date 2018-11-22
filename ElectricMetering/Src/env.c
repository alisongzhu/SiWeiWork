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
#include "user_config.h"
/* Private macros ------------------------------------------------------------*/
 
/* Private types -------------------------------------------------------------*/
 
/* Private constants ---------------------------------------------------------*/
 static const Config_ParameterTypeDef env_DefautPara={
                                             .Config_flag =0xff,
	                                           .MD_Adrr=DEFAULT_ADDRRESS ,
	                                           .Bsp_EquipmentType=DEFAULT_EquipmentType, 
	                                           .Firmware_version=FIRMWARE_VERSION ,
	                                             .RN8302FirmPara[0]={
																								 	.VRmsConst =1,
																									.IRmsConst =1,
																									.PRmsConst =1,
																								/*Sets the default parameter threshold*/
																									.VoltageThreshould=VOLTTHRESHOLDUINT,
																									.CurrentThreshould =CURRETHESHOLDUINT,
																									.PowerThreshould =1,
																									.FrequencyThreshould =1,
																									.IGain[0]=0,
																									.IGain[1]=0,
																								  .IGain[2]=0,
																									.VGain[0]=0,
																									.VGain[1]=0,
																									.VGain[2]=0,
																				 
																							 
																						 },
																							 .RN8302FirmPara[1]={
																							   	.VRmsConst =1,
																									.IRmsConst =1,
																									.PRmsConst =1,
																								/*Sets the default parameter threshold*/
																									.VoltageThreshould=VOLTTHRESHOLDUINT,
																									.CurrentThreshould =CURRETHESHOLDUINT,
																									.PowerThreshould =1,
																									.FrequencyThreshould =1,
																									.IGain[0]=0,
																									.IGain[1]=0,
																								  .IGain[2]=0,
																									.VGain[0]=0,
																									.VGain[1]=0,
																									.VGain[2]=0,
																							 
																						 }
 
 };
/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
uint8_t ParaRegAddressInChip[PARA_IN_RN8302_COUNT] = {0}; 
Config_ParameterTypeDef DevConfig_Parameter;
/* Private function prototypes -----------------------------------------------*/
 

/* Private functions ---------------------------------------------------------*/
 
 
/* Exported functions --------------------------------------------------------*/
void envUserParaLoad(void)
{
	LoadFlashConfig(&DevConfig_Parameter);/*Load default parameters */
	if(DevConfig_Parameter.Config_flag==0xFF)
	{
		DevConfig_Parameter=env_DefautPara;

	}
	  

}
