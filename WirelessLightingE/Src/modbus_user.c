/*
 * device.c
 *
 *  Created on: 6 15, 2018
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
#include "mbport.h"
#include "mbutils.h"
#include "mbrtu.h"
#include "flash.h"
#include "stdio.h"
#include "bsp_DS18B20.h"
#include "device.h"
/* Private macros ------------------------------------------------------------*/
#define REG_INPUT_METER_START   11000
#define REG_INPUT_METER_NREGS   43
#define REG_INPUT_START    10000
#define REG_INPUT_NREGS    10
#define REG_HOLDING_START  10000
#define REG_HOLDING_NREGS  20
#define REG_COILS_START    10000
#define REG_COILS_SIZE     16
#define chipID_Num        0x06
#define SWITCH1_BIT       (1<<0)
#define SWITCH2_BIT       (1<<1)
#define SWITCH3_BIT       (1<<2)



#define  VOLTAGE_TYPE          1
#define  CURRENT_TYPE          2
#define  POWER_TYPE            3
#define  POWERFACTOR_TYPE      4
#define  ENERGY_TYPE           5
#define	 FREQUENCY             6
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t ListFuncType[26] = {
		VOLTAGE_TYPE  ,   /*A phase voltage effective value-1*/
		VOLTAGE_TYPE ,    /*B phase voltage effective value-1*/
		VOLTAGE_TYPE ,    /*C phase voltage effective value-1*/


		CURRENT_TYPE ,   /*A phase current effective value-1*/
		CURRENT_TYPE,    /*B phase current effective value-1*/
		CURRENT_TYPE,    /*C phase current effective value-1*/


		POWER_TYPE,   /*A phase active power-1*/
		POWER_TYPE,   /*B phase active power-1*/
		POWER_TYPE,   /*C phase active power-1*/
		POWER_TYPE,   /*ABC ALL phase active power-1*/


		POWER_TYPE ,   /*A phase reactive power-1*/
		POWER_TYPE,    /*B phase reactive power-1*/
		POWER_TYPE ,   /*C phase reactive power-1*/
		POWER_TYPE ,   /*ABC phase reactive power-1*/

		POWER_TYPE,   /*A phase Apparent power-1*/
		POWER_TYPE,   /*B phase Apparent power-1*/
		POWER_TYPE,  /*C phase  Apparent power-1 */
		POWER_TYPE,  /*ABC phase  Apparent  power-1*/

		POWERFACTOR_TYPE ,   /*A phase power factor-1*/
		POWERFACTOR_TYPE ,   /*B phase power factor-1*/
		POWERFACTOR_TYPE ,   /*C phase power factor-1*/

		ENERGY_TYPE,
		ENERGY_TYPE,
		ENERGY_TYPE,
		ENERGY_TYPE,

		FREQUENCY,           /*frequency*/
};
/* Global variables ----------------------------------------------------------*/
USHORT   usRegInputStart = REG_INPUT_START;//输入寄存器开始地址
USHORT   usRegInputBuf[REG_INPUT_NREGS];//输入寄存器数组

USHORT   usRegHoldingStart = REG_INPUT_START;//保持继电器开始地址
USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];//保持继电器数组

static USHORT   usRegHoldingMeterStart = REG_INPUT_METER_START;   //电表寄存器
USHORT   usRegHoldingMeterBuf[REG_INPUT_METER_NREGS] = { 0 };//

unsigned char ucRegCoilsBuf[REG_COILS_SIZE / 8]={0,0};//继电器线圈
extern  UCHAR    ucMBAddress;
extern  float io[2][26] ;
/* Private function prototypes -----------------------------------------------*/
static void LED_UP_Sta(void);
void SWITCH_Action(GPIO_TypeDef* SWITCH_OFF_GPIO_Port,GPIO_TypeDef* SWITCH_ON_GPIO_Port, uint16_t SWITCH_OFF_Pin, uint16_t SWITCH_ON_Pin, GPIO_PinState PinAction);
/* Private functions ---------------------------------------------------------*/
/**
 * @brief   开关动作
 * @details 开关打开，继电器关闭，
 * @param    none
 * @retval   none
 */
void SWITCH_Action(GPIO_TypeDef* SWITCH_OFF_GPIO_Port,GPIO_TypeDef* SWITCH_ON_GPIO_Port, uint16_t SWITCH_OFF_Pin, uint16_t SWITCH_ON_Pin, GPIO_PinState PinAction)
{
#ifdef  HFE7
	if (PinAction != RESET)
	{
		HAL_GPIO_WritePin(SWITCH_OFF_GPIO_Port, SWITCH_OFF_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SWITCH_ON_GPIO_Port, SWITCH_ON_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(SWITCH_OFF_GPIO_Port, SWITCH_OFF_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SWITCH_ON_GPIO_Port, SWITCH_ON_Pin, GPIO_PIN_RESET);
	}
#else 
	if (PinAction ==  GPIO_PIN_RESET)/*新版继电器*/
	{
		HAL_GPIO_WritePin(SWITCH_OFF_GPIO_Port, SWITCH_OFF_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SWITCH_ON_GPIO_Port, SWITCH_ON_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(SWITCH_OFF_GPIO_Port, SWITCH_OFF_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(SWITCH_ON_GPIO_Port, SWITCH_ON_Pin, GPIO_PIN_RESET);
	}
#endif
}
/*开关命令处理*/
/**
 * @brief   开关命令处理
 * @details 9600 8N1  0x17信道，433M
 * @param    none
 * @retval   none
 */

static void SwitchDeal(void)
{
	//static UCHAR SwitchNum = 0;
	//SwitchNum = pvScanInternalDiscrete() & 0x07;
	//SwitchNum=(GPIOB->IDR&(SWITCH1_STAT_Pin|SWITCH2_STAT_Pin|SWITCH3_STAT_Pin))>>3;//读取线圈状态
	//if(SwitchNum!=ucRegCoilsBuf[0])
	{
		// if((ucRegCoilsBuf[0]^SwitchNum)&SWITCH3_BIT)
		SWITCH_Action(SWITCH3_OFF_GPIO_Port,SWITCH3_ON_GPIO_Port, SWITCH3_OFF_Pin, SWITCH3_ON_Pin, (GPIO_PinState)(ucRegCoilsBuf[0] & SWITCH3_BIT));
		// if((ucRegCoilsBuf[0]^SwitchNum)&SWITCH1_BIT)
		SWITCH_Action(SWITCH1_OFF_GPIO_Port,SWITCH1_ON_GPIO_Port, SWITCH1_OFF_Pin, SWITCH1_ON_Pin, (GPIO_PinState)(ucRegCoilsBuf[0] & SWITCH1_BIT));
		//if((ucRegCoilsBuf[0]^SwitchNum)&SWITCH2_BIT)
		SWITCH_Action(SWITCH2_OFF_GPIO_Port,SWITCH2_ON_GPIO_Port, SWITCH2_OFF_Pin, SWITCH2_ON_Pin, (GPIO_PinState)(ucRegCoilsBuf[0] & SWITCH2_BIT));
	//	SwitchNum = ucRegCoilsBuf[0];
		__HAL_TIM_SET_COUNTER(&htim3, 0);
		HAL_TIM_Base_Start_IT(&htim3);//开启定时器产生控制产生脉冲
		LED_POWER_OFF();

	}
}

/**
 * @brief   命令处理
 * @details匹配CHIPID配置地址
 * @param    none
 * @retval   none
 */
void Config_Cmd_Deal(USHORT usAddress)
{


	if (usAddress == REG_HOLDING_START + 1)
	{

		if (memcmp((UCHAR*)usRegInputBuf, (UCHAR*)usRegHoldingBuf, 12) == 0) //匹配CHIPID
		{

			ucMBAddress = (UCHAR)usRegHoldingBuf[chipID_Num];//写入地址
			RTU_Device.Dev_config_data.Switch_Adrr = ucMBAddress;
			RTU_Device.Dev_config_data.Config_flag = RESET;
			RTU_Device.Dev_config_data.Router_Level = (UCHAR)usRegHoldingBuf[chipID_Num + 1];//写入路由级数
			save_config(&(RTU_Device.Dev_config_data));/*保存设备参数到FLASH*/

			HAL_TIM_Base_Start_IT(&htim3);//开启定时器产生控制LED
			LED_POWER_OFF();

		}

	}

	else if (usAddress == (REG_HOLDING_START + E32_config_NUM + 1))/*配置模块参数的寄存器地址*/
	{


		memcpy(&RTU_Device.Dev_config_data.E32_config, &usRegHoldingBuf[E32_config_NUM], 6);/*拷贝参数主程序进行处理*/

		save_config(&RTU_Device.Dev_config_data);


	}

}
void modbusTransimt(uint8_t* txbuffer,uint8_t size)
{
	if(RTU_Device.Rx_Buffer.channel==(uint32_t)USART3)
	{
		if(RTU_Device.BusUsartState == UART_TX_Busy)
		{
			HAL_Delay(__HAL_DMA_GET_COUNTER(RTU_Device.BusUsartHandle->hdmatx)+1);
		}
		HAL_UART_Transmit_DMA(RTU_Device.BusUsartHandle, txbuffer, size) ;
		RTU_Device.BusUsartState = UART_TX_Busy;

	}
	else if(RTU_Device.Rx_Buffer.channel==(uint32_t)USART2)
	{
		if(RTU_Device.WirelessUsartState == UART_TX_Busy)
		{
			HAL_Delay(__HAL_DMA_GET_COUNTER(RTU_Device.WirelessUsartHandle->hdmatx)+1);
		}
		HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle, txbuffer, size );
		RTU_Device.WirelessUsartState = UART_TX_Busy;
	}

}


void meterDataDeal(void)
{
	uint8_t i,j=0;
	float tmp=0;
	for(i=0;i<26;i++)
	{
		switch (ListFuncType[i])
		{
		case VOLTAGE_TYPE :
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100);
			break;
		case CURRENT_TYPE :
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100);
			break;
		case POWER_TYPE :
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100)/65535;
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100)%65535;
			break;

		case POWERFACTOR_TYPE :
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100);
			break;
		case ENERGY_TYPE :
			usRegHoldingMeterBuf[j++]=( USHORT)(((uint32_t)io[0][i])/65535);
			usRegHoldingMeterBuf[j++]=( USHORT)(((uint32_t)io[0][i])%65535);
			break;
		case FREQUENCY  :
			usRegHoldingMeterBuf[j++]=( USHORT)(io[0][i]*100);
			break;
		}
	}
	tmp=DS18B20_GetTemp_SkipRom();
	usRegHoldingMeterBuf[ REG_INPUT_METER_NREGS-1]=( USHORT)( (tmp+100)*100);
}
/**
 * @brief   输入继电器处理
 * @detail  输入继电器10000-10005存储CHIPID 10006 路由级数
 * @param    none
 * @retval   none
 */

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex=0;
	//static  uint8_t Router_Level_flag=0;

	if( ( usAddress >= REG_INPUT_START )
			&& ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS+1 ) )
	{

		iRegIndex = ( int )( usAddress - usRegInputStart-1 );


		while( usNRegs > 0 )
		{
			*pucRegBuffer++ =
					( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
			*pucRegBuffer++ =
					( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
			iRegIndex++;
			usNRegs--;
		}


	}
	else if ((usAddress >= REG_INPUT_METER_START) &&
			(usAddress + usNRegs <= REG_INPUT_METER_START + REG_INPUT_METER_NREGS+1))
	{


		/*end*/
		iRegIndex = (int)(usAddress - usRegHoldingMeterStart - 1);

		while (usNRegs > 0)
		{
			*pucRegBuffer++ = (unsigned char)(usRegHoldingMeterBuf[iRegIndex] >> 8);
			*pucRegBuffer++ = (unsigned char)(usRegHoldingMeterBuf[iRegIndex] & 0xFF);
			iRegIndex++;
			usNRegs--;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

/**
 * @brief   保持寄存器处理
 * @detail  保持寄存器10000-10005存储CHIPID 10006 设配地址
 * @param    none
 * @retval   none
 */
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
		eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex = 0;

	if ((usAddress >= REG_HOLDING_START) &&
			(usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS+1))
	{
		/*end*/
		iRegIndex = (int)(usAddress - usRegHoldingStart - 1);
		switch (eMode)
		{
		/* Pass current register values to the protocol stack. */
		case MB_REG_READ:
			while (usNRegs > 0)
			{
				*pucRegBuffer++ = (unsigned char)(usRegHoldingBuf[iRegIndex] >> 8);
				*pucRegBuffer++ = (unsigned char)(usRegHoldingBuf[iRegIndex] & 0xFF);
				iRegIndex++;
				usNRegs--;
			}
			break;

			/* Update current register values with new values from the
			 * protocol stack. */
		case MB_REG_WRITE:
			while (usNRegs > 0)
			{
				usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
				usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
			}
			Config_Cmd_Deal(usAddress);/**/
		}
	}

	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}
/**
 * @brief  继电器处理
 * @detail  继电器8个
 * @param    none
 * @retval   none
 */

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
		eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iNCoils = ( int )usNCoils;
	unsigned short  usBitOffset;

	/* Check if we have registers mapped at this block. */
	if( ( usAddress >= REG_COILS_START ) &&
			( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE+1 ) )
	{
		usBitOffset = ( unsigned short )( usAddress - REG_COILS_START-1 );//减1
		switch ( eMode )
		{
		/* Read current values and pass to protocol stack. */
		case MB_REG_READ:
			ucRegCoilsBuf[0]=pvScanInternalDiscrete();
			while( iNCoils > 0 )
			{
				*pucRegBuffer++ =
						xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
								( unsigned char )( iNCoils >
				8 ? 8 :
						iNCoils ) );
				iNCoils -= 8;
				usBitOffset += 8;
				LED_POWER_OFF();												
				HAL_TIM_Base_Start_IT(&htim3);//开启定时器产生控制产生脉冲，让LED闪烁												
			}
			break;

			/* Update current register values. */
		case MB_REG_WRITE:
			while( iNCoils > 0 )
			{
				xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
						( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
						*pucRegBuffer++ );
				iNCoils -= 8;
				usBitOffset += 8;
			}
			SwitchDeal();//开关处理
			break;
		}

	}
	else
	{
		eStatus = MB_ENOREG;
	}
	return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
	return MB_ENOREG;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance ==TIM2){
		// prvvTIMERExpiredISR();

		RTU_Device.MeterScanFlag=TRUE;
	}
	if (htim->Instance ==TIM3){
		LED_UP_Sta();
	}
}

static void LED_UP_Sta(void)
{
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, SWITCH3_ON_Pin|SWITCH3_OFF_Pin|SWITCH1_ON_Pin, GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, SWITCH1_OFF_Pin|SWITCH2_ON_Pin |SWITCH2_OFF_Pin, GPIO_PIN_SET);
	LED_POWER_ON();
	HAL_TIM_Base_Stop(&htim3);
	__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
	__HAL_TIM_SET_COUNTER(&htim3, 0);

}	

/* Exported functions --------------------------------------------------------*/

/**
 * @}
 */

/**
 * @}
 */
