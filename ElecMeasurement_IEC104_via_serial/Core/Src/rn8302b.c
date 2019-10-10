/*
 * rb8302b.c
 *
 *  Created on: 2018年12月10日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include <rn8302b.h>
#include <gpio.h>
#include <rn8302b_priv.h>

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
#define SPI_CS_ENABE()      HAL_GPIO_WritePin(SCSN_M_GPIO_Port,SCSN_M_Pin,GPIO_PIN_RESET);
#define SPI_CS_DASABE()     HAL_GPIO_WritePin(SCSN_M_GPIO_Port,SCSN_M_Pin,GPIO_PIN_SET);

#define  Standard_Voltage     (44000000)
#define  VIN_STA              (220)
#define  Standard_Current     (70000000)
#define  CURRENT_STA          (5)

#define  VOLTTHRESHOLDUINT    1 //(Standard_Voltage /VIN_STA  )     //  threshold unit 1V
#define  CURRETHESHOLDUINT    0.1f// (Standard_Current/(CURRENT_STA*10 ))  //  threshold unit 0.1A

#define  D_ZXOT              (((int32_t )(Standard_Current*0.05f))>>12)
#define  D_LostVoltage       (((int32_t )(Standard_Voltage*0.5f))>>12)

#define  VOLTTHRESHOLDUINT    1 //(Standard_Voltage /VIN_STA  )     //  threshold unit 1V
#define  CURRETHESHOLDUINT    0.1f// (Standard_Current/(CURRENT_STA*10 ))  //  threshold unit 0.1A

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
static int32_t RN8302B_DataTable[READ_LIST_SIZE] = {0};

static const uint8_t  RN8302B_DataTable1_AddressTable[READ_LIST_SIZE] =
                        {0x07,0x08,0x09,0x0a, /*Voltage*/
                        0x0b,0x0c,0x0d,0x0e,  /*Current*/
                        0x14,0x15,0x16,   /*Power active*/
                        0x18,0x19,0x1a,   /*Power inactive*/
                        0x20,0x21,0x22,   /*Power factor*/
                        0x17,0x1b,        /*Total Power active/inactive*/
                        0x23};            /*Total Power factor*/
static const uint8_t  RN8302B_DataTable1_LenTable[READ_LIST_SIZE] =
                  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,3,3,3,4,4,3};

/* Private variables ---------------------------------------------------------*/
//static struct rn8302b_priv rn8302b_data;
static sRN8302RatioPara_TypeDef RN8302Ratio = {
    .LostVoltage = 0,
    .IRmsConst = 1,
    .VRmsConst = 1,
    .PRmsConst = 1,
};

static sRN8302FirmParaFile_TypeDef default_RN8302FirmPara = {
    .PHSU = {0x80,0x80,0x80},
    .VGain = {4732,4732,4732},
    .IGain = {3690,3690,3690,3690 },

};

/* Global variables ----------------------------------------------------------*/

float RN8302B_DataTable_Front[READ_LIST_SIZE] = {0};


/* Private function prototypes -----------------------------------------------*/
static ErrorStatus RN8302_WriteReg(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t *pBuf, uint8_t DatLen);
static ErrorStatus RN8302_WriteDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);
static ErrorStatus RN8302_ReadReg(SPI_HandleTypeDef *hspi, uint16_t wReg, int32_t* val, uint8_t DatLen);
static ErrorStatus RN8302_ReadDataCheck(SPI_HandleTypeDef *hspi, uint16_t wReg, uint8_t*pBuf, uint8_t DatLen);

static float ProcVoltageData(int voltage);
static float ProcCurrentData(int current);
static float ProcPowerData(int power);
static float ProcPowerFactorData(int fator);
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
  for ( i = 0; i < 6; i++)
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

static float ProcVoltageData(int voltage)
{
  return (float)(((float)voltage)/VOLTAGE_CONST*RN8302Ratio.VRmsConst);
}

static float ProcCurrentData(int current)
{
  return (float)(((float)current)/CURRENT_CONST*RN8302Ratio.IRmsConst);
}

static float ProcPowerData(int power)
{
  return power*POWER_CONST;
};

static float ProcPowerFactorData(int fator)
{
  return ((float)fator)/FACTOR_CONST;
}
/* Exported functions --------------------------------------------------------*/
void RN8302B_init(sRN8302FirmParaFile_TypeDef *RN8302FirmPara )
{
  if(RN8302FirmPara == NULL)
  {
    RN8302FirmPara = &default_RN8302FirmPara;
  }

  sRN8302StruDataComm_TypeDef RN8302DataComm;
  RN8302DataComm.ucTemp8 = 0xe5; // 写使能位
  RN8302_WriteReg(&hspi1, WREN, RN8302DataComm.ucTempBuf, 1);

  RN8302DataComm.ucTemp8 = 0xA2; // 切换到EMM模式,计量模式
  RN8302_WriteReg(&hspi1, WMSW, RN8302DataComm.ucTempBuf, 1);

  RN8302DataComm.ucTemp8 = 0xfa; //软件复位
  RN8302_WriteReg(&hspi1, SOFTRST, RN8302DataComm.ucTempBuf, 1);
  HAL_Delay(10);
  RN8302DataComm.ucTemp8 = 0xe5; //// 写使能位
  RN8302_WriteReg(&hspi1, WREN, RN8302DataComm.ucTempBuf, 1);

  RN8302DataComm.ucTemp8 = 0xA2; // 切换到EMM模式,计量模式
  RN8302_WriteReg(&hspi1, WMSW, RN8302DataComm.ucTempBuf, 1);

  RN8302DataComm.ucTempBuf[0] = 0x77; // // 计量控制位对各相计量使能
  RN8302DataComm.ucTempBuf[1] = 0x77;
  RN8302DataComm.ucTempBuf[2] = 0x77;
  RN8302_WriteReg(&hspi1, 0x0162, RN8302DataComm.ucTempBuf, 3);

  //RN8302DataComm.wTemp16 = 0xd8d6;//设置电表常数 HFConst ＝ INT[P*3.6*10 6 *fosc / (32*EC*Un*Ib*2 31 )]邋EC
  RN8302DataComm.wTemp16 = HFConst1_VALUE;//设置电表常数
  RN8302_WriteReg(&hspi1, HFCONST1, RN8302DataComm.ucTempBuf, 2);

  RN8302DataComm.wTemp16 = HFConst2_VALUE;//设置电表常数 HFConst ＝ INT[P*3.6*10 6 *fosc / (32*EC*Un*Ib*2 31 )]邋EC
  RN8302_WriteReg(&hspi1, HFCONST2, RN8302DataComm.ucTempBuf, 2);

  // 电压增益初始化
  RN8302_WriteReg(&hspi1, GSUA, (uint8_t *)&RN8302FirmPara->VGain[0], 2);
  RN8302_WriteReg(&hspi1, GSUB, (uint8_t *)&RN8302FirmPara->VGain[1], 2);
  RN8302_WriteReg(&hspi1, GSUC, (uint8_t *)&RN8302FirmPara->VGain[2], 2);

  // 电流增益初始化
  RN8302_WriteReg(&hspi1, GSIA, (uint8_t *)&RN8302FirmPara->IGain[0], 2);
  RN8302_WriteReg(&hspi1, GSIB, (uint8_t *)&RN8302FirmPara->IGain[1], 2);
  RN8302_WriteReg(&hspi1, GSIC, (uint8_t *)&RN8302FirmPara->IGain[2], 2);
  RN8302_WriteReg(&hspi1, GSIN, (uint8_t *)&RN8302FirmPara->IGain[3], 2);

  // 通道相位分段参数设置
  RN8302_WriteReg(&hspi1,PHSUA,(uint8_t *)&RN8302FirmPara->PHSU[0],2);
  RN8302_WriteReg(&hspi1,PHSUB,(uint8_t *)&RN8302FirmPara->PHSU[1],2);
  RN8302_WriteReg(&hspi1,PHSUC,(uint8_t *)&RN8302FirmPara->PHSU[2],2);

  ///过零阈值寄存
  RN8302DataComm.wTemp16 = D_ZXOT;
  RN8302_WriteReg(&hspi1, ZXOT, RN8302DataComm.ucTempBuf, 2);

  //失压阀值寄存器
  RN8302DataComm.wTemp16 = D_LostVoltage;
  RN8302_WriteReg(&hspi1, LostVoltT, RN8302DataComm.ucTempBuf, 2);

  //配置CF脉冲输出
  RN8302DataComm.ucTempBuf[0] = 0x77;
  RN8302DataComm.ucTempBuf[1] = 0x77;
  RN8302DataComm.ucTempBuf[2] = 0x04;
  RN8302_WriteReg(&hspi1, CFCFG, RN8302DataComm.ucTempBuf, 3);

  //配置 计量控制位 默认值 400000  计量单元配置寄存器 EMUCFG 用于配置计量模块
  RN8302DataComm.ucTempBuf[0] = 0x40;
  RN8302DataComm.ucTempBuf[1] = 0x00;
  RN8302DataComm.ucTempBuf[2] = 0x40;
  RN8302_WriteReg(&hspi1, 0x0161, RN8302DataComm.ucTempBuf, 3);


  RN8302DataComm.ucTemp8 = 0x10; //  // 清空采样数据缓存区
  RN8302_WriteReg(&hspi1, 0x0163, RN8302DataComm.ucTempBuf, 1);
  //RN8302DataComm.wTemp16=HFconst1；
  //  Read_RN8302_Data();
  RN8302DataComm.ucTemp8 = 0xDC; // 写保护
  RN8302_WriteReg(&hspi1, 0x0180, RN8302DataComm.ucTempBuf, 1);
  HAL_Delay(250);
}


void rn8302b_poll(void)
{
  uint32_t i;
  for(i=0; i< READ_LIST_SIZE; i++)
  {
    RN8302_ReadReg(&hspi1,(uint16_t)RN8302B_DataTable1_AddressTable[i],&RN8302B_DataTable[i],RN8302B_DataTable1_LenTable[i]);
  }

  for(i = 0; i < 4; i++)
  {
    RN8302B_DataTable_Front[i] = ProcVoltageData(RN8302B_DataTable[i]);
  }

  for(i = 4; i < 8; i++)
  {
    RN8302B_DataTable_Front[i] = ProcCurrentData(RN8302B_DataTable[i]);
  }

  for(i = 8; i < 14; i++)
  {
    RN8302B_DataTable_Front[i] = ProcPowerData(RN8302B_DataTable[i]);
  }

  for(i=14;i<17;i++)
  {
    RN8302B_DataTable_Front[i] = ProcPowerFactorData(RN8302B_DataTable[i]);
  }

  RN8302B_DataTable_Front[17] = ProcPowerData(RN8302B_DataTable[17]);
  RN8302B_DataTable_Front[18] = ProcPowerData(RN8302B_DataTable[18]);

  RN8302B_DataTable_Front[19] = ProcPowerFactorData(RN8302B_DataTable[19]);
}
