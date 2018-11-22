/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include<math.h>

#include "spi.h"
#include "spi_network.h"
#include "RN8302.h"
#include "env.h"
#include "iic.h"
#include "port.h"
#include <stdio.h>
#include "usart.h"
/* Private macros ------------------------------------------------------------*/
#define SPI_RX_BUF_SIZE     10 
#define SPI_TX_BUF_SIZE     10 

#define  SPI_CS_ENABE()     HAL_GPIO_WritePin(SCSN_M_GPIO_Port , SCSN_M_Pin, GPIO_PIN_RESET);

#define  SPI_CS_DASABE()   HAL_GPIO_WritePin(SCSN_M_GPIO_Port , SCSN_M_Pin, GPIO_PIN_SET);

#define SPI_REST_OFF     HAL_GPIO_WritePin( RN8302_RST_GPIO_Port, RN8302_RST_Pin, GPIO_PIN_SET);

#define SPI_REST_ON      HAL_GPIO_WritePin( RN8302_RST_GPIO_Port, RN8302_RST_Pin, GPIO_PIN_RESET); 



#define  GETWRITECMDADDR(wReg)    ((((uint8_t)(wReg >> 4)) & 0xf0) + 0x80)
#define  GETREADCMDADDR(wReg)			(((uint8_t)(wReg >> 4)) & 0xf0)								 
#define  GETREGADDR(wReg)			    (wReg & 0x00ff)	


#define  D_ZXOT              (((int32_t )(Standard_Current*0.05f))>>12)
#define  D_LostVoltage       (((int32_t )(Standard_Voltage*0.5f))>>12)

#define  VOLTAGE_TYPE          1
#define  CURRENT_TYPE          2
#define  POWER_TYPE            3
#define  POWERFACTOR_TYPE      4
#define  ENERGY_TYPE           5
#define	 FREQUENCY             6

#define 	ERROR_UPPER_LIMIT	   (0.2	)
#define 	ERROR_LOWER_LIMIT	   (-0.2)			

#define   VOLTAGE_ERROR_LIMIT				(Standard_Voltage>>10)			// 千分之一的误差 					 
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
char IOstring[][20] = {
		"VOLTAGE_TYPE A" ,   /*A phase voltage effective value-1*/
		"VOLTAGE_TYPE B" ,    /*B phase voltage effective value-1*/
		"VOLTAGE_TYPE C" ,    /*C phase voltage effective value-1*/


		"CURRENT_TYPE A" ,   /*A phase current effective value-1*/
		"CURRENT_TYPE B",    /*B phase current effective value-1*/
		"CURRENT_TYPE C",    /*C phase current effective value-1*/


		"POWER_TYPE A",   /*A phase active power-1*/
		"POWER_TYPE B",   /*B phase active power-1*/
		"POWER_TYPE C",   /*C phase active power-1*/
		"POWER_TYPE ABC",   /*ABC ALL phase active power-1*/


		"POWER_TYPE A" ,   /*A phase reactive power-1*/
		"POWER_TYPE B",    /*B phase reactive power-1*/
		"POWER_TYPE C",   /*C phase reactive power-1*/
		"POWER_TYPE ABC",   /*ABC phase reactive power-1*/

		"POWER_TYPE A",   /*A phase Apparent power-1*/
		"POWER_TYPE B",   /*B phase Apparent power-1*/
		"POWER_TYPE C",  /*C phase  Apparent power-1 */
		"POWER_TYPE ABC",  /*ABC phase  Apparent  power-1*/

		"POWERFACTOR_TYPE A" ,   /*A phase power factor-1*/
		"POWERFACTOR_TYPE B" ,   /*B phase power factor-1*/
		"POWERFACTOR_TYPE C",   /*C phase power factor-1*/

		"ENERGY_TYPE A",
		"ENERGY_TYPE B",
		"ENERGY_TYPE C",
		"ENERGY_TYPE ABC",

		"FREQUENCY",           /*frequency*/
};
/* Private variables ---------------------------------------------------------*/
static uint8_t rxbuf[SPI_RX_BUF_SIZE];
static uint8_t txbuf[SPI_TX_BUF_SIZE];

static uint16_t ListRegAddr[RN8302_READ_POINT_COUNT] = {
		0x007 ,   /*A phase voltage effective value-1*/
		0x008,    /*B phase voltage effective value-1*/
		0x009,    /*C phase voltage effective value-1*/


		0x00B ,   /*A phase current effective value-1*/
		0x00C,    /*B phase current effective value-1*/
		0x00D,    /*C phase current effective value-1*/


		0x0014,   /*A phase active power-1*/
		0x0015,   /*B phase active power-1*/
		0x0016,   /*C phase active power-1*/
		0x0017,   /*ABC ALL phase active power-1*/


		0x0018 ,   /*A phase reactive power-1*/
		0x0019,    /*B phase reactive power-1*/
		0x001A ,   /*C phase reactive power-1*/
		0x001B ,   /*ABC phase reactive power-1*/

		0x001C,   /*A phase Apparent power-1*/
		0x001D,   /*B phase Apparent power-1*/
		0x001E,  /*C phase  Apparent power-1 */
		0x001F ,  /*ABC phase  Apparent  power-1*/

		0x0020,   /*A phase power factor-1*/
		0x0021,   /*B phase power factor-1*/
		0x0022,   /*C phase power factor-1*/

		0x0030,
		0x0031,
		0x0032,
		0x0033,

		0x0057    /*frequency*/

};
static uint8_t ListFuncType[RN8302_READ_POINT_COUNT] = {
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
static uint8_t TimeFlag = 0;
static uint8_t ListRegLen[RN8302_READ_POINT_COUNT] = {
		4 ,   /*A phase voltage effective value-1*/
		4,    /*B phase voltage effective value-1*/
		4,    /*C phase voltage effective value-1*/


		4 ,   /*A phase current effective value-1*/
		4,    /*B phase current effective value-1*/
		4,    /*C phase current effective value-1*/


		4,   /*A phase active power-1*/
		4,   /*B phase active power-1*/
		4,   /*C phase active power-1*/
		4,   /*ABC ALL phase active power-1*/


		4 ,   /*A phase reactive power-1*/
		4,    /*B phase reactive power-1*/
		4 ,   /*C phase reactive power-1*/
		4 ,   /*ABC phase reactive power-1*/

		4,   /*A phase Apparent power-1*/
		4,   /*B phase Apparent power-1*/
		4,  /*C phase  Apparent power-1 */
		4 ,  /*ABC phase  Apparent  power-1*/

		3,   /*A phase power factor-1*/
		3,   /*B phase power factor-1*/
		3,   /*C phase power factor-1*/

		3,   /*A phase Apparent power-1*/
		3,   /*B phase Apparent power-1*/
		3,  /*C phase  Apparent power-1 */
		3 ,  /*ABC phase  Apparent  power-1*/

		3,   /*frequency*/

};
static uint8_t RmsConstList[RN8302_READ_POINT_COUNT] = {
		0 ,   /*A phase voltage effective value-1*/
		1,    /*B phase voltage effective value-1*/
		2,    /*C phase voltage effective value-1*/


		0 ,   /*A phase current effective value-1*/
		1,    /*B phase current effective value-1*/
		2,    /*C phase current effective value-1*/


		0,   /*A phase active power-1*/
		1,   /*B phase active power-1*/
		2,   /*C phase active power-1*/
		0,   /*ABC ALL phase active power-1*/


		0 ,   /*A phase reactive power-1*/
		1,    /*B phase reactive power-1*/
		2 ,   /*C phase reactive power-1*/
		0 ,   /*ABC phase reactive power-1*/

		0,   /*A phase Apparent power-1*/
		1,   /*B phase Apparent power-1*/
		2,  /*C phase  Apparent power-1 */
		0 ,  /*ABC phase  Apparent  power-1*/

		3,   /*A phase power factor-1*/
		3,   /*B phase power factor-1*/
		3,   /*C phase power factor-1*/

		0,   /*A phase Energy power-1*/
		1,   /*B phase Apparent power-1*/
		2,  /*C phase  Apparent power-1 */
		0 ,  /*ABC phase  Apparent  power-1*/

		3,   /*frequency*/

};
static uint8_t EnergyAddr_FRAM[4] ={
		10,
		20,
		30,
		40,

};
/* Global variables ----------------------------------------------------------*/

float io[2][RN8302_READ_POINT_COUNT] = { 0 };

/* Private function prototypes -----------------------------------------------*/
void RN8302SpiScanData(void);

static ErrorStatus RN8302_WriteReg(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t *pBuf, uint8_t DatLen);
static ErrorStatus RN8302_WriteDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);
static ErrorStatus RN8302_ReadReg(SPI_HandleTypeDef *hspi, uint16_t wReg, int32_t* val, uint8_t DatLen);
static ErrorStatus RN8302_ReadDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);
static void  VoltageDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
static void  CurrentDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
static void  PowerDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
static void  FactolDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
static void  FrequencyDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
static void  EnergyDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum);
//static uint8_t RN8302_ReadReg(uint8_t spiNo,uint16_t address,uint8_t len,int32_t* val);
//static uint8_t RN8302_WriteReg();


/* Private functions ---------------------------------------------------------*/


void RN8302SpiScanData(void)
{
	uint32_t i;
	int32_t val1 = 0, val2 = 0;
	ErrorStatus Sta =  ERROR;
	for (i = 0; i < RN8302_READ_POINT_COUNT; i++)
	{
		val1 = 0; val2 = 0;

		//	if (RN8302_ReadReg(&hspi1, ListRegAddr[i], &val2, ListRegLen[i]) == SUCCESS)//pt 2018 2 26(val1->val2)
		//HAL_GPIO_WritePin(GPIOB, SPI1_STATE_LED_Pin, GPIO_PIN_RESET);

		//HAL_GPIO_WritePin(GPIOB, SPI1_STATE_LED_Pin, GPIO_PIN_SET);
		Sta=RN8302_ReadReg(&hspi1, ListRegAddr[i], &val1, ListRegLen[i]) ;
		if(Sta == ERROR)
		{
			sprintf(user_usartTXBuffer,"SPI read error!");
			HAL_UART_Transmit_DMA(&huart1,(uint8_t *)user_usartTXBuffer,strlen(user_usartTXBuffer));
		}
        switch (ListFuncType[i])
			{
			case VOLTAGE_TYPE:
				VoltageDealFunc(i, val1, 0);
				//VoltageDealFunc(i, val2, 1);
				break;
			case CURRENT_TYPE:
				CurrentDealFunc(i, val1, 0);
				//CurrentDealFunc(i, val2, 1);
				break;
			case POWER_TYPE:
				PowerDealFunc(i, val1, 0);
				//PowerDealFunc(i, val2,1);
				break;
			case POWERFACTOR_TYPE:
				FactolDealFunc(i, val1, 0);
				//FactolDealFunc(i, val2, 1);
				break;
			case FREQUENCY:
				FrequencyDealFunc(i, val1, 0);
				//FrequencyDealFunc(i, val2, 1);
				break;
			case ENERGY_TYPE:
				// ENTER_CRITICAL_SECTION( );
				EnergyDealFunc(i, val1, 0);
				//EXIT_CRITICAL_SECTION( );
				// EnergyDealFunc(i, val2, 1);


			default:
				break;
			}

	}
	TimeFlag = 1;
	meterDataDeal();
}
static void  VoltageDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;
	float Kvm = VIN_STA * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VRmsConst[RmsConstList[Addr]] / (float)Standard_Voltage;
	//uint8_t CanAddr;
	Dat = ReadValue * Kvm;
	if (fabsf(Dat - io[ChipNum][Addr]) > RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VoltageThreshould &&TimeFlag)
	{
		io[ChipNum][Addr] = Dat;
		//CanAddr = Addr + (RN8302_READ_POINT_COUNT*ChipNum) + 1;
		//prvvDataTxFrame(CanAddr, CAN_FRAME_COMMAND_EVENT, io[ChipNum][Addr]);
	}
	io[ChipNum][Addr] = Dat;
	if (io[ChipNum][Addr] < RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.VoltageDeadZone[RmsConstList[Addr]])
		io[ChipNum][Addr] = 0;

}
static void  CurrentDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;
	float Kcm = CURRENT_STA * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.IRmsConst[RmsConstList[Addr]] / (float)Standard_Current;
	//uint8_t CanAddr;
	Dat = ReadValue * Kcm;
	if (fabsf(Dat - io[ChipNum][Addr]) > RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.CurrentThreshould&&TimeFlag)
	{
		io[ChipNum][Addr] = Dat;
		//CanAddr = Addr + (RN8302_READ_POINT_COUNT*ChipNum) + 1;
		//prvvDataTxFrame(CanAddr, CAN_FRAME_COMMAND_EVENT, io[ChipNum][Addr]);
	}
	io[ChipNum][Addr] = Dat;
	if (io[ChipNum][Addr] < RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.CurrentDeadZone[RmsConstList[Addr]])
		io[ChipNum][Addr] = 0;

}
static void  PowerDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;
	//uint8_t CanAddr;
	float Kpm = (VIN_STA / (float)Standard_Voltage)*(CURRENT_STA / (float)Standard_Current) * 8388608 * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[RmsConstList[Addr]];
	//2`27*2`27/2`31=8388608
	if (ReadValue & 0x80000000)
		ReadValue = ~ReadValue + 1;
	Dat = ReadValue * Kpm;
	if (fabsf(Dat - io[ChipNum][Addr]) > RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PowerThreshould&&TimeFlag)
	{
		io[ChipNum][Addr] = Dat;
		//CanAddr = Addr + (RN8302_READ_POINT_COUNT*ChipNum) + 1;
		//prvvDataTxFrame(CanAddr, CAN_FRAME_COMMAND_EVENT, io[ChipNum][Addr]);
	}
	io[ChipNum][Addr] = Dat;
	if (io[ChipNum][Addr] <RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PowerDeadZone[RmsConstList[Addr]])
		io[ChipNum][Addr] = 0;
}
static void  FactolDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;

	Dat = (float)ReadValue / (float)8388608;
	io[ChipNum][Addr] = Dat;

}
static void   EnergyDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;
	float lastDat=0;
	//uint8_t CanAddr;
	//io[ChipNum][Addr]=0x00000000;
	//iic_FRAM_BufferWrite((uint8_t*)&(io[ChipNum][Addr]),EnergyAddr_FRAM[RmsConstList[Addr]],4);
	iic_FRAM_BufferRead((uint8_t*)&lastDat, EnergyAddr_FRAM[Addr-21], 4);
	//float Kpm = (VIN_STA / (float)Standard_Voltage)*(CURRENT_STA / (float)Standard_Current) * 8388608 * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[RmsConstList[Addr]];
	//volatile  float Kpm = (VIN_STA / ((0.8*(float)Standard_Voltage)/0x8000000))*(CURRENT_STA / ((0.8*(float)Standard_Current)/0x8000000))  * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[RmsConstList[Addr]];
	//volatile  float Kpm = (VIN_STA / (float)Standard_Voltage)*(CURRENT_STA / (float)Standard_Current)  * RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[RmsConstList[Addr]];
	volatile  float Kpm =RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.PRmsConst[RmsConstList[Addr]];

	Dat = ReadValue * Kpm;

	if (fabsf(Dat - io[ChipNum][Addr]) > 1)
	{

		io[ChipNum][Addr] = Dat+lastDat;
		//CanAddr = Addr + (RN8302_READ_POINT_COUNT*ChipNum) + 1;
		//prvvDataTxFrame(CanAddr, CAN_FRAME_COMMAND_EVENT, io[ChipNum][Addr]);
		//iic_FRAM_BufferWrite((uint8_t*)&(io[ChipNum][Addr]),EnergyAddr_FRAM[Addr-21],4);
	}
	io[ChipNum][Addr] = Dat+lastDat;
	iic_FRAM_BufferWrite((uint8_t*)&(io[ChipNum][Addr]),EnergyAddr_FRAM[Addr-21],4);
	if (io[ChipNum][Addr] <1)
		io[ChipNum][Addr] = 0;
}
static void  FrequencyDealFunc(uint8_t Addr, uint32_t ReadValue, uint8_t ChipNum)
{
	float Dat;
	uint8_t CanAddr;
	if((ReadValue>163840)||(ReadValue<109226))  //when 50HZ is 5120 ;+- 20% frequence is 6144 and 4096
		ReadValue=1310721;
	Dat = 65536000.0f / ReadValue;
	if (fabsf(Dat - io[ChipNum][Addr]) > RTU_Device.Dev_config_data.MeterConfig.RN8302FirmPara.FrequencyThreshould&&TimeFlag)
	{
		io[ChipNum][Addr] = Dat;
		CanAddr = Addr + (RN8302_READ_POINT_COUNT*ChipNum) + 1;
		//prvvDataTxFrame(CanAddr, CAN_FRAME_COMMAND_EVENT, io[ChipNum][Addr]);
	}
	if(Dat>1)
	Dat=1;
	io[ChipNum][Addr] = Dat;


}
static ErrorStatus RN8302_WriteReg(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t *pBuf, uint8_t DatLen)
{
	uint8_t i, chksum = 0, Repeat;
	uint8_t *DatP = NULL;
	ErrorStatus	err = SUCCESS;
	if ((DatLen == 0) || (DatLen > 4)) return(ERROR);

	for (Repeat = 3; Repeat != 0; Repeat--)
	{
		DatP = txbuf;
		SPI_CS_ENABE()
		DatP[0] = (uint8_t)GETREGADDR(wReg);
		DatP[1] = GETWRITECMDADDR(wReg);
		for (i = 0; i < DatLen; i++)/*高字节在前，低字节在后*/
		{
			DatP[2 + i] = pBuf[DatLen - 1 - i];
		}
		for (i = 0; i < DatLen + 2; i++)/*求校验*/
		{
			chksum += DatP[i];
		}
		DatP[DatLen + 2] = ~chksum;
		HAL_SPI_Transmit(hspi, DatP, DatLen + 1 + 2, 0XFFF);
		SPI_CS_DASABE()

		/*读回数据检查*/
		err = RN8302_WriteDataCheck(hspi, 0X018D, pBuf, DatLen);//上次写入SPI数据保存在0x018D地址
		if (err == SUCCESS)
		{
			break;
		}

	}

	return(err);
}

static ErrorStatus RN8302_WriteDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen)
{
	uint8_t i, chksum = 0;
	uint8_t *DatP = NULL;
	uint8_t ReadLast[6] = { 0 };
	ErrorStatus	err = SUCCESS;
	SPI_CS_ENABE()
	ReadLast[0] = (uint8_t)GETREGADDR(wReg);
	ReadLast[1] = GETREADCMDADDR(wReg);
	HAL_SPI_Transmit(hspi, ReadLast, 2, 0XFF);// 

	HAL_SPI_Receive(hspi, &ReadLast[2], 4, 0XFF);
	DatP = &ReadLast[2];
	for (i = 3; i > 0; i--)
	{
		if (DatLen >= i)
		{
			if (*DatP != pBuf[i - 1]) //对比读出和写入数据
			{
				err = ERROR;
				break;
			}
		}
		DatP++;
	}
	for (i = 0; i < 5; i++)
	{
		chksum += ReadLast[i];
	}
	chksum = ~chksum;
	if (err == SUCCESS)
	{
		if (ReadLast[5] != chksum)  err = ERROR; //校验读出数据是否正确
	}
	SPI_CS_DASABE()
	return(err);
}
static ErrorStatus RN8302_ReadReg(SPI_HandleTypeDef *hspi, uint16_t wReg, int32_t* val, uint8_t DatLen)
{
	uint8_t i, chksum = 0, Repeat;
	uint8_t *DatP = NULL, *pBuf = (uint8_t*)val;
	ErrorStatus	err = SUCCESS;
	if (DatLen == 0) return(ERROR);

	for (Repeat = 3; Repeat != 0; Repeat--)
	{
		DatP = rxbuf;
		SPI_CS_ENABE()
		DatP[0] = (uint8_t)GETREGADDR(wReg);
		DatP[1] = GETREADCMDADDR(wReg);

		HAL_SPI_Transmit(hspi, DatP, 2, 0XFF);// 

		HAL_SPI_Receive(hspi, &DatP[2], DatLen + 1, 0XFF);

		for (i = 0; i < DatLen; i++)
		{
			pBuf[DatLen - 1 - i] = DatP[2 + i];
		}
		for (i = 0; i < DatLen + 2; i++)
		{
			chksum += DatP[i];
		}
		chksum = ~chksum;
		if (DatP[2 + DatLen] != chksum)  err = ERROR;
		if (err != SUCCESS) continue;
		SPI_CS_DASABE()
		/*读回数据检查*/
		err = RN8302_ReadDataCheck(hspi, 0X018C, pBuf, DatLen);//上次读入SPI数据保存在0x018C地址
		if (err == SUCCESS)
		{
			break;
		}

	}
	SPI_CS_DASABE()
	return(err);
}

static ErrorStatus RN8302_ReadDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen)
{
	uint8_t i, chksum = 0;
	uint8_t *DatP = NULL;
	uint8_t ReadLast[7] = { 0 };
	ErrorStatus	err = SUCCESS;

	SPI_CS_ENABE()
	ReadLast[0] = (uint8_t)GETREGADDR(wReg);
	ReadLast[1] = GETREADCMDADDR(wReg);
	HAL_SPI_Transmit(hspi, ReadLast, 2, 0XFF);// 

	HAL_SPI_Receive(hspi, &ReadLast[2], 5, 0XFF);

	DatP = &ReadLast[2];
	for (i = 4; i > 0; i--)
	{
		if (DatLen >= i)
		{
			if (*DatP != pBuf[i - 1]) //对比读出和写入数据
			{
				err = ERROR;
				break;
			}

		}
		DatP++;
	}
	for (i = 0; i < 6; i++)
	{
		chksum += ReadLast[i];
	}
	chksum = ~chksum;
	if (err == SUCCESS)
	{

		if (ReadLast[6] != chksum)  err = ERROR; //校验读出数据是否正确
	}
	SPI_CS_DASABE()

	return(err);
}


void RN8302_Init(SPI_HandleTypeDef *hspi, sRN8302FirmParaFile_TypeDef RN8302FirmPara)
{

	sRN8302StruDataComm_TypeDef RN8302DataComm;
	RN8302DataComm.ucTemp8 = 0xe5; // 写使能位
	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xA2; // 切换到EMM模式,计量模式
	RN8302_WriteReg(hspi, WMSW, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xfa; //软件复位
	RN8302_WriteReg(hspi, 0x0182, RN8302DataComm.ucTempBuf, 1);
	HAL_Delay(20);
	RN8302DataComm.ucTemp8 = 0xe5; //// 写使能位
	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xA2; // 切换到EMM模式,计量模式
	RN8302_WriteReg(hspi, WMSW, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTempBuf[0] = 0x77; // // 计量控制位对各相计量使能
	RN8302DataComm.ucTempBuf[1] = 0x77;
	RN8302DataComm.ucTempBuf[2] = 0x77;
	RN8302_WriteReg(hspi, 0x0162, RN8302DataComm.ucTempBuf, 3);
	//RN8302DataComm.wTemp16 = 0xd8d6;//设置电表常数 HFConst ＝ INT[P*3.6*10 6 *fosc / (32*EC*Un*Ib*2 31 )]邋EC
	RN8302DataComm.wTemp16 = RN8302FirmPara.HFConst1;//设置电表常数
	RN8302_WriteReg(hspi, HFCONST1, RN8302DataComm.ucTempBuf, 2);

	RN8302DataComm.wTemp16 = 0xd8d6;//设置电表常数 HFConst ＝ INT[P*3.6*10 6 *fosc / (32*EC*Un*Ib*2 31 )]邋EC
	//RN8302DataComm.wTemp16 = RN8302FirmPara.HFConst1;//设置电表常数
	RN8302_WriteReg(hspi, HFCONST2, RN8302DataComm.ucTempBuf, 2);
	// 电压增益初始化
	RN8302_WriteReg(hspi, GSUA, (uint8_t *)&RN8302FirmPara.VGain[0], 2);
	RN8302_WriteReg(hspi, GSUB, (uint8_t *)&RN8302FirmPara.VGain[1], 2);
	RN8302_WriteReg(hspi, GSUC, (uint8_t *)&RN8302FirmPara.VGain[2], 2);

	// 电流增益初始化
	RN8302_WriteReg(hspi, GSIA, (uint8_t *)&RN8302FirmPara.IGain[0], 2);
	RN8302_WriteReg(hspi, GSIB, (uint8_t *)&RN8302FirmPara.IGain[1], 2);
	RN8302_WriteReg(hspi, GSIC, (uint8_t *)&RN8302FirmPara.IGain[2], 2);
	RN8302_WriteReg(hspi, GSIN, (uint8_t *)&RN8302FirmPara.IGain[3], 2);

	//	// 通道相位分段参数设置
	//	RN8302_WriteReg(hspi,PRTH1L,(uint8_t *)&RN8302FirmPara.PRth[0],2);
	//	RN8302_WriteReg(hspi,PRTH1H,(uint8_t *)&RN8302FirmPara.PRth[1],2);
	//	RN8302_WriteReg(hspi,PRTH2L,(uint8_t *)&RN8302FirmPara.PRth[2],2);
	//	RN8302_WriteReg(hspi,PRTH2H,(uint8_t *)&RN8302FirmPara.PRth[3],2);	 


	//	// 通道相位校正
	//	RN8302_WriteReg(hspi,PHSIA,(uint8_t *)&RN8302FirmPara.PHSI[0],3);
	//	RN8302_WriteReg(hspi,PHSIB,(uint8_t *)&RN8302FirmPara.PHSI[1],3);
	//	RN8302_WriteReg(hspi,PHSIC,(uint8_t *)&RN8302FirmPara.PHSI[2],3);
	//	//// 通道相位校正,有功分段相位校正寄存器
	//	RN8302_WriteReg(hspi,PA_PHS,(uint8_t *)&RN8302FirmPara.P_PHS[0],2);
	//	RN8302_WriteReg(hspi,PB_PHS,(uint8_t *)&RN8302FirmPara.P_PHS[1],2);
	//	RN8302_WriteReg(hspi,PC_PHS,(uint8_t *)&RN8302FirmPara.P_PHS[2],2);
	//	//有功功率增益
	//	RN8302_WriteReg(hspi,GPA,(uint8_t *)&RN8302FirmPara.PGain[0],2);
	//	RN8302_WriteReg(hspi,GPB,(uint8_t *)&RN8302FirmPara.PGain[1],2);
	//	RN8302_WriteReg(hspi,GPC,(uint8_t *)&RN8302FirmPara.PGain[2],2);
	//	//无功功率增益
	//	RN8302_WriteReg(hspi,GQA,(uint8_t *)&RN8302FirmPara.PGain[0],2);
	//	RN8302_WriteReg(hspi,GQB,(uint8_t *)&RN8302FirmPara.PGain[1],2);
	//	RN8302_WriteReg(hspi,GQC,(uint8_t *)&RN8302FirmPara.PGain[2],2);
	//	//实在功率增益
	//	RN8302_WriteReg(hspi,GSA,(uint8_t *)&RN8302FirmPara.PGain[0],2);
	//	RN8302_WriteReg(hspi,GSB,(uint8_t *)&RN8302FirmPara.PGain[1],2);
	//	RN8302_WriteReg(hspi,SC,(uint8_t *)&RN8302FirmPara.PGain[2],2);

	// 通道功率OFFSET校正
	//	RN8302_WriteReg(hspi,PA_OS,(uint8_t *)&RN8302FirmPara.UI_Offset[0],2);
	//	RN8302_WriteReg(hspi,PB_OS,(uint8_t *)&RN8302FirmPara.UI_Offset[1],2);
	//	RN8302_WriteReg(hspi,PC_OS,(uint8_t *)&RN8302FirmPara.UI_Offset[2],2);
	// 启动电流阈值寄存器
	//	RN8302_WriteReg(IStart_PS,(uint8_t *)&RN8302FirmPara.NOLOAD,2);
	///过零阈值寄存
	RN8302DataComm.wTemp16 = D_ZXOT;
	RN8302_WriteReg(hspi, ZXOT, RN8302DataComm.ucTempBuf, 2);

	//失压阀值寄存器
	RN8302DataComm.wTemp16 = D_LostVoltage;
	RN8302_WriteReg(hspi, LostVoltT, RN8302DataComm.ucTempBuf, 2);

	//	RN8302DataComm.ucTemp8 = 0x42; 
	//	RN8302_WriteReg(0x0184,RN8302DataComm.ucTempBuf,1);
	//	RN8302DataComm.ucTemp8 = 0;   
	//	RN8302_WriteReg(0x0185,RN8302DataComm.ucTempBuf,1);
	//	RN8302DataComm.ucTemp8 = 0; 
	//	RN8302_WriteReg(0x0184,RN8302DataComm.ucTempBuf,1);
	//配置CF脉冲输出
	RN8302DataComm.ucTempBuf[0] = 0x10;
	RN8302DataComm.ucTempBuf[1] = 0x32;
	RN8302DataComm.ucTempBuf[2] = 0x07;
	RN8302_WriteReg(hspi, CFCFG, RN8302DataComm.ucTempBuf, 3);
	//配置 计量控制位 默认值 400000  计量单元配置寄存器 EMUCFG 用于配置计量模块
	RN8302DataComm.ucTempBuf[0] = 0x40;
	RN8302DataComm.ucTempBuf[1] = 0x00;
	RN8302DataComm.ucTempBuf[2] = 0x00;
	RN8302_WriteReg(hspi, 0x0161, RN8302DataComm.ucTempBuf, 3);


	RN8302DataComm.ucTemp8 = 0x10; //  // 清空采样数据缓存区
	RN8302_WriteReg(hspi, 0x0163, RN8302DataComm.ucTempBuf, 1);
	//RN8302DataComm.wTemp16=HFconst1；
	//	Read_RN8302_Data();
	RN8302DataComm.ucTemp8 = 0xDC; // 写保护
	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);
	HAL_Delay(250);
}
/* Exported functions --------------------------------------------------------*/


//void ParameterVerification(SPI_HandleTypeDef *hspi, sRN8302FirmParaFile_TypeDef *RN8302FirmPara)
//{
//	uint8_t i, j, Clear;
//	int32_t Read_data = 0;
//	float   ADErr = 0;
//	HAL_IWDG_Refresh(&hiwdg);
//
//	sRN8302StruDataComm_TypeDef RN8302DataComm;
//	RN8302DataComm.ucTemp8 = 0xe5; // 写使能位
//	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);
//	RN8302FirmPara->HFConst1 = 1598;
//	RN8302_WriteReg(hspi, HFCONST1, (uint8_t*)&(RN8302FirmPara->HFConst1), 2);//写入HFconst1
//	Read_data = 0;
//	// 电压增益初始化
//	RN8302_WriteReg(hspi, GSUA, (uint8_t*)&Read_data, 2);
//	RN8302_WriteReg(hspi, GSUB, (uint8_t*)&Read_data, 2);
//	RN8302_WriteReg(hspi, GSUC, (uint8_t*)&Read_data, 2);
//
//	// 电流增益初始化
//	RN8302_WriteReg(hspi, GSIA, (uint8_t*)&Read_data, 2);
//	RN8302_WriteReg(hspi, GSIB, (uint8_t*)&Read_data, 2);
//	RN8302_WriteReg(hspi, GSIC, (uint8_t*)&Read_data, 2);
//	RN8302_WriteReg(hspi, GSIN, (uint8_t*)&Read_data, 2);
//	osDelay(250);
//	/*电压校正*/
//	for (i = 0; i < 3; i++)  //
//	{
//		for (j = 0; j < 5; j++)
//		{
//			RN8302_ReadReg(hspi, 0x0007 + i, &Read_data, 4);
//			ADErr += ((float)Read_data - (float)Standard_Voltage) / Standard_Voltage;
//			//if(ADErr<ERROR_LOWER_LIMIT)
//			//	break;
//			osDelay(250);
//		}
//		ADErr = ADErr / 5.0;
//		if (ADErr<ERROR_UPPER_LIMIT&&ADErr>ERROR_LOWER_LIMIT)
//		{
//			ADErr = (-ADErr / (1 + ADErr));
//			if (ADErr > 0)
//				RN8302FirmPara->VGain[i] = (uint16_t)(ADErr * 32768);
//			else
//				RN8302FirmPara->VGain[i] = (uint16_t)(65535 + ADErr * 32768);
//			RN8302_WriteReg(hspi, GSUA + i, (uint8_t *)&(RN8302FirmPara->VGain[i]), 2);
//			RN8302_ReadReg(hspi, GSUA + i, &Read_data, 2);
//		}
//		ADErr = 0;
//	}
//
//	/*电流校正*/
//	for (i = 0; i < 3; i++)  //
//	{
//		for (j = 0; j < 5; j++)
//		{
//			RN8302_ReadReg(hspi, 0x000B + i, &Read_data, 4);
//			ADErr += ((float)Read_data - (float)Standard_Current) / Standard_Current;
//			//if(ADErr<ERROR_LOWER_LIMIT)
//				//break;
//			osDelay(250);
//		}
//		ADErr = ADErr / 5.0;
//		if (ADErr<ERROR_UPPER_LIMIT&&ADErr>ERROR_LOWER_LIMIT)
//		{
//			ADErr = (-ADErr / (1 + ADErr));
//			if (ADErr > 0)
//				RN8302FirmPara->IGain[i] = (uint16_t)(ADErr * 32768);
//			else
//				RN8302FirmPara->IGain[i] = (uint16_t)(65535 + ADErr * 32768);
//
//			RN8302_WriteReg(hspi, GSIA + i, (uint8_t *)&(RN8302FirmPara->IGain[i]), 2);
//			RN8302_ReadReg(hspi, GSIA + i, &Read_data, 2);
//
//		}
//		ADErr = 0;
//	}
//	RN8302DataComm.ucTemp8 = 0xDC; // 写保护
//	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);
//
//
//}
void ParameterVerification(SPI_HandleTypeDef *hspi, sRN8302FirmParaFile_TypeDef *RN8302FirmPara)
{
	uint8_t i, j;
	int32_t Read_data = 0;
	float   ADErr[3] = { 0, 0,0};
	//HAL_IWDG_Refresh(&hiwdg);

	sRN8302StruDataComm_TypeDef RN8302DataComm;
	RN8302DataComm.ucTemp8 = 0xe5; // 写使能位
	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);
	RN8302FirmPara->HFConst1 = 1598;
	RN8302_WriteReg(hspi, HFCONST1, (uint8_t*)&(RN8302FirmPara->HFConst1), 2);//写入HFconst1
	Read_data = 0;
	// 电压增益初始化
	RN8302_WriteReg(hspi, GSUA, (uint8_t*)&Read_data, 2);
	RN8302_WriteReg(hspi, GSUB, (uint8_t*)&Read_data, 2);
	RN8302_WriteReg(hspi, GSUC, (uint8_t*)&Read_data, 2);

	// 电流增益初始化
	RN8302_WriteReg(hspi, GSIA, (uint8_t*)&Read_data, 2);
	RN8302_WriteReg(hspi, GSIB, (uint8_t*)&Read_data, 2);
	RN8302_WriteReg(hspi, GSIC, (uint8_t*)&Read_data, 2);
	RN8302_WriteReg(hspi, GSIN, (uint8_t*)&Read_data, 2);
	HAL_Delay(1000);
	/*电压校正*/

	for (j = 0; j < 5; j++)
	{
		for (i = 0; i < 3; i++)  //for (i = 0; i < 3; i++)  //
		{
			RN8302_ReadReg(hspi, 0x0007 + i, &Read_data, 4);
			ADErr[i] += ((float)Read_data - (float)Standard_Voltage) / Standard_Voltage;
		}
		HAL_Delay(250);
	}
	for (i = 0; i < 3; i++)  //
	{
		ADErr[i] = ADErr[i] / 5.0;
		if (ADErr[i]<ERROR_UPPER_LIMIT&&ADErr[i]>ERROR_LOWER_LIMIT)
		{
			ADErr[i] = (-ADErr[i] / (1 + ADErr[i]));
			if (ADErr[i] > 0)
				RN8302FirmPara->VGain[i] = (uint16_t)(ADErr[i] * 32768);
			else
				RN8302FirmPara->VGain[i] = (uint16_t)(65535 + ADErr[i] * 32768);
			RN8302_WriteReg(hspi, GSUA + i, (uint8_t *)&(RN8302FirmPara->VGain[i]), 2);
			RN8302_ReadReg(hspi, GSUA + i, &Read_data, 2);
		}
		ADErr[i] = 0;
	}

	/*电流校正*/
	for (j = 0; j < 5; j++)
	{
		for (i = 0; i < 3; i++)  //
		{
			RN8302_ReadReg(hspi, 0x000B + i, &Read_data, 4);
			ADErr[i] += ((float)Read_data - (float)Standard_Current) / Standard_Current;
		}
		//if(ADErr<ERROR_LOWER_LIMIT)
		//break;
		HAL_Delay(250);
	}
	for (i = 0; i < 3; i++)  //
	{

		ADErr[i] = ADErr[i] / 5.0;
		if (ADErr[i]<ERROR_UPPER_LIMIT&&ADErr[i]>ERROR_LOWER_LIMIT)
		{
			ADErr[i] = (-ADErr[i] / (1 + ADErr[i]));
			if (ADErr[i]> 0)
				RN8302FirmPara->IGain[i] = (uint16_t)(ADErr [i]* 32768);
			else
				RN8302FirmPara->IGain[i] = (uint16_t)(65535 + ADErr [i]* 32768);

			RN8302_WriteReg(hspi, GSIA + i, (uint8_t *)&(RN8302FirmPara->IGain[i]), 2);
			RN8302_ReadReg(hspi, GSIA + i, &Read_data, 2);

		}
		ADErr [i]= 0;
	}
	RN8302DataComm.ucTemp8 = 0xDC; // 写保护
	RN8302_WriteReg(hspi, 0x0180, RN8302DataComm.ucTempBuf, 1);


}

