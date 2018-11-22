/*
 * user_cmd.c
 *
 *  Created on: 2018年7月17日
 *      Author: pt
 */
/* Includes ------------------------------------------------------------------*/
#include "user_cmd.h"
#include "memory.h"
#include "usart.h"
#include <stdlib.h>
#include "usart.h"
#include "user_config.h"
#include "flash.h"
#include "spi.h"
#include "spi_network.h"
#include "env.h"
#include "tim.h"
#include "port.h"
#include "bsp_DS18B20.h"
#include "iic.h"
#include <stdio.h>
/* Private macros ------------------------------------------------------------*/

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
char user_usartTXBuffer[100];
extern char IOstring[][20] ;
/* Private function prototypes -----------------------------------------------*/

Cmd_listTypeDef  CommandLineHead={
		.nextnode=NULL,
		.CommandWord=0,
};
Cmd_listTypeDef* CommandLine=&CommandLineHead;

char user_Enter[2]={'\r','\n'};
/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/*******************************************
函数名称: Float2Char
函数功能:浮点数转字符串
参   数：无
返回 值: 无
********************************************/
void Float2Char(float value, uint8_t *array)
{
      uint16_t IntegerPart;  //整数部分
      float DecimalPart;//小数部分
      uint8_t i = 0;
      uint8_t j = 0;
      uint16_t temp;
      uint8_t bit = 0;
      //判断正负数
      if(value < 0)
      {
              value =0.0-value;
              array[0] = '-';
              bit = 1; //标记负数
              i++;
      }
      //分离整数和小数
      if(value >= 1)//大于1的浮点数
      {
              IntegerPart = (uint16_t)value;  //传入的值强制转换为整数
      }
      else
      {
              IntegerPart = 0;//小于1的浮点数
      }
      DecimalPart = value - IntegerPart;//获取小数部分
      //处理整数部分
      if(IntegerPart == 0) //小于1的浮点数
      {
              if(bit)//如果是负数
              {
                      array[1] = '0';
                      array[2] = '.';
                      i = 2;
              }
              else
              {
                      array[0] = '0';
                      array[1] = '.';
                      i = 1;
              }
      }
      else //大于1的浮点数
      {
              while(IntegerPart > 0)
              {
                      array[i++] = IntegerPart % 10 + '0'; //
                      IntegerPart /= 10;
              }
              i--;//退出来记录前一个位置


              //修正数据位置
              if(bit)//为负数的时候
              {
                      if(i%2==0)  //位数为偶数
                      {
                              for(j=1; j<=i/2; j++) //最中间的数字位置不变
                              {
                                      temp = array[j];
                                      array[j] = array[i+1 - j];
                                      array[i+1 - j] = temp;
                              }
                      }
                      else//位数为奇数
                              for(j=1; j<=(i-1)/2; j++)
                              {
                                      temp = array[j];
                                      array[j] = array[i+1 - j];
                                      array[i+1 - j] = temp;
                              }
              }
              else //为整数
                      if(i%2==0)  //位数为奇数
                      {
                              for(j=0; j<i/2; j++) //最中间的数字位置不变
                              {
                                      temp = array[j];
                                      array[j] = array[i - j];
                                      array[i - j] = temp;
                              }
                      }
                      else//位数为偶数
                              for(j=0; j<(i+1)/2; j++)
                              {
                                      temp = array[j];
                                      array[j] = array[i - j];
                                      array[i - j] = temp;
                              }
              array[++i] = '.';//添加小数点
      }
      //小数部分处理
      i++;
      array[i++] = (uint8_t)(DecimalPart * 10)%10 + '0';
      array[i++] = (uint8_t)(DecimalPart * 100)%10 + '0';
      array[i]   = '\0';
}
uint8_t command_add(void (*commandhabdle)(Cmd_listTypeDef*),char*CmdString)
{
	Cmd_listTypeDef* NewCommandLine=NULL;
	Cmd_listTypeDef* Command1;
	//uint8_t i;
	if(commandhabdle)
	{
		NewCommandLine=	malloc(sizeof(Cmd_listTypeDef));
		if(NewCommandLine)
		{
			for(Command1=CommandLine; ;Command1=Command1->nextnode)
			{
				if(Command1->nextnode==NULL)
				{
					NewCommandLine->CommandWord=Command1->CommandWord+1;
					NewCommandLine->CommandString=CmdString;
					NewCommandLine->commandhabdle=commandhabdle;
					NewCommandLine->nextnode=NULL;
					NewCommandLine->CommandStringSize=strlen((const char *)CmdString);
					Command1->nextnode=NewCommandLine;
					return TRUE;
				}
			}

		}

	}
	return FALSE;
}

uint8_t command_match(char*CmdString,uint8_t size)
{
	Cmd_listTypeDef* Command1;

	for(Command1=CommandLine;Command1!=NULL ;Command1=Command1->nextnode)
	{

		if(memcmp(Command1->CommandString,CmdString,size)==0)
		{
			if(Command1->commandhabdle != NULL )
			{
				(Command1->commandhabdle)(Command1);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void user_Command1(Cmd_listTypeDef* commanda)
{
	Cmd_listTypeDef* Command1;

	for(Command1=CommandLine;Command1!=NULL ;Command1=Command1->nextnode)
	{

		user_log(Command1->CommandString,Command1->CommandStringSize);
		user_log(user_Enter,2);
		if(Command1->nextnode==NULL)
			break;
	}
}

void user_Command2(Cmd_listTypeDef* Command1)
{
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);
	HAL_NVIC_SystemReset();

}

void user_paraPrintf(Cmd_listTypeDef* Command1)
{
	uint8_t i=0;
	uint8_t string[32]={0};
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);
	sprintf(	user_usartTXBuffer,"\r\nWPANState=%X  （WPAN_Succede=3）\r\n",RTU_Device.WPANState);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer));
	sprintf(	user_usartTXBuffer,"\r\nConfig_flag=%X\r\n",RTU_Device.Dev_config_data.Config_flag);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer));
	sprintf(	user_usartTXBuffer,"\r\nSwitch_Adrr=%u\r\n",RTU_Device.Dev_config_data.Switch_Adrr);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer));
	sprintf(	user_usartTXBuffer,"\r\nRouter_Level=%u\r\n",RTU_Device.Dev_config_data.Router_Level);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer));
	sprintf(	user_usartTXBuffer,"\r\nSwitch_Scense=%u\r\n",RTU_Device.Dev_config_data.Switch_Scense);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	sprintf(	user_usartTXBuffer,"\r\nwirelessChannel=%u\r\n",RTU_Device.Dev_config_data.E32_config.Channel);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	sprintf(	user_usartTXBuffer,"\r\nwirelessSending_Rate=%u\r\n",RTU_Device.Dev_config_data.E32_config.Sending_Rate);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	sprintf(	user_usartTXBuffer,"\r\nwirelessTranmist_Power=%u\r\n",RTU_Device.Dev_config_data.E32_config.Tranmist_Power);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))

	sprintf(	user_usartTXBuffer,"\r\nMeterConfig_flag=%lu\r\n",RTU_Device.Dev_config_data.MeterConfig.MeterConfig_flag);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))

	Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.CurrentThreshould,string);
	sprintf(	user_usartTXBuffer,"\r\nCurrentDeadZone=%s\r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))


	Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VoltageThreshould,string);
	sprintf(	user_usartTXBuffer,"\r\nVoltageThreshould=%s\r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))

	Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PowerThreshould,string);
	sprintf(	user_usartTXBuffer,"\r\nPowerThreshould=%s\r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	for(i=0;i<3;i++)
	{
		sprintf(	user_usartTXBuffer,"\r\nCIRmsConst=%lu\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.IRmsConst[i]);
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	for(i=0;i<3;i++)
	{
		sprintf(	user_usartTXBuffer,"\r\nVRmsConst=%lu\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VRmsConst[i]);
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	for(i=0;i<3;i++)
		{
			sprintf(	user_usartTXBuffer,"\r\nPRmsConst=%lu\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[i]);
			user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
		}
	for(i=0;i<3;i++)
	{
		Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.CurrentDeadZone[i],string);
		sprintf(	user_usartTXBuffer,"\r\nCurrentDeadZone=%s\r\n",string);
	    user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	for(i=0;i<3;i++)
		{
		    Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VoltageDeadZone[i],string);
			sprintf(	user_usartTXBuffer,"\r\nVoltageDeadZone=%s\r\n",string);
		    user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
		}
	for(i=0;i<3;i++)
			{
		Float2Char(RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PowerDeadZone[i],string);
				sprintf(	user_usartTXBuffer,"\r\nPowerDeadZone=%s\r\n",string );
			    user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
			}
	for(i=0;i<3;i++)
	{
	sprintf(	user_usartTXBuffer,"\r\nVGain=%X\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VGain[i]);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	for(i=0;i<3;i++)
		{
	sprintf(	user_usartTXBuffer,"\r\nIGain=%X\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.IGain[i]);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
		}

}
void user_readTemperature(Cmd_listTypeDef* Command1)
{
	float temp=0;
	uint8_t string[32];
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);

	temp=DS18B20_GetTemp_SkipRom();


	Float2Char(temp,string);
	sprintf(user_usartTXBuffer,"\r\nTemperature=%s\r\n",string);

	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
}

void user_ClearEnergy(Cmd_listTypeDef* Command1)
{
	float temp=0;
	int temp1=0;
	uint8_t string[32];
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);

	/*A phase*/
	iic_FRAM_BufferRead((uint8_t*)&temp, 10, 4);
	Float2Char(temp,string);
	sprintf(user_usartTXBuffer,"\r\nEnergy A=%s  \r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	temp1=0x0;
    iic_FRAM_BufferWrite((uint8_t*)&(temp1),10,4);
    HAL_Delay(20);
	/*B phase*/
	iic_FRAM_BufferRead((uint8_t*)&temp, 20, 4);
	Float2Char(temp,string);
	sprintf(	user_usartTXBuffer,"\r\nEnergy B=%s  \r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	temp1=0;
	iic_FRAM_BufferWrite((uint8_t*)&(temp1),20,4);
	 HAL_Delay(20);
	/*C phase*/
	iic_FRAM_BufferRead((uint8_t*)&temp, 30, 4);
	Float2Char(temp,string);
	sprintf(	user_usartTXBuffer,"\r\nEnergy C=%s  \r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	temp1=0;
	iic_FRAM_BufferWrite((uint8_t*)&(temp1),30,4);
    HAL_Delay(20);
	/*ABC phase*/
	iic_FRAM_BufferRead((uint8_t*)&temp, 40, 4);
	Float2Char(temp,string);
	sprintf(	user_usartTXBuffer,"\r\nEnergy ABC=%s  \r\n",string);
	user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	temp1=0;
	iic_FRAM_BufferWrite((uint8_t*)&(temp1),40,4);
	//EXIT_CRITICAL_SECTION( );
}

void user_ReadData(Cmd_listTypeDef* Command1)
{
	//float temp=0;
	uint8_t a=0;
	uint32_t i,j;
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);
	for(a=0;a<RN8302_READ_POINT_COUNT;a++)
	{
		i=(uint32_t)io[0][a];
		j=(uint32_t)(io[0][a]*100)%100;
		sprintf(	user_usartTXBuffer,"\r\nEnergy %s=%lu.%lu\r\n",IOstring[a],i,j);
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))

	}
}

void user_MeterCheck(Cmd_listTypeDef* Command1)
{
   uint8_t i;
	user_log(Command1->CommandString,Command1->CommandStringSize);
	user_log(user_Enter,2);
	ParameterVerification(&hspi1, &RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara);
	RTU_Device.Dev_config_data.MeterConfig.MeterConfig_flag = 0x1111;
	save_config(&RTU_Device.Dev_config_data);
	for(i=0;i<3;i++)
	{
	   sprintf(	user_usartTXBuffer,"\r\nVGain=%X\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VGain[i]);
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	for(i=0;i<4;i++)
	{
	   sprintf(	user_usartTXBuffer,"\r\nIGain=%X\r\n",RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.IGain[i]);
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
	}
	HAL_NVIC_SystemReset();
}

/**
 * @}
 */

/**
 * @}
 */

