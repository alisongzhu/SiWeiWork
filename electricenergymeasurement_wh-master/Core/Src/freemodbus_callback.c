/*
 * freemodbus_callback.c
 *
 *  Created on: 2019年1月23日
 *      Author: pt
 */


 
/* Includes ------------------------------------------------------------------*/
#include "modbus_regs.h"
#include "rn8302b.h"
#include "mb.h"
#include "FreeRTOS.h"
#include "bsp_nvs_config.h"
#include "serial.h"
#include "nvs/nvs.h"
#include "string.h"
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
static USHORT usRegInputStart = REG_INPUT_START;

static USHORT usRegHoldingStart = REG_HOLDING_START;
static USHORT usRegHoldingBuf[REG_HOLDING_NREGS];

/* Private variables ---------------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
 USHORT usRegInputBuf[REG_INPUT_NREGS];
/* Private function prototypes -----------------------------------------------*/
static void save_para(void*Para ,uint8_t size, uint8_t NvsID);
static void* get_para(void*Para ,uint8_t size, uint8_t NvsID);
static void WriteDevPara(UCHAR * pucRegBuffer,USHORT usAddress,USHORT usNRegs);
static void ReadDevPara(UCHAR * pucRegBuffer,USHORT usAddress,USHORT usNRegs);
/* Private functions ---------------------------------------------------------*/
 static void* get_para(void*Para ,uint8_t size, uint8_t NvsID)
 {

	 int rc = 0;
	 if(Para == NULL)
	 {
		Para = pvPortMalloc(size);
		if(Para != NULL)
		{
			rc = nvs_read(&fs, NvsID, Para, size);
			if(rc < 0)
			{
				switch (NvsID)
				{
				case RS485_PARA_ID:
					memcpy(Para,&defultRs485_para,size);
					break;
				case RN8302B_RATIO_PARA_ID:
					memcpy(Para,&default_RN8302Ratio,size);
					break;
				case RN8302B_DEADZONE_PARA_ID:
					memcpy(Para,&default_RN8302DeadZone,size);
					break;
				default:
					vPortFree(Para );
					Para =NULL;
					break;

				}
			}
		}
	 }
	 return Para;
 }
 static void save_para(void*Para ,uint8_t size, uint8_t NvsID)
 {
	 int rc = 0;
	 if(Para != NULL)
	 {
		rc = nvs_write(&fs, NvsID, Para, size);
		vPortFree(Para);
		Para =NULL;
	 }
 }
static void WriteDevPara(UCHAR * pucRegBuffer,USHORT usAddress,USHORT usNRegs)
{
	int rc = 0;
	struct rs485_para* dev_para = NULL;
	USHORT cmd;
	int data;
	float *value = &data;
	sRN8302RatioPara_TypeDef* RN8302Ratio = NULL;
	sRN8302DeadZonePara_TypeDef* RN8302DeadZone = NULL;
	switch (usAddress)
	{
	case 1000:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if( dev_para != NULL)
		{
			dev_para->address  = (uint8_t)(*pucRegBuffer++ << 8 | *pucRegBuffer++);
		}
		if(--usNRegs == 0)
		{
			save_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
			break;
		}

	case 1001:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if( dev_para != NULL)
		{
			dev_para->BaudRate = *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			dev_para = NULL;
			break;
		}
	case 1002:
		if(dev_para == NULL)
		{
			/*未从高16位写入，不能修改*/
		}
		else
		{
			dev_para->BaudRate = dev_para->BaudRate |*pucRegBuffer++ << 8 | *pucRegBuffer++;
		}
		if(--usNRegs == 0)
		{
			save_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
			break;
		}
	case 1003:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if( dev_para != NULL)
		{
			dev_para->DataBits = (uint8_t)(*pucRegBuffer++ << 8 | *pucRegBuffer++);
		}
		if(--usNRegs == 0)
		{
			save_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
			break;
		}
	case 1004:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if( dev_para != NULL)
		{
			dev_para->Parity = (uint8_t)(*pucRegBuffer++ << 8 | *pucRegBuffer++);
		}
		if(--usNRegs == 0)
		{
			save_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
			break;
		}
	case 1005:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if( dev_para != NULL)
		{
			dev_para->StopBits = (uint8_t)(*pucRegBuffer++ << 8 | *pucRegBuffer++);
		}
		save_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		if(--usNRegs == 0)
		{
			break;
		}

		/* 变比*/
	case 1006:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[0]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1007:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[0]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
			break;
		}
	case 1008:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[1]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1009:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[1]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
			break;
		}
	case 1010:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[2]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1011:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->VRmsConst[2]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
			break;
		}
	case 1012:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->IRmsConst[0]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1013:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->IRmsConst[0]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
			break;
		}
	case 1014:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
		RN8302Ratio->IRmsConst[1]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1015:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->IRmsConst[1]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
			break;
		}
	case 1016:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->IRmsConst[2]  =  *pucRegBuffer++ << 24 | *pucRegBuffer++ << 16 ;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			RN8302Ratio = NULL;
			break;
		}
	case 1017:
		if(RN8302Ratio != NULL)
		{
			RN8302Ratio->IRmsConst[2]  |=  *pucRegBuffer++ << 8 | *pucRegBuffer++;;
		}
		save_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		if(--usNRegs == 0)
		{

			break;
		}
		/* 死区*/
	case 1018:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
		 	data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;

		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1019:
		if(RN8302DeadZone != NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;

			RN8302DeadZone->VDzConst[0] = *value ;
			//memcpy(&(RN8302DeadZone->VDzConst[0]),data,sizeof(float));
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
			break;
		}
	case 1020:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
			data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1021:
		if(RN8302DeadZone != NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;
			RN8302DeadZone->VDzConst[1] = *value ;
			//memcpy(&RN8302DeadZone->VDzConst[1],data,sizeof(float));
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
			break;
		}
	case 1022:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
			data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1023:
		if(RN8302DeadZone != NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;
			RN8302DeadZone->VDzConst[2] = *value ;
			//memcpy(&RN8302DeadZone->VDzConst[2],data,sizeof(float));
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
			break;
		}
	case 1024:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
			data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1025:
		if(RN8302DeadZone != NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;
			RN8302DeadZone->IDzConst[0] = *value ;
			//memcpy(&(RN8302DeadZone->IDzConst[0]),data,sizeof(float));
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
			break;
		}
	case 1026:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
			data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1027:
		if(RN8302DeadZone != NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;
			RN8302DeadZone->IDzConst[1] = *value ;
			//memcpy(&RN8302DeadZone->IDzConst[1],data,sizeof(float));
		}
		if(--usNRegs == 0)
		{
			save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
			break;
		}
	case 1028:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(RN8302DeadZone != NULL)
		{
			data =  (*pucRegBuffer++ << 24 | *pucRegBuffer++ << 16) & 0xffff0000;
		}
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			RN8302DeadZone = NULL;
			break;
		}
	case 1029:
		if(RN8302DeadZone == NULL)
		{
			data |= *pucRegBuffer++ << 8 | *pucRegBuffer++ ;
			RN8302DeadZone->IDzConst[2] = *value ;
			//memcpy(&RN8302DeadZone->IDzConst[1],data,sizeof(float));
		}
		save_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		if(--usNRegs == 0)
		{
			break;
		}
	case 1030:
		cmd =  *pucRegBuffer++ << 8 | *pucRegBuffer++  ;
		if(cmd == 0xA5A5)
		{
			cmdline_input("clean_energy");
		}
		break;
	case 1031:
		cmd =  *pucRegBuffer++ << 8 | *pucRegBuffer++  ;
		if(cmd == 0xA5A5)
		{
			cmdline_input("reset");
		}
			break;
	default:break;
	}

}
static void ReadDevPara(UCHAR * pucRegBuffer,USHORT usAddress,USHORT usNRegs)
{
	struct rs485_para* dev_para = NULL;
	int* data;
	sRN8302RatioPara_TypeDef* RN8302Ratio = NULL;
	sRN8302DeadZonePara_TypeDef* RN8302DeadZone = NULL;
	switch (usAddress)
	{
	case 1000:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = dev_para->address;
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			break;
		}

	case 1001:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(dev_para->BaudRate>>24);
		*pucRegBuffer++ = (UCHAR)(dev_para->BaudRate>>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			break;
		}
	case 1002:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(dev_para->BaudRate>>8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(dev_para->BaudRate & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			break;
		}
	case 1003:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = dev_para->DataBits;
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			break;
		}
	case 1004:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = dev_para->Parity;
		if(--usNRegs == 0)
		{
			vPortFree(dev_para);
			break;
		}
	case 1005:
		dev_para = get_para(dev_para,sizeof(struct rs485_para),RS485_PARA_ID);
		*pucRegBuffer++ = 0;
		*pucRegBuffer++ = dev_para->StopBits;
		vPortFree(dev_para);

		if(--usNRegs == 0)
		{
			break;
		}

		/* 变比*/
	case 1006:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[0] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[0] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1007:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[0] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[0] & 0x00FF) ;
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1008:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[1] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[1] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1009:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[1] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[1] & 0x00FF) ;
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1010:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[2] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[2] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1011:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[2] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->VRmsConst[2] & 0x00FF) ;
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
		}
	case 1012:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[0] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[0] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1013:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[0] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[0] & 0x00FF) ;
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1014:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[1] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[1] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1015:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[1] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[1] & 0x00FF) ;
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1016:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[2] >>24 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[2] >>16 & 0x00ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302Ratio);
			break;
		}
	case 1017:
		RN8302Ratio = get_para(RN8302Ratio,sizeof(sRN8302RatioPara_TypeDef),RN8302B_RATIO_PARA_ID);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[2] >> 8 & 0x00ff);
		*pucRegBuffer++ = (UCHAR)(RN8302Ratio->IRmsConst[2] & 0x00FF) ;
		vPortFree(RN8302Ratio);
		if(--usNRegs == 0)
		{

			break;
		}
		/* 死区*/
	case 1018:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[0];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1019:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[0];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1020:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[1];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1021:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[1];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1022:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[2];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1023:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->VDzConst[2];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
		    vPortFree(RN8302DeadZone);
			break;
		}
	case 1024:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[0];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1025:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[0];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1026:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[1];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1027:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[1];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1028:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[2];
		*pucRegBuffer++ = (UCHAR)(*data >>24 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data >>16 & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
	case 1029:
		RN8302DeadZone = get_para(RN8302DeadZone,sizeof(sRN8302DeadZonePara_TypeDef),RN8302B_DEADZONE_PARA_ID);
		data = &RN8302DeadZone->IDzConst[2];
		*pucRegBuffer++ = (UCHAR)(*data >>8 & 0x000000ff);
		*pucRegBuffer++ = (UCHAR)(*data & 0x000000ff);
		if(--usNRegs == 0)
		{
			vPortFree(RN8302DeadZone);
			break;
		}
		vPortFree(RN8302DeadZone);

	case 1030:
		*pucRegBuffer++ = (UCHAR)(0);
		*pucRegBuffer++ = (UCHAR)(0);
		if(--usNRegs == 0)
		{

			break;
		}
	case 1031:
		*pucRegBuffer++ = (UCHAR)(0);
		*pucRegBuffer++ = (UCHAR)(0);
		break;
	default:break;
	}

	}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
                             USHORT usNRegs, eMBRegisterMode eMode) {
  eMBErrorCode eStatus = MB_ENOERR;
  if ((usAddress >= REG_HOLDING_START)
      && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS)) {
    switch (eMode) {
      case MB_REG_READ:
//        while (usNRegs > 0) {
//          *pucRegBuffer++ = (unsigned char) (usRegHoldingBuf[iRegIndex] >> 8);
//          *pucRegBuffer++ = (unsigned char) (usRegHoldingBuf[iRegIndex] & 0xFF);
//          iRegIndex++;
//          usNRegs--;
//        }
    	  if(usNRegs > 0)
    	  {
    		  ReadDevPara(pucRegBuffer,usAddress,usNRegs);
    	  }
        break;

      case MB_REG_WRITE:
//        while (usNRegs > 0) {
//          usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
//          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
//          iRegIndex++;
//          usNRegs--;
//        }
    	  if(usNRegs > 0)
    	  {
    		  WriteDevPara(pucRegBuffer,usAddress,usNRegs);
    	  }
    }
  }
  else {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                           USHORT usNRegs)
{
  eMBErrorCode eStatus = MB_ENOERR;
  int iRegIndex;

  if ((usAddress >= REG_INPUT_START)
      && (usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS)) {
    iRegIndex = (int) (usAddress - usRegInputStart);
    while (usNRegs > 0) {
        *pucRegBuffer++ = (unsigned char) (((uint16_t*)usRegInputBuf)[iRegIndex] >> 8);
        *pucRegBuffer++ = (unsigned char) (((uint16_t*)usRegInputBuf)[iRegIndex] & 0xFF);
        iRegIndex++;
        usNRegs--;
    }
  } else {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

/* Exported functions --------------------------------------------------------*/



