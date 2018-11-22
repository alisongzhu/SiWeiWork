/**
  ******************************************************************************
  * @file           : device.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_H__
#define  __DEVICE_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "stm32f1xx_hal.h"
#include "stdint.h"
/* USER CODE END Includes */
#include "RN8302.h"
/* Private define ------------------------------------------------------------*/


/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */


#define Router_Primary         0x01
#define Router_secondary       0x02
#define E32_config_NUM    10
#define LED_POWER_ON()    HAL_GPIO_WritePin(LED_STATE_GPIO_Port,LED_STATE_Pin , GPIO_PIN_SET)
#define LED_POWER_OFF()   HAL_GPIO_WritePin(LED_STATE_GPIO_Port,LED_STATE_Pin , GPIO_PIN_RESET)
typedef enum{
    WPAN_READY=0,
    WPAN_GatewayJoing ,
    WPAN_RouterJoing ,
    WPAN_Succede ,
    WPAN_RouterACK
	  
} WPANStateType;

#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus RTU frame. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
typedef enum 
{
  HEARTBEAR_FIRST = 0, 
  HEARTBEAR_AGAIN = 1,
  HEARTBEAR_OUT=2
} HeartBeatStatus;

typedef struct Router_list {
	struct Router_list        *Router_next;
	uint32_t            HeartBeatTime;
	uint8_t             Node_addr;
	HeartBeatStatus     HeartBeatflag;
  

}Router_list;
typedef struct {
	uint32_t            HeartBeatTime;
	uint8_t             Host_Addr;
	
}Router_Father;



typedef struct{
	 uint16_t  Sending_Rate;
	 uint16_t  Channel;
	 uint16_t  Tranmist_Power;
} E32_Config_DataTypeDef;


typedef struct{
    uint32_t MeterConfig_flag;
    sRN8302FirmParaFile_TypeDef RN8302FirmPara;
    
}MeterConfig_ParameterTypeDef;
typedef struct {
	uint8_t Config_flag;
	uint8_t Switch_Adrr;
	uint16_t  Switch_Scense;
 	uint16_t Router_Level;
	E32_Config_DataTypeDef  E32_config;
    MeterConfig_ParameterTypeDef MeterConfig;

}Device_Config_DataTypeDef;
typedef struct{
   
   uint32_t channel;
   uint8_t size;
   uint8_t DataFlag;
   uint8_t Datatype;
   uint8_t ucRTUBuf[MB_SER_PDU_SIZE_MAX];

}Rx_BufferHandleDef;

typedef enum{
	UART_READY=0,
	UART_TX_Busy,

} UART_HandleStateType;
typedef struct{
	Device_Config_DataTypeDef Dev_config_data;
	uint8_t                   BridgeFlag;
	WPANStateType             WPANState;
	Router_list               Router_Node_Head;
	Router_Father             Host_Node;
    uint8_t                   ChildNodeNum;
    Rx_BufferHandleDef        Rx_Buffer; 
    UART_HandleTypeDef*       WirelessUsartHandle;
    UART_HandleStateType      WirelessUsartState;
    UART_HandleTypeDef*       BusUsartHandle;
    UART_HandleStateType      BusUsartState;
    uint8_t                   MeterScanFlag;
	//uint8_t                   DMARxdataBuffe[RxSize];
	//uint8_t                   WirelessTxdataBuffe[TxSize];
	//void (*DeviceInit)();
	
}Device_handleDef;


extern Device_handleDef RTU_Device;
extern uint8_t  WireLess_NET_DATA[3] ;/*ÈëÍøÊý¾Ý*/


extern void DataStreamPrecessing(void);
extern void DeviceInit(void);
extern void meterDataDeal(void);
extern   uint8_t pvScanInternalDiscrete(void);
extern WPANStateType WPAN_Startup(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

