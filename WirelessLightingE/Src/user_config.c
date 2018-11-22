/*
 * user_config.c
 *
 *  Created on: 2018年7月14日
 *      Author: pt
 */

 /* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>


#include "usart.h"
#include "user_config.h"
#include "flash.h"
#include "spi.h"
#include "spi_network.h"
#include "env.h"
#include "tim.h"
#include "port.h"
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "user_cmd.h"
#include "iic.h"
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
#define UARTTODMAMAXSIZE  500

#define CMD_Frame_Head 0xFE
#define CMD_Frame_Length  22
#define CMD_Frame_FLAG    0x1111
#define CHIPIDADDR        (uint32_t*)(0X1FFFF7E8)
/* Private constants ---------------------------------------------------------*/
enum {
	ReadParaCmd = 0x00000001,
	WriteParaCmd,
	CheckParaCmd,
	DevRestCmd,
	ReadDataCmd,
	DefaultPara
}CommandCode;
/* Private variables ---------------------------------------------------------*/
static struct sSystemProtcolFrame CummunictionFrame;
static uint32_t Rx_Count = 0;
/* Global variables ----------------------------------------------------------*/
uint8_t RecConfigParaSize=0;

uint8_t UartRx_buffer[UARTTODMAMAXSIZE] = { 0 };
/* Private function prototypes -----------------------------------------------*/
void test1(void);
/* Private functions ---------------------------------------------------------*/
static void UsartParseCmdFrame(struct sSystemProtcolFrame* RX_Frame)
{
	uint32_t cmd;
	memcpy((uint8_t*)&cmd, (uint8_t*)&(RX_Frame->Command), 4);
	switch (cmd)
	{
	case ReadParaCmd:
		memset(&CummunictionFrame.destinationAddress,0,12);
		memcpy(&CummunictionFrame.SourceAddress, CHIPIDADDR, 12);
		memcpy(&CummunictionFrame.Config_Parameter, &RTU_Device.Dev_config_data, sizeof(Device_Config_DataTypeDef));
		CummunictionFrame.Command = ReadParaCmd;
		CummunictionFrame.FrameCRC=usMBCRC16(( uint8_t*)&CummunictionFrame, sizeof(struct sSystemProtcolFrame)-2 );

		HAL_UART_Transmit(&huart1, (uint8_t*)&CummunictionFrame, sizeof(struct sSystemProtcolFrame), 1000);

		break;
	case WriteParaCmd:
		if ((0 == memcmp(&RX_Frame->destinationAddress, CHIPIDADDR, 12)))
				{


					memcpy(&RTU_Device.Dev_config_data, &RX_Frame->Config_Parameter, sizeof(Device_Config_DataTypeDef));
					RTU_Device.Dev_config_data.Config_flag=0;
					save_config(&RTU_Device.Dev_config_data);
					memset(&CummunictionFrame.destinationAddress,0,12);
					memcpy(&CummunictionFrame.SourceAddress, CHIPIDADDR, 12);
					memcpy(&CummunictionFrame.Config_Parameter, &RTU_Device.Dev_config_data, sizeof(Device_Config_DataTypeDef));
					CummunictionFrame.Command = ReadParaCmd;
					CummunictionFrame.FrameCRC=usMBCRC16( (uint8_t*)&CummunictionFrame, sizeof(struct sSystemProtcolFrame)-2 );


					sprintf(	user_usartTXBuffer," Device parameters set successfully\r\n");
					user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
					HAL_NVIC_SystemReset();/*配置完成复位*/
				}
				Rx_Count = 0;
				break;
		break;
	case CheckParaCmd:
		ParameterVerification(&hspi1, &RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara);
		RTU_Device.Dev_config_data.MeterConfig.MeterConfig_flag = CMD_Frame_FLAG;
		save_config(&RTU_Device.Dev_config_data);
		sprintf(	user_usartTXBuffer," Device check successfully\r\n");
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
		HAL_NVIC_SystemReset();/*配置完成复位*/
		break;
	case DevRestCmd:
		HAL_NVIC_SystemReset();/*配置完成复位*/
		break;
	case ReadDataCmd:

		break;
	case DefaultPara:
		memset(&RTU_Device.Dev_config_data,0xff, sizeof(Device_Config_DataTypeDef));
		save_config(&RTU_Device.Dev_config_data);
		sprintf(	user_usartTXBuffer," Device set successfully\r\n");
		user_log(user_usartTXBuffer,strlen(user_usartTXBuffer))
		HAL_NVIC_SystemReset();/*配置完成复位*/
		break;
	default:break;

	}

}





/* Exported functions --------------------------------------------------------*/
void UserConfig_Init(void)
{

//	MX_DMA_Init();
	MX_USART1_UART_Init();
	HAL_UART_Receive_DMA( &huart1, Usart1_DMARxdataBuffer, RxSize);
 __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
 command_add(user_Command1,USER_COMMAND1);
 command_add(user_Command2,USER_COMMAND2);
 command_add(user_paraPrintf,"readpara");
 command_add(user_ReadData,"readdata");
 command_add(user_readTemperature,"temperature");
 command_add(user_ClearEnergy,"clearenergy");
 command_add(user_MeterCheck,"Check");
 memset(&CummunictionFrame.destinationAddress,0,12);
 		memcpy(&CummunictionFrame.SourceAddress, CHIPIDADDR, 12);
 		memcpy(&CummunictionFrame.Config_Parameter, &RTU_Device.Dev_config_data, sizeof(Device_Config_DataTypeDef));
 		CummunictionFrame.Command = ReadParaCmd;
 		CummunictionFrame.FrameCRC=usMBCRC16(( uint8_t*)&CummunictionFrame, sizeof(struct sSystemProtcolFrame)-2 );

}

void UserHandle(void)
{
	//uint8_t i;

  //test1();
	if(RecConfigParaSize)
	{
		HAL_UART_DMAStop(&huart1);
		if(RecConfigParaSize>=sizeof(struct sSystemProtcolFrame))
		{
			//for(i=0;i<=(RecConfigParaSize-sizeof(struct sSystemProtcolFrame));i++)
			{
				if ( usMBCRC16(Usart1_DMARxdataBuffer, RecConfigParaSize ) == 0 )
				{
					UsartParseCmdFrame((struct sSystemProtcolFrame*)&Usart1_DMARxdataBuffer);
					//break;
				}
			}
			//HAL_UART_Transmit(&huart1, (uint8_t*)Usart1_DMARxdataBuffer, RecConfigParaSize, 1000);
		}
		else
		command_match((char*)Usart1_DMARxdataBuffer,RecConfigParaSize);
		HAL_UART_Receive_DMA(&huart1,(Usart1_DMARxdataBuffer),RxSize);
		RecConfigParaSize=0;
	}

}

void test1(void)
{
	char text[10]={'a','s','f',};

	char *out;cJSON *json;

		json=cJSON_Parse(text);
		if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else
		{
			out=cJSON_Print(json);
			cJSON_Delete(json);
			printf("%s\n",out);
			free(out);
		}
}

/**
  * @}
  */

  /**
	* @}
	*/


