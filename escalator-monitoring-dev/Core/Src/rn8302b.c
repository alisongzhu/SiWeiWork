/*
 * rb8302b.c
 *
 *  Created on:
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include <rn8302b.h>
#include <gpio.h>
#include <rn8302b_priv.h>
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "nvs/nvs.h"
#include "flash.h"
#include "bsp_nvs_config.h"
#include <fram.h>
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "stdlib.h"
#include "stddef.h"
#include "string.h"
#include "user_regs.h"
#include "crc8.h"
/* Private macros ------------------------------------------------------------*/
#define  ConstE_VALUE             (3200)
#define  HFConst1_VALUE           (44764)
#define  HFConst2_VALUE           HFConst1_VALUE

#define VOLTAGE_CONST           ((float)200000)
#define CURRENT_CONST           ((float)14000000)
#define POWER_CONST             ((float)2.99593e-6)
#define FACTOR_CONST            ((float)8388608.0)

#define  GETWRITECMDADDR(wReg)    ((((uint8_t)(wReg >> 4)) & 0xf0) + 0x80)
#define  GETREADCMDADDR(wReg)     (((uint8_t)(wReg >> 4)) & 0xf0)
#define  GETREGADDR(wReg)         (wReg & 0x00ff)

#define SPI_RX_BUF_SIZE     10
#define SPI_TX_BUF_SIZE     10
#define SPI_CS_ENABE()      HAL_GPIO_WritePin(RN8302B_SPI2_CS_GPIO_Port,RN8302B_SPI2_CS_Pin,GPIO_PIN_RESET);
#define SPI_CS_DASABE()     HAL_GPIO_WritePin(RN8302B_SPI2_CS_GPIO_Port,RN8302B_SPI2_CS_Pin,GPIO_PIN_SET);

#define  Standard_Voltage     (44000000)
#define  VIN_STA              (220)
#define  Standard_Current     (21000000)//(70000000)
#define  CURRENT_STA          (1.5)
#define  PA_HALF_STA          (55074691)

#define  VOLTTHRESHOLDUINT    1 //(Standard_Voltage /VIN_STA  )     //  threshold unit 1V
#define  CURRETHESHOLDUINT    0.1f// (Standard_Current/(CURRENT_STA*10 ))  //  threshold unit 0.1A

#define  D_ZXOT              (((int32_t )(Standard_Current*0.05f))>>12)
#define  D_LostVoltage       (((int32_t )(Standard_Voltage*0.5f))>>12)

#define  VOLTTHRESHOLDUINT    1 //(Standard_Voltage /VIN_STA  )     //  threshold unit 1V
#define  CURRETHESHOLDUINT    0.1f// (Standard_Current/(CURRENT_STA*10 ))  //  threshold unit 0.1A

#define  READTIME             5
#define  FRAMOVERTIME         1000
#define  ENERGYFRAMADDR_A     (uint16_t)(0)
#define  ENERGYFRAMADDR_B     (uint16_t)(256)
#define  CALIBRATELIMIT       (0.2f)
#define  RN8302_ENERGY_NUM    10
#define  Pi                   (3.1415)

#define READ_LIST_NUM         REG_INPUT_RN8302B_LIST_SIZE
/* Private types -------------------------------------------------------------*/
typedef enum
{
	Voltage = 1,
	Current,
	Power,
	energy,
	freque,
	factor,
	Sum,

}DataHandleTypeDef;
typedef struct Rn8302_ConstParadef
{
	uint16_t Address;
	uint8_t  Size;
	DataHandleTypeDef DataHandle;
	const char *DatName;
}Rn8302_ConstParadef;

typedef struct EnergySavedef
{
	uint64_t Data;
	uint8_t num;
	uint8_t Crc;
}__packed ;
typedef struct EnergySavedef EnergySavedef;
 const Rn8302_ConstParadef  Rn8302_Const[READ_LIST_NUM]={
	{
		.Address=0x007,/*A Voltage*/
		.Size=4,
		.DataHandle=Voltage,
		.DatName ="A Voltage"
	},/*0*/
	{
		.Address=0x008,/*B Voltage*/
		.Size=4,
		.DataHandle=Voltage,
		.DatName ="B Voltage"
	},/*1*/
	{
		.Address=0x009,/*C Voltage*/
		.Size=4,
		.DataHandle=Voltage,
		.DatName ="C Voltage"
	},/*2*/
	{
		.Address=0x00b,/*A Current*/
		.Size=4,
		.DataHandle=Current,
		.DatName ="A Current"
	},/*3*/
	{
		.Address=0x00c,/*B Current*/
		.Size=4,
		.DataHandle=Current,
		.DatName ="B Current"
	},/*4*/
	{
		.Address=0x00d,/*C Current*/
		.Size=4,
		.DataHandle=Current,
		.DatName ="C Current"
	},/*5*/
	{
		.Address=0x057,/*频率*/
		.Size=3,
		.DataHandle=freque,
		.DatName ="frequency"
	},/*6*/
	{
		.Address=0x017,/*有功功率和*/
		.Size=4,
		.DataHandle=Sum,
		.DatName ="Total active power"
	},/*7*/
	{
		.Address=0x01b,/*无功功率和*/
		.Size=4,
		.DataHandle=Sum,
		.DatName ="Total reactive power"
	},/*8*/
	{
		.Address=0x023,/*功率因数和*/
		.Size=3,
		.DataHandle=factor,
		.DatName ="Total power factor"
	},/*9*/
	{
		.Address=0x037,/*总正向有功电能*/
		.Size=3,
		.DataHandle=Sum,
		.DatName ="Total positive active energy"
	},/*10*/
	{
		.Address=0x03f,/*总正向无功电能*/
		.Size=3,
		.DataHandle=Sum,
		.DatName ="Total forward reactive energy"
	},/*11*/
	{
		.Address=0x033,/*总有功电能*/
		.Size=3,
		.DataHandle=Sum,
		.DatName ="Total reactive energy"
	},/*12*/
	{
		.Address=0x014,/*A 有功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="A active power"
	},/*13*/
	{
		.Address=0x015,/*B 有功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="B active power"
	},/*14*/
	{
		.Address=0x016,/*c 有功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="B active power"
	},/*15*/
	{
		.Address=0x018,/*A 无功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="A 无功功率*"
	},/*16*/
	{
		.Address=0x019,/*B 无功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="B 无功功率"
	},/*17*/
	{
		.Address=0x01a,/*C 无功功率*/
		.Size=4,
		.DataHandle=Power,
		.DatName ="C 无功功率"
	},/*18*/
	{
		.Address=0x020,/*A 功率因数*/
		.Size=3,
		.DataHandle=factor,
		.DatName ="A 功率因数"
	},/*19*/
	{
		.Address=0x021,/*B 功率因数*/
		.Size=3,
		.DataHandle=factor,
		.DatName ="B 功率因数"
	},/*20*/
	{
		.Address=0x022,/*C 功率因数*/
		.Size=3,
		.DataHandle=factor,
		.DatName ="C 功率因数"
	},/*21*/
	{
		.Address=0x030,/*A有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="A有功电能"
	},/*22*/
	{
		.Address=0x031,/*/*B有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="B有功电能"
	},/*23*/
	{
		.Address=0x032,/*C有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C有功电能"
	},/*24*/
	{
		.Address=0x034,/*A正向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="A正向有功电能"
	},/*25*/
	{
		.Address=0x035,/*/*B正向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="B正向有功电能"
	},/*26*/
	{
		.Address=0x036,/*C正向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C正向有功电能"
	},/*27*/
	{
		.Address=0x040,/*A正向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="A正向无功电能"
	},/*28*/
	{
		.Address=0x041,/*/*B正向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="B正向无功电能"
	},/*29*/
	{
		.Address=0x042,/*C正向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C正向无功电能*"
	},/*30*/
	{
		.Address=0x038,/*A反向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C正向无功电能*"
	},/*31*/
	{
		.Address=0x039,/*/*B反向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="B反向有功电能"
	},/*32*/
	{
		.Address=0x03a,/*C反向有功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C反向有功电能"
	},/*33*/
	{
		.Address=0x044,/*A反向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="A反向无功电能"
	},/*34*/
	{
		.Address=0x045,/*/*B反向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="B反向无功电能"
	},/*35*/
	{
		.Address=0x046,/*C反向无功电能*/
		.Size=3,
		.DataHandle=energy,
		.DatName ="C反向无功电能"
	},/*36*/
};

/* Private constants ---------------------------------------------------------*/


static BaseType_t prvRN8302bCalibrate( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRN8302bCleanEnergy( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvWriteRatio( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReadRatio( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvWriteDeadZone( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReadDeadZone( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvread_Meter( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReadRN8302bCalibrate( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static uint64_t prvSaveEnergy_Fram(uint32_t EnergyData,uint16_t addr);
static const CLI_Command_Definition_t xCalibrateCommand =
{
  "calibrate",
  "\r\ncalibrate:\r\n Calibration electricity meter\r\n",
  prvRN8302bCalibrate,
  0
};

static const CLI_Command_Definition_t xReadCalibrateCommand =
{
  "read_cali",
  "\r\nread_cali:\r\n read electricity meter Calibration\r\n",
  prvReadRN8302bCalibrate,
  0
};
static const CLI_Command_Definition_t xCleanEnergyCommand =
{
  "clean_energy",
  "\r\nclean_energy:\r\n clean electricity meter energy data\r\n",
  prvRN8302bCleanEnergy,
  0
};
static const CLI_Command_Definition_t xWriteRatioCommand =
{
  "set_ratio",
  "\r\nset_ratio:\r\n set ratio IRmsConst1-3 VRmsConst1-3 eg.g:set_deadzone 10 10 10 5 5 5;;\r\n",
  prvWriteRatio,
  6
};
static const CLI_Command_Definition_t xReadRatioCommand =
{
  "read_ratio",
  "\r\nread_ratio:\r\n read ratio IRmsConst1-3 VRmsConst1-3 ;\r\n",
  prvReadRatio,
  0
};
static const CLI_Command_Definition_t xWriteDeadZoneCommand =
{
  "set_deadzone",
  "\r\nset_deadzone:\r\n set deadzone IdzConst1-3 VdzConst1-3 eg.g:set_deadzone 10 10 10 0.1 0.1 0.1;\r\n",
  prvWriteDeadZone,
  6
};
static const CLI_Command_Definition_t xReadDeadZoneCommand =
{
  "read_deadzone",
  "\r\nread_deadzone:\r\n read deadzone IdzConst1-3 VdzConst1-3;\r\n",
  prvReadDeadZone,
  0
};
static const CLI_Command_Definition_t xReadEleMeterCommand =
{
  "read_meter",
  "\r\nread_Meter:\r\n read deadzone IdzConst1-3 VdzConst1-3;\r\n",
  prvread_Meter,
  0
};
/* Private variables ---------------------------------------------------------*/
//static struct rn8302b_priv rn8302b_data;

static sRN8302RatioPara_TypeDef RN8302RatioPara ;
static sRN8302DeadZonePara_TypeDef RN8302DeadZonePara ;
/* Global variables ----------------------------------------------------------*/

float RN8302B_DataTable_Front[READ_LIST_SIZE] = {0};


/* Private function prototypes -----------------------------------------------*/
static ErrorStatus RN8302_WriteReg(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t *pBuf, uint8_t DatLen);
static ErrorStatus RN8302_WriteDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);
static ErrorStatus RN8302_ReadReg(SPI_HandleTypeDef *hspi, uint16_t wReg, int32_t* val, uint8_t DatLen);
static ErrorStatus RN8302_ReadDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);

static float ProcVoltageData(int voltage,uint16_t addr);
static float ProcCurrentData(int current,uint16_t addr);
static float ProcPowerData(int power,uint16_t addr);
static float ProcPowerFactorData(int fator);
static uint32_t   ProcEnergyrData(int fator ,uint16_t addr);
static float ProcFrequencyData(int fator);
static BaseType_t rn8302ParaInit(void);
/* Private functions ---------------------------------------------------------*/

ErrorStatus RN8302_WriteReg(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t *pBuf, uint8_t DatLen)
{
  uint8_t i, chksum = 0, Repeat;
  uint8_t DatP[8] ;
  ErrorStatus err = SUCCESS;
  if ((DatLen == 0) || (DatLen > 4)) return(ERROR);

  for (Repeat = 3; Repeat != 0; Repeat--)
  {

    SPI_CS_ENABE()
    DatP[0] = (uint8_t)GETREGADDR(wReg);
    DatP[1] = GETWRITECMDADDR(wReg);
    for (i = 0; i < DatLen; i++)
    {
      DatP[2 + i] = pBuf[DatLen - 1 - i];
    }
    for (i = 0; i < DatLen + 2; i++)
    {
      chksum += DatP[i];
    }
    DatP[DatLen + 2] = ~chksum;
    HAL_SPI_Transmit(hspi, DatP, DatLen + 1 + 2, 0XFFF);
    SPI_CS_DASABE()


    err = RN8302_WriteDataCheck(hspi, 0X018D, pBuf, DatLen);
    if (err == SUCCESS)
    {
      break;
    }

  }

  return(err);
}

ErrorStatus RN8302_WriteDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen)
{
  uint8_t i, chksum = 0;
  uint8_t *DatP = NULL;
  uint8_t ReadLast[6] = { 0 };
  ErrorStatus err = SUCCESS;
  SPI_CS_ENABE()
  ReadLast[0] = (uint8_t)GETREGADDR(wReg);
  ReadLast[1] = GETREADCMDADDR(wReg);
  HAL_SPI_Transmit(hspi, ReadLast, 2, 0XFF);//

  HAL_SPI_Receive(hspi, &ReadLast[2], 4, 0XFF);
  SPI_CS_DASABE()
  DatP = &ReadLast[2];
  for (i = 3; i > 0; i--)
  {
    if (DatLen >= i)
    {
      if (*DatP != pBuf[i - 1])
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
    if (ReadLast[5] != chksum)  err = ERROR;
  }
  return(err);
}


 ErrorStatus RN8302_ReadReg(SPI_HandleTypeDef *hspi, uint16_t wReg, int32_t* val, uint8_t DatLen)
{
  uint8_t i, chksum = 0, Repeat;
  uint8_t DatP[SPI_TX_BUF_SIZE];
  uint8_t *pBuf = (uint8_t*)val;
  ErrorStatus err = SUCCESS;
  if (DatLen == 0) return(ERROR);

  for (Repeat = 3; Repeat != 0; Repeat--)
  {

    SPI_CS_ENABE()
    DatP[0] = (uint8_t)GETREGADDR(wReg);
    DatP[1] = GETREADCMDADDR(wReg);

    HAL_SPI_Transmit(hspi, DatP, 2, 0XFF);//

    HAL_SPI_Receive(hspi, &DatP[2], DatLen + 1, 0XFF);
    SPI_CS_DASABE()
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


    err = RN8302_ReadDataCheck(hspi, 0X018C, pBuf, DatLen);
    if (err == SUCCESS)
    {
      break;
    }

  }
//  SPI_CS_DASABE()
  return(err);
}

 ErrorStatus RN8302_ReadDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen)
{
  uint8_t chksum = 0;
  uint8_t *DatP = NULL;
  uint8_t ReadLast[7] = { 0 };
  uint32_t i;
  ErrorStatus err = SUCCESS;

  SPI_CS_ENABE()
  ReadLast[0] = (uint8_t)GETREGADDR(wReg);
  ReadLast[1] = GETREADCMDADDR(wReg);
  HAL_SPI_Transmit(hspi, ReadLast, 2, 0XFF);//

  HAL_SPI_Receive(hspi, &ReadLast[2], 5, 0XFF);
  SPI_CS_DASABE()
  DatP = &ReadLast[2];
  for (i = 4; i > 0; i--)
  {
    if (DatLen >= i)
    {
      if (*DatP != pBuf[i - 1])
      {
        err = ERROR;
        break;
      }

    }
    DatP++;
  }
  for ( i = 0; i < 6; i++)
  {
    chksum += ReadLast[i];
  }
  chksum = ~chksum;
  if (err == SUCCESS)
  {

    if (ReadLast[6] != chksum)  err = ERROR;
  }


  return(err);
}

static float ProcVoltageData(int voltage,uint16_t addr)
{
	uint8_t i ;
	float Value;
	switch(addr)
	{
	case 0x0007:
	i = 0;
	break;
	case 0x0008:
	i = 1;
	break;
	case 0x0009:
	i = 2;
	break;
	default:i = 1;
	break;
	}
	Value = (float)(voltage/VOLTAGE_CONST*RN8302RatioPara.VRmsConst[i]);
	if(Value < RN8302DeadZonePara.VDzConst[i++])
	{
	   Value = 0;
	}
	return Value;
}

static float ProcCurrentData(int current,uint16_t addr)
{
	uint8_t i ;
	float Value;
	switch(addr)
	{
	case 0x000b:
	i = 0;
	break;
	case 0x000c:
	i = 1;
	break;
	case 0x000d:
	i = 2;
	break;
	default:i = 1;
	break;
	}
	Value = (float)(((float)current)/CURRENT_CONST*RN8302RatioPara.IRmsConst[i]);
	if(Value < RN8302DeadZonePara.IDzConst[i++])
	{
	   Value = 0;
	}
	return Value;
}

static float ProcPowerData(int power,uint16_t addr)
{
	uint8_t i ;
	float Value;
	switch(addr)
	{
	case 0x0014:
	i = 0;
	break;
	case 0x0015:
	i = 1;
	break;
	case 0x0016:
	i = 2;
	break;
	case 0x0018:
	i = 0;
	break;
	case 0x0019:
	i = 1;
	break;
	case 0x001a:
	i = 2;
	break;
	case 0x0017:
	i = 0;
	break;
	case 0x001b:
	i = 0;
	break;
	default:i = 1;
	break;
	}
	Value =  power*POWER_CONST*RN8302RatioPara.IRmsConst[i]*RN8302RatioPara.VRmsConst[i];
	if(abs(Value) < (RN8302DeadZonePara.IDzConst[i]*RN8302DeadZonePara.IDzConst[i]))
	{
	   Value = 0;
	}
	return Value;
};

static float ProcPowerFactorData(int fator)
{
	float data = 0;
	fator &= 0xffffff;
	if(fator  > FACTOR_CONST)
	fator = (~fator)&0x7fffff +1;
    data=((float)fator)/FACTOR_CONST;
    if(data > 1 || data < 0)
    {
    	data = 0;
    }
    return data;
}

static uint32_t ProcEnergyrData(int fator ,uint16_t addr)
{
	uint8_t PashNum;
	uint64_t EnergData;
	uint32_t value=0,Energdata;
	PashNum =(uint8_t)( (addr-0x30)%4);
	if(PashNum>3) return value;
	Energdata = (fator&0x00ffffff)*RN8302RatioPara.IRmsConst[PashNum]*RN8302RatioPara.VRmsConst[PashNum] ;
	EnergData = prvSaveEnergy_Fram((uint32_t) Energdata,addr);
	value = (uint32_t)( EnergData/(ConstE_VALUE/100));
	return value;
}
static float ProcFrequencyData(int fator )
{
  return 65536000.0/(fator&0x00ffffff);
}

static uint64_t prvSaveEnergy_Fram(uint32_t EnergyData,uint16_t addr)
{
	EnergySavedef FramDataA ,FramDataB ,ReData, EnergData;
	uint16_t FramAddrA,FramAddrB;

	FramAddrA = ENERGYFRAMADDR_A + (uint16_t)(addr-0x30)*(sizeof(EnergySavedef));
	FramAddrB = ENERGYFRAMADDR_B + (uint16_t)(addr-0x30)*(sizeof(EnergySavedef));
	/*read A sector data*/
	taskENTER_CRITICAL();
	FramDataA.Data = 0;
	FramDataB.Data = 0;
	EnergData.Data = EnergyData;
	ReData = EnergData;
	fram_read((uint16_t)( FramAddrA),(uint8_t*)&FramDataA, sizeof(EnergySavedef));
	fram_read((uint16_t)( FramAddrB),(uint8_t*)&FramDataB, sizeof(EnergySavedef));
	/*A setor crc */
	if(FramDataA.Crc == crc8_ccitt(0xff, &FramDataA, offsetof(EnergySavedef,Crc)) && (FramDataA.num >= FramDataB.num || FramDataB.Crc != crc8_ccitt(0xff, &FramDataB, offsetof(EnergySavedef,Crc))))
	{
		EnergData.Data =EnergyData + FramDataA.Data;
		EnergData.num = FramDataA.num+1;
		EnergData.Crc = crc8_ccitt(0xff,&EnergData,offsetof(EnergySavedef,Crc));
		ReData = EnergData;
		fram_write((uint16_t)( FramAddrB),(uint8_t*)&EnergData, sizeof(EnergySavedef));
		fram_read((uint16_t)( FramAddrB),(uint8_t*)&FramDataB, sizeof(EnergySavedef));
		if(FramDataB.Crc== ReData.Crc)
		{
			EnergData =ReData ;
			fram_write((uint16_t)( FramAddrA),(uint8_t*)&EnergData, sizeof(EnergySavedef));
		}
		else
		{
			EnergData =ReData;
			fram_write((uint16_t)( FramAddrB),(uint8_t*)&EnergData, sizeof(EnergySavedef));
		}
	}
	else
	{
		if(FramDataB.Crc == crc8_ccitt(0xff, &FramDataB, offsetof(EnergySavedef,Crc)))
		{
			EnergData.Data = EnergyData + FramDataB.Data+1;
			EnergData.num = FramDataB.num+1;
			EnergData.Crc = crc8_ccitt(0xff,&EnergData,offsetof(EnergySavedef,Crc));
			ReData = EnergData;
			fram_write((uint16_t)( FramAddrA),(uint8_t*)&EnergData, sizeof(EnergySavedef));
			fram_read((uint16_t)( FramAddrA), (uint8_t*)&FramDataA,sizeof(EnergySavedef));
			if(FramDataA.Crc== ReData.Crc)
			{
				EnergData =ReData;
				fram_write((uint16_t)( FramAddrB), (uint8_t*)&EnergData,sizeof(EnergySavedef));
			}
			else
			{
				EnergData =ReData;
				fram_write((uint16_t)( FramAddrA),(uint8_t*)&EnergData, sizeof(EnergySavedef));
			}
		}
	}
	taskEXIT_CRITICAL();
	return  ReData.Data;
}

static BaseType_t prvRN8302bCalibrate( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	sRN8302FirmParaFile_TypeDef RN8302FirmPara;
	int rc = 0;
	uint8_t caCunter=0;
	uint32_t ReadVa[3][READTIME]={{0},{0},{0}};
	uint32_t ReadIa[3][READTIME]={{0},{0},{0}};
	uint32_t ReadPa[3][READTIME]={{0},{0},{0}};
	uint32_t ReadDat = 0 ,Dat;
	int32_t ReadVDat,ReadIDat,ReadPDat;
	uint8_t i,j,k,a;
	__IO float data;
	rc = nvs_read(&fs, RN8302B_PARA_ID, &RN8302FirmPara,
				sizeof(sRN8302FirmParaFile_TypeDef));
	if (rc < 0)
	RN8302FirmPara = default_RN8302FirmPara ;

	taskENTER_CRITICAL();

   	sRN8302StruDataComm_TypeDef RN8302DataComm;
  	RN8302DataComm.ucTemp8 = 0xe5;
  	RN8302_WriteReg(&hspi2, WREN, RN8302DataComm.ucTempBuf, 1);

  	RN8302DataComm.wTemp16 = HFConst1_VALUE;
  	RN8302_WriteReg(&hspi2, HFCONST1, RN8302DataComm.ucTempBuf, 2);

  	RN8302DataComm.wTemp16 = HFConst2_VALUE;
  	RN8302_WriteReg(&hspi2, HFCONST2, RN8302DataComm.ucTempBuf, 2);

	RN8302_WriteReg(&hspi2,PHSUA,(uint8_t *)&(default_RN8302FirmPara.PHSU[0]),1);
	RN8302_WriteReg(&hspi2,PHSUB,(uint8_t *)&(default_RN8302FirmPara.PHSU[1]),1);
	RN8302_WriteReg(&hspi2,PHSUC,(uint8_t *)&(default_RN8302FirmPara.PHSU[2]),1);
    HAL_Delay(2000);
    /*相位校准*/
   	for(i = 0; i < READTIME ;i++)
   	{
   		for(j = 0 ; j < 3; j++)
   		{
   			RN8302_ReadReg(&hspi2,0x014+j,(int32_t *)&ReadDat,4);
   			ReadPa[j][i] = ReadDat;
   			for(k=i;k<i+1;k++)
   			{
   				a=k;
   				while(a>0 && ReadPa[j][a] < ReadPa[j][a -1] )
   				{
   					Dat= ReadPa[j][a -1];
   					ReadPa[j][a] = ReadPa[j][a -1];
   					ReadPa[j][a -1] = Dat;
   					a--;
   				}
   			}
   		}

           HAL_Delay(300);
   	}

   	for(j = 0;j < 3 ; j++)
   	{
   		ReadPDat = 0;
 		for(i = 1; i<READTIME-1;i++)
 		{
 			ReadPDat += ReadPa[j][i];
 		}
 		ReadPDat = ReadPDat/(READTIME-2);
 		/* 确保相位校正，输入功率因数0.5时，测得功率值和理论值相差在正负0.2，才会进行功率相位校正*/
 		if(abs(PA_HALF_STA -ReadPDat) < (CALIBRATELIMIT*PA_HALF_STA))
 		{
 			data = (PA_HALF_STA -ReadPDat  )/(PA_HALF_STA*1.732);
 			data = asinf(data)*180.0/Pi;
 			RN8302FirmPara.PHSU[j] = 0x80  + (int16_t)(data/0.017578);
 			rc = RN8302_WriteReg(&hspi2, PHSUA + j, (uint8_t *)&(RN8302FirmPara.PHSU[j]), 1);
 			if(rc == ERROR)
 			{
 				strncpy(pcWriteBuffer,"RN8302 Write error\r\n",xWriteBufferLen);
 				taskEXIT_CRITICAL();
 				return xReturn;
 			}
 			caCunter++;
 		}
 		else
 		{
 			break;
 		}

 	}
   	if(caCunter != 0)/*如果输入功率因素在0.5，保存校验值并退出*/
   	{
   		goto end;
   	}
    /*电压电流校准*/

   	/*清零校验值*/
   	ReadDat = 0;
  	RN8302_WriteReg(&hspi2, GSUA, (uint8_t*)&ReadDat, 2);
  	RN8302_WriteReg(&hspi2, GSUB, (uint8_t*)&ReadDat, 2);
  	RN8302_WriteReg(&hspi2, GSUC, (uint8_t*)&ReadDat, 2);

  	RN8302_WriteReg(&hspi2, GSIA, (uint8_t*)&ReadDat, 2);
  	RN8302_WriteReg(&hspi2, GSIB, (uint8_t*)&ReadDat, 2);
  	RN8302_WriteReg(&hspi2, GSIC, (uint8_t*)&ReadDat, 2);
  	RN8302_WriteReg(&hspi2, GSIN, (uint8_t*)&ReadDat, 2);
    HAL_Delay(2000);
  	for(i = 0; i < READTIME ;i++)
  	{
  		for(j = 0 ; j < 3; j++)
  		{
  			RN8302_ReadReg(&hspi2,0x007+j,(int32_t *)&ReadDat,4);
  			ReadVa[j][i] = ReadDat;
  			for(k=i;k<i+1;k++)
  			{
  				a=k;
  				while(a>0 && ReadVa[j][a] < ReadVa[j][a -1] )
  				{
  					Dat= ReadVa[j][a -1];
  					ReadVa[j][a] = ReadVa[j][a -1];
  					ReadVa[j][a -1] = Dat;
  					a--;
  				}
  			}
  		}
  		for(j = 0 ; j < 3; j++)
  		{
  			RN8302_ReadReg(&hspi2,0x00b+j, (int32_t *)&ReadDat,4);
  			ReadIa[j][i] = ReadDat;
			for(k=i;k<i+1;k++)
				{
					a=k;
					while(a>0 && ReadIa[j][a] < ReadIa[j][a -1] )
					{
						Dat= ReadIa[j][a -1];
						ReadIa[j][a] = ReadIa[j][a -1];
						ReadIa[j][a -1] = Dat;
						a--;
					}
				}
  		}
          HAL_Delay(1000);
  	}

  	for(j = 0 ; j < 3; j++)
  	{
		ReadVDat = 0;
		ReadIDat = 0;
		for(i = 1; i<READTIME-1;i++)
		{
			ReadVDat += ReadVa[j][i];
			ReadIDat += ReadIa[j][i];
		}
		ReadVDat = ReadVDat/(READTIME-2);
		ReadIDat = ReadIDat/(READTIME-2);

  		RN8302FirmPara.VGain[j] =(uint16_t)(((float)Standard_Voltage/ReadVDat-1)*(32768));
  		/*读取功率，确保功率因数为1*/
  		RN8302_ReadReg(&hspi2,0x021+j, (int32_t *)&ReadDat,3);
  		if(RN8302FirmPara.VGain[j] < CALIBRATELIMIT *32768 && ReadDat >(1- CALIBRATELIMIT)*8388608)
  		{
  			rc = RN8302_WriteReg(&hspi2, GSUA + j, (uint8_t *)&(RN8302FirmPara.VGain[j]), 2);
  			if(rc == ERROR)
  	  		{
  	  			strncpy(pcWriteBuffer,"RN8302 Write error\r\n",xWriteBufferLen);
  	  			taskEXIT_CRITICAL();
  	  			return xReturn;
  	  		}
  			caCunter++;
  		}

  		RN8302FirmPara.IGain[j] =(uint16_t)( ((float)Standard_Current/ReadIDat-1)*(32768));
  		if(RN8302FirmPara.IGain[j] < CALIBRATELIMIT *32768 && ReadDat >(1- CALIBRATELIMIT)*8388608)
  		{
  			rc = RN8302_WriteReg(&hspi2, GSIA + j, (uint8_t *)&(RN8302FirmPara.IGain[j]), 2);
			if(rc == ERROR)
			{
				strncpy(pcWriteBuffer,"RN8302 Write error\r\n",xWriteBufferLen);
				taskEXIT_CRITICAL();
				return xReturn;
			}
  			caCunter++;
  		}

  	}

    HAL_Delay(500);

end:
/*写保护*/
	RN8302DataComm.ucTemp8 = 0xDC;
	RN8302_WriteReg(&hspi2, 0x0180, RN8302DataComm.ucTempBuf, 1);
  	taskEXIT_CRITICAL();

	rc = nvs_write(&fs, RN8302B_PARA_ID, &RN8302FirmPara,
				sizeof(sRN8302FirmParaFile_TypeDef));
	if(rc < 0)
	{
		strncpy(pcWriteBuffer,"NVS Write,error\r\n",xWriteBufferLen);
	}
	else
	{
		if(caCunter == 3)/*判断相位校验计数，计数值为3，相位校验完成*/
		{
	 		snprintf(pcWriteBuffer,xWriteBufferLen,"Calibrate successful \r\nIGA=%d IGB=%d IGC=%d VGA=%d VGB=%d VGC=%d PHA=%d PHB=%d PHC=%d\r\n"
	 			,RN8302FirmPara.IGain[0],RN8302FirmPara.IGain[1],RN8302FirmPara.IGain[2]
	 			,RN8302FirmPara.VGain[0],RN8302FirmPara.VGain[1],RN8302FirmPara.VGain[2]
				,RN8302FirmPara.PHSU[0]	,RN8302FirmPara.PHSU[1],RN8302FirmPara.PHSU[2]);
		}
		else if(caCunter == 6) /* 电压电流校验完成，未执行相位校验*/
		{
			strncpy(pcWriteBuffer,"Voltage and current correction completed, please confirm power factor =0.5 \r\n",xWriteBufferLen);
		}
		else
		{
			strncpy(pcWriteBuffer,"Calibrate failed, 220V 1.5A \r\n",xWriteBufferLen);

		}

	}
  return xReturn;
}
static BaseType_t prvReadRN8302bCalibrate( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	sRN8302FirmParaFile_TypeDef RN8302FirmPara;
	int rc = 0;
	rc = nvs_read(&fs, RN8302B_PARA_ID, &RN8302FirmPara,
					sizeof(sRN8302FirmParaFile_TypeDef));
	if(rc < 0)
	{
		RN8302FirmPara = default_RN8302FirmPara;
	}
	snprintf(pcWriteBuffer,xWriteBufferLen,"Calibrate successful \r\nIGA=%d IGB=%d IGC=%d VGA=%d VGB=%d VGC=%d PHA=%d PHB=%d PHC=%d\r\n"
		,RN8302FirmPara.IGain[0],RN8302FirmPara.IGain[1],RN8302FirmPara.IGain[2]
		,RN8302FirmPara.VGain[0],RN8302FirmPara.VGain[1],RN8302FirmPara.VGain[2]
		,RN8302FirmPara.PHSU[0]	,RN8302FirmPara.PHSU[1],RN8302FirmPara.PHSU[2]);

	  return xReturn;

}
static BaseType_t prvRN8302bCleanEnergy( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	uint8_t Sta = pdFALSE ;
	EnergySavedef  FramDataA ,FramDataB , EnergData;
	uint16_t i,FramAddrA,FramAddrB;
	taskENTER_CRITICAL();
	EnergData.Data = 0;
	EnergData. num = 1;
	EnergData.Crc = crc8_ccitt(0xff,&EnergData,offsetof(EnergySavedef,Crc));
	for(i=0;i<25;i++)
	{
		FramAddrA = ENERGYFRAMADDR_A + (i)*(sizeof(EnergySavedef));
	    FramAddrB = ENERGYFRAMADDR_B + (i)*(sizeof(EnergySavedef));
	    FramDataA = EnergData;
	    FramDataB = EnergData;
		fram_write((uint16_t)( FramAddrA),(uint8_t*)&FramDataA, sizeof(EnergySavedef));
		fram_write((uint16_t)( FramAddrB),(uint8_t*)&FramDataB, sizeof(EnergySavedef));
		fram_read((uint16_t)( FramAddrA), (uint8_t*)&FramDataA, sizeof(EnergySavedef));
		fram_read((uint16_t)( FramAddrB), (uint8_t*)&FramDataB, sizeof(EnergySavedef));
		if(FramDataB.Crc == FramDataA.Crc )
		{
			Sta = pdTRUE;
		}
		else
		{
			Sta = pdFALSE;
			break;
		}
	}
	taskEXIT_CRITICAL();
	if(Sta)
	{
//		snprintf(pcWriteBuffer,xWriteBufferLen,"crc1=%X  crc2=%X crc3=%X\r\n",FramDataB.Crc,FramDataA.Crc,EnergData.Crc 	);
	 strncpy(pcWriteBuffer,"Clean Energy succse\r\n",xWriteBufferLen);
	}
	else
	{
		strncpy(pcWriteBuffer,"Clean Energy error\r\n",xWriteBufferLen);
	}
	return xReturn;
}
static BaseType_t prvWriteRatio( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	sRN8302RatioPara_TypeDef  RN8302Ratio;
	char * pcParameter[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
	uint32_t xStringLength1 = 0;
	uint8_t i,xstlen;
	int rc = 0;
	for(i=0;i<6;i++)
	{
		 pcParameter[i] = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i,(BaseType_t *)&xStringLength1);
	}
	for(i=0;i<3;i++)
	{
		RN8302Ratio.IRmsConst[i] = strtol((const char *)pcParameter[i],NULL,10);
		RN8302Ratio.VRmsConst[i] = strtol((const char *)pcParameter[2+i],NULL,10);
	}

	rc = nvs_write(&fs, RN8302B_RATIO_PARA_ID, &RN8302Ratio, sizeof(sRN8302RatioPara_TypeDef));
	if(rc>0)
	{
		strncpy(pcWriteBuffer,"Write Ratio success\r\n",xWriteBufferLen);
	}
	else
	{
		strncpy(pcWriteBuffer,"Write Ratio error eg:( write ratio  IRmsConst VRmsConst)\r\n",xWriteBufferLen);
	}
	return xReturn;
}
static BaseType_t prvReadRatio( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	int rc = 0;
	sRN8302RatioPara_TypeDef  RN8302Ratio;
	rc = nvs_read(&fs, RN8302B_RATIO_PARA_ID, &RN8302Ratio, sizeof(sRN8302RatioPara_TypeDef));
	if(rc < 0)
	{
		RN8302Ratio =default_RN8302Ratio;
	}
		snprintf(pcWriteBuffer,xWriteBufferLen,"IRms1=%d IRms2=%d IRms3=%d VRms1=%d VRms2=%d VRms3=%d\r\n",RN8302Ratio.IRmsConst[0],
				RN8302Ratio.IRmsConst[1],RN8302Ratio.IRmsConst[2],RN8302Ratio.VRmsConst[0],RN8302Ratio.VRmsConst[1],RN8302Ratio.VRmsConst[2]	);
	return xReturn;
}

static BaseType_t prvWriteDeadZone( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	sRN8302DeadZonePara_TypeDef  RN8302DeadZone;
	char * pcParameter[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
	uint32_t xStringLength1 = 0;
	uint8_t i,xstlen;
	int rc = 0;
	for(i=0;i<6;i++)
	{
		 pcParameter[i] = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i,(BaseType_t *)&xStringLength1);
	}
	for(i=0;i<3;i++)
	{
		RN8302DeadZone.IDzConst[i] = strtof((const char *)pcParameter[i],NULL);
		RN8302DeadZone.VDzConst[i] = strtof((const char *)pcParameter[2+i],NULL);
	}

	rc = nvs_write(&fs, RN8302B_DEADZONE_PARA_ID, &RN8302DeadZone, sizeof(sRN8302DeadZonePara_TypeDef));
	if(rc>0)
	{
		strncpy(pcWriteBuffer,"Write deadzone success\r\n",xWriteBufferLen);
	}
	else
	{
		strncpy(pcWriteBuffer,"Write deadzone error eg:( write deadzone  IdzConst VdzConst)\r\n",xWriteBufferLen);
	}
	return xReturn;
}
static BaseType_t prvReadDeadZone( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	int rc = 0;
	sRN8302DeadZonePara_TypeDef  RN8302DeadZone;
	rc = nvs_read(&fs, RN8302B_DEADZONE_PARA_ID, &RN8302DeadZone, sizeof(sRN8302DeadZonePara_TypeDef));
	if(rc < 0)
	{
		RN8302DeadZone = default_RN8302DeadZone;
	}
	snprintf(pcWriteBuffer,xWriteBufferLen,"IDz1=%d.%d IDz2=%d.%d IDz3=%d.%d VDz1=%d.%d VDz2=%d.%d VDz3=%d.%d\r\n",
			(uint32_t)RN8302DeadZone.IDzConst[0],(uint32_t)(RN8302DeadZone.IDzConst[0]*100)%100,
			(uint32_t)RN8302DeadZone.IDzConst[1],(uint32_t)(RN8302DeadZone.IDzConst[1]*100)%100,
			(uint32_t)RN8302DeadZone.IDzConst[2],(uint32_t)(RN8302DeadZone.IDzConst[2]*100)%100,
			(uint32_t)RN8302DeadZone.VDzConst[0],(uint32_t)(RN8302DeadZone.VDzConst[0]*100)%100,
			(uint32_t)RN8302DeadZone.VDzConst[1],(uint32_t)(RN8302DeadZone.VDzConst[1]*100)%100,
			(uint32_t)RN8302DeadZone.VDzConst[2],(uint32_t)(RN8302DeadZone.VDzConst[2]*100)%100);

	return xReturn;
}

static BaseType_t prvread_Meter( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	static counter=0;
	int32_t *Rn8302DataTab =(int32_t *) usRegInputBuf;
	if(counter < READ_LIST_NUM)
	{
		snprintf(pcWriteBuffer,xWriteBufferLen,"%d=%d.%d :%s\r\n",counter,Rn8302DataTab[counter]/100,Rn8302DataTab[counter]%100,Rn8302_Const[counter].DatName);
    counter++ ;
		osDelay(10);
		xReturn = pdTRUE;
	}
	else
	{
		  xReturn = pdFALSE;
			counter=0;
	}
		return xReturn;
}
static BaseType_t rn8302ParaInit(void)
{
	BaseType_t xReturn = pdTRUE;
	int rc = 0;
	rc = nvs_read(&fs, RN8302B_RATIO_PARA_ID, &RN8302RatioPara, sizeof(sRN8302RatioPara_TypeDef));
	if(rc < 0)
	{
	  RN8302RatioPara = default_RN8302Ratio;
	  xReturn = pdFALSE;
	}

	rc = nvs_read(&fs, RN8302B_DEADZONE_PARA_ID, &RN8302DeadZonePara, sizeof(sRN8302DeadZonePara_TypeDef));
	if(rc < 0)
	{
	  RN8302DeadZonePara = default_RN8302DeadZone;
	  xReturn = pdFALSE;
	}
	return xReturn;
}
/* Exported functions --------------------------------------------------------*/
void RN8302B_init(sRN8302FirmParaFile_TypeDef *RN8302FirmPara )
{
	sRN8302FirmParaFile_TypeDef rn8302FirmPara = default_RN8302FirmPara;
	if(RN8302FirmPara == NULL)
	{
	RN8302FirmPara = &rn8302FirmPara;
	}

	sRN8302StruDataComm_TypeDef RN8302DataComm;
	RN8302DataComm.ucTemp8 = 0xe5;
	RN8302_WriteReg(&hspi2, WREN, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xA2;
	RN8302_WriteReg(&hspi2, WMSW, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xfa;
	RN8302_WriteReg(&hspi2, SOFTRST, RN8302DataComm.ucTempBuf, 1);
	HAL_Delay(10);
	RN8302DataComm.ucTemp8 = 0xe5;
	RN8302_WriteReg(&hspi2, WREN, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xA2;
	RN8302_WriteReg(&hspi2, WMSW, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTempBuf[0] = 0x77;
	RN8302DataComm.ucTempBuf[1] = 0x77;
	RN8302DataComm.ucTempBuf[2] = 0x77;
	RN8302_WriteReg(&hspi2, 0x0162, RN8302DataComm.ucTempBuf, 3);

	RN8302DataComm.wTemp16 = HFConst1_VALUE;
	RN8302_WriteReg(&hspi2, HFCONST1, RN8302DataComm.ucTempBuf, 2);

	RN8302DataComm.wTemp16 = HFConst2_VALUE;
	RN8302_WriteReg(&hspi2, HFCONST2, RN8302DataComm.ucTempBuf, 2);


	RN8302_WriteReg(&hspi2, GSUA, (uint8_t *)&RN8302FirmPara->VGain[0], 2);
	RN8302_WriteReg(&hspi2, GSUB, (uint8_t *)&RN8302FirmPara->VGain[1], 2);
	RN8302_WriteReg(&hspi2, GSUC, (uint8_t *)&RN8302FirmPara->VGain[2], 2);


	RN8302_WriteReg(&hspi2, GSIA, (uint8_t *)&RN8302FirmPara->IGain[0], 2);
	RN8302_WriteReg(&hspi2, GSIB, (uint8_t *)&RN8302FirmPara->IGain[1], 2);
	RN8302_WriteReg(&hspi2, GSIC, (uint8_t *)&RN8302FirmPara->IGain[2], 2);
	RN8302_WriteReg(&hspi2, GSIN, (uint8_t *)&RN8302FirmPara->IGain[3], 2);


	RN8302_WriteReg(&hspi2,PHSUA,(uint8_t *)&RN8302FirmPara->PHSU[0],1);
	RN8302_WriteReg(&hspi2,PHSUB,(uint8_t *)&RN8302FirmPara->PHSU[1],1);
	RN8302_WriteReg(&hspi2,PHSUC,(uint8_t *)&RN8302FirmPara->PHSU[2],1);


	RN8302DataComm.wTemp16 = D_ZXOT;
	RN8302_WriteReg(&hspi2, ZXOT, RN8302DataComm.ucTempBuf, 2);


	RN8302DataComm.wTemp16 = D_LostVoltage;
	RN8302_WriteReg(&hspi2, LostVoltT, RN8302DataComm.ucTempBuf, 2);


	RN8302DataComm.ucTempBuf[0] = 0x77;
	RN8302DataComm.ucTempBuf[1] = 0x77;
	RN8302DataComm.ucTempBuf[2] = 0x04;
	RN8302_WriteReg(&hspi2, CFCFG, RN8302DataComm.ucTempBuf, 3);


	RN8302DataComm.ucTempBuf[0] = 0x40;
	RN8302DataComm.ucTempBuf[1] = 0x00;
	RN8302DataComm.ucTempBuf[2] = 0x40;
	RN8302_WriteReg(&hspi2, 0x0161, RN8302DataComm.ucTempBuf, 3);


	RN8302DataComm.ucTemp8 = 0x10;
	RN8302_WriteReg(&hspi2, 0x0163, RN8302DataComm.ucTempBuf, 1);

	RN8302DataComm.ucTemp8 = 0xDC;
	RN8302_WriteReg(&hspi2, 0x0180, RN8302DataComm.ucTempBuf, 1);
	HAL_Delay(250);
	fram_init();
	rn8302ParaInit();
	FreeRTOS_CLIRegisterCommand(&xCalibrateCommand);
	FreeRTOS_CLIRegisterCommand(&xCleanEnergyCommand);
	FreeRTOS_CLIRegisterCommand(&xWriteRatioCommand);
	FreeRTOS_CLIRegisterCommand(&xReadRatioCommand);
	FreeRTOS_CLIRegisterCommand(&xWriteDeadZoneCommand);
	FreeRTOS_CLIRegisterCommand(&xReadDeadZoneCommand);
	FreeRTOS_CLIRegisterCommand(&xReadEleMeterCommand);
	FreeRTOS_CLIRegisterCommand(&xReadCalibrateCommand);
}

//float usRegInputBuf1[40];
void rn8302b_poll(void)
{
  uint32_t i;
  ErrorStatus ReadSta;
 // float *Rn8302DataTab = usRegInputBuf;
  int32_t *Rn8302DataTab  = ( int32_t *) usRegInputBuf;
  //float *Rn8302DataTab = usRegInputBuf1;
  float Rn8302Value;
  static uint8_t ReadCounter = 0;
  int32_t ReadData = 0;

  for(i=0; i < READ_LIST_NUM; i++)
  {
	  /*时间未到，跳过电能数据读取*/
	  if(Rn8302_Const[i].DataHandle == energy && ReadCounter > 0)
	  {
		  continue;
	  }

	  ReadSta=RN8302_ReadReg(&hspi2,(uint16_t)Rn8302_Const[i].Address,&ReadData,Rn8302_Const[i].Size);
      if(ReadSta == ERROR)
    	  return ;
      switch(Rn8302_Const[i].DataHandle)
      {
//      case 	Voltage :
//    	  Rn8302DataTab[i] = ProcVoltageData(ReadData,Rn8302_Const[i].Address);
//    	  break;
//      case 	Current :
//    	  Rn8302DataTab[i] = ProcCurrentData(ReadData,Rn8302_Const[i].Address);
//          	  break;
//      case 	Power :
//    	  Rn8302DataTab[i] = ProcPowerData(ReadData,Rn8302_Const[i].Address);
//          	  break;
//      case 	energy :
//    	  Rn8302DataTab[i] = ProcEnergyrData(ReadData,Rn8302_Const[i].Address);
//              break;
//      case 	freque :
//    	  Rn8302DataTab[i] = ProcFrequencyData(ReadData);
//              break;
//      case 	factor :
//    	  Rn8302DataTab[i] = ProcPowerFactorData(ReadData);
//              break;
//      case Sum:break;
//      default:
//    	  break;
      case 	Voltage :
    	  Rn8302Value = ProcVoltageData(ReadData,Rn8302_Const[i].Address);
       	  Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
       	  break;
         case 	Current :
          Rn8302Value = ProcCurrentData(ReadData,Rn8302_Const[i].Address);
          Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
             	  break;
         case 	Power :
          Rn8302Value = ProcPowerData(ReadData,Rn8302_Const[i].Address);
          Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
             	  break;
         case 	energy :
		  Rn8302DataTab[i] = ProcEnergyrData(ReadData,Rn8302_Const[i].Address);
        //  Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
                 break;
         case 	freque :
          Rn8302Value = ProcFrequencyData(ReadData);
          Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
                 break;
         case 	factor :
          Rn8302Value = ProcPowerFactorData(ReadData);
          Rn8302DataTab[i] =(int32_t)( Rn8302Value*100);
                 break;
         case Sum:break;
         default:
       	  break;
      }

  }
  /*总有功功率*/
  Rn8302DataTab[7] = Rn8302DataTab[13]+Rn8302DataTab[14]+Rn8302DataTab[15] ;
  /*总无功功率*/
  Rn8302DataTab[8] = Rn8302DataTab[16]+Rn8302DataTab[17]+Rn8302DataTab[18] ;
  /*总有功电能*/
  Rn8302DataTab[10] = Rn8302DataTab[25]+Rn8302DataTab[26]+Rn8302DataTab[27] ;
  /*总无功电能*/
  Rn8302DataTab[11] = Rn8302DataTab[28]+Rn8302DataTab[29]+Rn8302DataTab[30] ;
  /*总电能*/
  Rn8302DataTab[12] = Rn8302DataTab[22]+Rn8302DataTab[23]+Rn8302DataTab[24] ;

  if( ReadCounter++ >= RN8302_ENERGY_NUM)
  {
	  ReadCounter = 0;
  }
}
