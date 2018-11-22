/*
 * device.c
 *
 *  Created on: 6 14, 2018
 *      Author: PT
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdlib.h>
#include <string.h>
#include "mb.h"
#include "dma.h"

#include "mbrtu.h"
#include "mbcrc.h"
#include "flash.h"
#include "stdio.h"
#include "device.h"
#include "env.h"
#include "spi_network.h"
#include  "spi.h"
#include "bsp_DS18B20.h"
#include <stdlib.h>
/* Private macros ------------------------------------------------------------*/

#define E32_1W            0x30
#define E32_500           0x27
#define WIRELESS_MAX_SIZE 0x3a
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
const uint8_t WirelssModuleRead[3]={0xc1,0xc1,0xc1};
const uint8_t WirelssModuleType[3]={0xc3,0xc3,0xc3};
const uint8_t WirelssModuleRest[3]={0xc4,0xc4,0xc4};
unsigned char WirelssModuleDefaults[6]={0xc0,0x00,0x00,0x3B,0x17,0x44};//默认配置无线1
unsigned char E32_Type;/*模块类型*/	
#define MODBUS_BAUD_RATE  9600      /*默认波特率*/
#define MODBUS_PORT       0x01
#define WirelessUsart     (uint32_t)USART2
#define BUSUsart          (uint32_t)USART3
/* Global variables ----------------------------------------------------------*/
extern USHORT   usRegInputBuf[ ];//输入寄存器数组

extern USHORT   usRegHoldingBuf[ ];//保持继电器数组
extern unsigned char ucRegCoilsBuf[ ];
extern volatile UCHAR  ucRTUBuf[];
extern  const MeterConfig_ParameterTypeDef env_DefautPara;
/* Private function prototypes -----------------------------------------------*/
extern uint8_t Router_List_Addr_Find(uint8_t  Addr );
extern void DataDistributionToChilds(void);

/* Private functions ---------------------------------------------------------*/
void RandomDelay(uint16_t factor)
{
	uint8_t uid[12];
	uint8_t i;
	uint16_t seed=0;
	HAL_GetUID((uint32_t*)uid);
	for(i=0;i<12;i++)
	{
		seed+=uid[i];
	}
	seed+=HAL_GetTick()&0x0f;
	srand(seed);
	HAL_Delay(rand()%factor+10);
}
/**
 * @brief
 * @details
 * @param    none
 * @retval   none
 */
void WirelessModuleConfig(void)
{

	/*配置模块通信速率*/
	switch (RTU_Device.Dev_config_data.E32_config.Sending_Rate)
	{
	case   300:  WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x00;
	break;
	case  1200: WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x01;
	break;
	case  2400:  WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x02;
	break;
	case  4800:  WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x03;
	break;
	case  9600:  WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x04;
	break;
	case 19200: WirelssModuleDefaults[3] = (WirelssModuleDefaults[3] & (~0x07)) | 0x05;
	break;
	case 65535: RTU_Device.Dev_config_data.E32_config.Sending_Rate=4800;/*写入默认值*/
	break;


	}
	/*配置模块通信信道*/
	if (RTU_Device.Dev_config_data.E32_config.Channel > 0 && RTU_Device.Dev_config_data.E32_config.Channel < 32)
		WirelssModuleDefaults[4] = RTU_Device.Dev_config_data.E32_config.Channel;
	else
		RTU_Device.Dev_config_data.E32_config.Channel =WirelssModuleDefaults[4];/*写入默认值*/
	/*配置模块发射功率*/
	if (E32_Type == E32_1W)/*因为电源输出功率不够，所以1W的模块不能工作在30dB*/
		switch (RTU_Device.Dev_config_data.E32_config.Tranmist_Power)
		{
		case 30:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x00;
		break;
		case 27: WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x01;
		break;
		case 24:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x02;
		break;
		case 21:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x03;
		break;
		case 65535: RTU_Device.Dev_config_data.E32_config.Tranmist_Power=30;/*写入默认值*/
		break;
		}
	else
		switch (RTU_Device.Dev_config_data.E32_config.Tranmist_Power)
		{
		case 30:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x00;
		case 27:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x00;
		break;
		case  24: WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x01;
		break;
		case  21:  WirelssModuleDefaults[5] = (WirelssModuleDefaults[5] & (~0x03)) | 0x02;
		break;
		case 65535: RTU_Device.Dev_config_data.E32_config.Tranmist_Power=30;
		break;
		}
}


/*Initialize the module configuration*/
/**
 * @brief   Initialize the module
 * @details
 * @param    none
 * @retval   none
 */

void E32_Moudle_Config(void)
{
	uint8_t WirelssModulePara[6]={0};
	uint32_t time=HAL_GetTick();
	/*配置模块串口,波特率9600*/
	MX_USART2_UART_Init();
	RTU_Device.WirelessUsartHandle->Init.BaudRate=9600;
	HAL_UART_Init(RTU_Device.WirelessUsartHandle); 
	HAL_NVIC_DisableIRQ(USART2_IRQn);

	/*进入配置模式*/
	HAL_GPIO_WritePin(E32_M1_GPIO_Port,E32_M1_Pin|E32_M0_Pin,GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModuleRest,3,0xff);/*复位模块*/
	HAL_Delay(100);
	HAL_GPIO_WritePin(E32_M1_GPIO_Port,E32_M1_Pin|E32_M0_Pin,GPIO_PIN_SET);
	//while(HAL_GPIO_ReadPin( E32_AUX_GPIO_Port ,E32_AUX_Pin)!=GPIO_PIN_SET);/*等待模块空闲*/
	HAL_Delay(100);
	E32_Type=E32_1W;
	WirelessModuleConfig();/*根据写入FLASH数据配置模块*/
	HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModuleRead,3,0xff);
	HAL_UART_Receive(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModulePara,6,1000) ;
	while(memcmp(WirelssModulePara,WirelssModuleDefaults,6)!=0)
	{
		LED_POWER_OFF();
		HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModuleDefaults,6,0x1ff);
		HAL_UART_Receive(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModulePara,6,5000);
		if(memcmp(WirelssModulePara,WirelssModuleDefaults,6)==0)
		{
			RTU_Device.BridgeFlag=TRUE;
			break;
		}
		if(HAL_GetTick()>time+10000)
		{
			RTU_Device.BridgeFlag=FALSE;
			break;
		}
		HAL_Delay(100);
	}
	HAL_Delay(1000);
	HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,(uint8_t *)WirelssModuleRest,3,0xff);/*复位模块*/
	HAL_Delay(100);
	HAL_GPIO_WritePin(E32_M1_GPIO_Port,E32_M1_Pin|E32_M0_Pin,GPIO_PIN_RESET);
	HAL_UART_MspDeInit(RTU_Device.WirelessUsartHandle);
	RTU_Device.WirelessUsartHandle->gState= HAL_UART_STATE_RESET;
	LED_POWER_ON();
}
/***************HC155****************/
/**
 * @brief   read hc165 DI
 * @details
 * @param    none
 * @retval   none
 */
uint8_t pvScanInternalDiscrete(void)
{
	uint8_t i,tmp1=0;
	//    DiscreteInfo item;
	// pull down HC165_CLK
	HAL_GPIO_WritePin( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
	// pull down HC165
	HAL_GPIO_WritePin( HC165_S_L_GPIO_Port, HC165_S_L_Pin, GPIO_PIN_RESET);
	// enable HC165 shift once
	HAL_GPIO_WritePin( HC165_S_L_GPIO_Port, HC165_S_L_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
	for(i=0;i<8;i++){
		HAL_GPIO_WritePin( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
		if(HAL_GPIO_ReadPin(HC165_DI_GPIO_Port, HC165_DI_Pin))
		{
			tmp1|=0x01;
		}
		if(i<7)
			tmp1=tmp1<<1;
		HAL_GPIO_WritePin( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);

	}
	HAL_GPIO_WritePin( HC165_S_L_GPIO_Port, HC165_S_L_Pin, GPIO_PIN_SET);
	tmp1 = (tmp1 & 0x07) | ((~tmp1) & 0xf8);/*改变光耦输出的电平，和检测继电器统一，导通为高电平*/
	return tmp1;
}
/* Exported functions --------------------------------------------------------*/
void DeviceInit(void)
{   
	//Dev_config_dataTypeDef Dev_config_data;
	RTU_Device.WirelessUsartHandle=&huart2;
	RTU_Device.BusUsartHandle     =&huart3;
	RTU_Device.BusUsartState = UART_READY;
	RTU_Device.WirelessUsartState = UART_READY;

	load_configuration(&RTU_Device.Dev_config_data);//加载配置参数
	E32_Moudle_Config();
	//HAL_GPIO_WritePin(E32_M1_GPIO_Port,E32_M1_Pin|E32_M0_Pin,GPIO_PIN_SET);
	if(RTU_Device.Dev_config_data.Config_flag)/*是否配置过设备参数*/
	{
		eMBInit(MB_RTU,DEFAULT_ADDR,MODBUS_PORT, MODBUS_BAUD_RATE, MB_PAR_NONE);
		RTU_Device.Dev_config_data.Router_Level=Router_Primary;
		RTU_Device.Dev_config_data.Switch_Adrr=DEFAULT_ADDR;
	}
	else 
		eMBInit(MB_RTU, RTU_Device.Dev_config_data.Switch_Adrr, MODBUS_PORT, MODBUS_BAUD_RATE, MB_PAR_NONE);
	if(RTU_Device.Dev_config_data.MeterConfig.MeterConfig_flag==0xffffffff)
	{
		RTU_Device.Dev_config_data.MeterConfig=env_DefautPara;
	}
	//RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.P_PHS[2]=0x5050;
	/*loading devie parameters and chipid for usRegHoldingBuf */
	memcpy(&usRegHoldingBuf[E32_config_NUM ], &RTU_Device.Dev_config_data.E32_config,6);
	HAL_GetUID((uint32_t *) usRegInputBuf);

	usRegInputBuf[6] =RTU_Device.Dev_config_data.Router_Level;//读取路由级数
	/*DI state*/
	ucRegCoilsBuf[0]=pvScanInternalDiscrete();


	DS18B20_Init();

	/*电测初始化*/
	RN8302_Init( &hspi1,  RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara);

	/*DMA usart */

	//MX_DMA_Init();

	MX_USART3_UART_Init();
	MX_USART2_UART_Init();
	//	HAL_UART_Receive_DMA( RTU_Device.WirelessUsartHandle, Usart2_DMARxdataBuffer, RxSize);
	//	HAL_UART_Receive_DMA( RTU_Device.BusUsartHandle, Usart3_DMARxdataBuffer, RxSize);
	//	__HAL_UART_ENABLE_IT(RTU_Device.WirelessUsartHandle, UART_IT_IDLE);
	//	__HAL_UART_ENABLE_IT(RTU_Device.BusUsartHandle, UART_IT_IDLE);

	/*FreeModbus 使能*/
	eMBEnable();
	vMBPortSerialEnable(  TRUE,FALSE );
}
#define ReadParaAddr                  10000
void DataStreamPrecessing(void)
{
	USHORT          usCRC16;
	if(RTU_Device.Rx_Buffer.DataFlag==TRUE )
	{
		/*modbus data*/
		if(usMBCRC16(RTU_Device.Rx_Buffer.ucRTUBuf,RTU_Device.Rx_Buffer.size)==0)
		{
			memcpy(ucRTUBuf,RTU_Device.Rx_Buffer.ucRTUBuf,RTU_Device.Rx_Buffer.size);

			if(ucRTUBuf[MB_SER_PDU_ADDR_OFF]==RTU_Device.Dev_config_data.Switch_Adrr && ucRTUBuf[MB_SER_PDU_ADDR_OFF]!=DEFAULT_ADDR )/*Match the device address*/
			{
				//if(RTU_Device.Dev_config_data.Router_Level== Router_Primary)
				xMBPortEventPost(EV_FRAME_RECEIVED);
			}
			else if(ucRTUBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST )/*broadcast*/
			{
				ucRTUBuf[MB_SER_PDU_ADDR_OFF]=RTU_Device.Dev_config_data.Switch_Adrr;
				usCRC16 = usMBCRC16( ( UCHAR * ) ucRTUBuf, RTU_Device.Rx_Buffer.size-2 );
				ucRTUBuf[RTU_Device.Rx_Buffer.size-2] = ( UCHAR )( usCRC16 & 0xFF );
				ucRTUBuf[RTU_Device.Rx_Buffer.size-1] = ( UCHAR )( usCRC16 >> 8 );
				xMBPortEventPost(EV_FRAME_RECEIVED);
				if((RTU_Device.Dev_config_data.Router_Level==Router_Primary) &&RTU_Device.Router_Node_Head.Router_next != NULL)
					DataDistributionToChilds();

			}

			else if( Router_List_Addr_Find(ucRTUBuf[MB_SER_PDU_ADDR_OFF])==TRUE)
			{
				HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,(RTU_Device.Rx_Buffer.ucRTUBuf),RTU_Device.Rx_Buffer.size,250);
				//转发无线
			}


			if(RTU_Device.Rx_Buffer.ucRTUBuf[MB_SER_PDU_ADDR_OFF]== MB_ADDRESS_BROADCAST )/*broadcast*/
			{
				RandomDelay(5000);
			}
		}



		if( RTU_Device.BridgeFlag==TRUE)/*两个串口透传*/
		{
			if(RTU_Device.Rx_Buffer.channel == WirelessUsart  && ucRTUBuf[MB_SER_PDU_ADDR_OFF]!=RTU_Device.Dev_config_data.Switch_Adrr)
			{
				if(RTU_Device.BusUsartState == UART_TX_Busy)
				{
					HAL_Delay(__HAL_DMA_GET_COUNTER(RTU_Device.BusUsartHandle->hdmatx)+1);
				}
				HAL_UART_Transmit_DMA(RTU_Device.BusUsartHandle,(RTU_Device.Rx_Buffer.ucRTUBuf),RTU_Device.Rx_Buffer.size);
				RTU_Device.BusUsartState = UART_TX_Busy;
			}
			else if(RTU_Device.Rx_Buffer.channel == BUSUsart  && ucRTUBuf[MB_SER_PDU_ADDR_OFF]!=RTU_Device.Dev_config_data.Switch_Adrr)
			{
				if(RTU_Device.WirelessUsartState == UART_TX_Busy)
				{
					HAL_Delay(__HAL_DMA_GET_COUNTER(RTU_Device.WirelessUsartHandle->hdmatx)+1);
				}
				if(RTU_Device.Rx_Buffer.size > WIRELESS_MAX_SIZE)
		        {
		        	HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle,(RTU_Device.Rx_Buffer.ucRTUBuf),WIRELESS_MAX_SIZE);
		        	while(	RTU_Device.WirelessUsartState == UART_TX_Busy);
		        	HAL_Delay(200);
		        	HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle,(&(RTU_Device.Rx_Buffer.ucRTUBuf[WIRELESS_MAX_SIZE])),RTU_Device.Rx_Buffer.size-WIRELESS_MAX_SIZE);
		        }

				HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle,(RTU_Device.Rx_Buffer.ucRTUBuf),RTU_Device.Rx_Buffer.size);
				RTU_Device.WirelessUsartState = UART_TX_Busy;
			}
		}
	}
	if(RTU_Device.Rx_Buffer.DataFlag == TRUE)
	{
		HAL_UART_Transmit_DMA(&huart1,RTU_Device.Rx_Buffer.ucRTUBuf,RTU_Device.Rx_Buffer.size);
		RTU_Device.Rx_Buffer.DataFlag=FALSE;
		if(RTU_Device.Rx_Buffer.channel == WirelessUsart)
		{
			__HAL_UART_CLEAR_IDLEFLAG(RTU_Device.WirelessUsartHandle);
			HAL_UART_Receive_DMA(RTU_Device.WirelessUsartHandle, (Usart2_DMARxdataBuffer), RxSize);
		    __HAL_UART_ENABLE_IT(RTU_Device.WirelessUsartHandle, UART_IT_IDLE);/*空闲中断*/
		}
		else
		{
		  __HAL_UART_CLEAR_IDLEFLAG(RTU_Device.BusUsartHandle);
           HAL_UART_Receive_DMA(RTU_Device.BusUsartHandle, (Usart3_DMARxdataBuffer), RxSize);
	      __HAL_UART_ENABLE_IT(RTU_Device.BusUsartHandle, UART_IT_IDLE);/*空闲中断*/
		}

	}
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart == RTU_Device.BusUsartHandle)
	{
		RTU_Device.BusUsartState= UART_READY;


	}
	if (huart == RTU_Device.WirelessUsartHandle)
	{
		RTU_Device.WirelessUsartState= UART_READY;
	}
	__HAL_UART_CLEAR_FLAG(huart, UART_FLAG_TC);

}
/**
 * @}
 */

/**
 * @}
 */

