/*
 * can_network.c
 *
 *  Created on: Dec 1, 2017
 *      Author: LH
 */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "can_network.h"
#include "flash_if.h"

/* Private macros ------------------------------------------------------------*/
#define CAN_DEVICE_STATUS_GOOD       1
#define CAN_DEVICE_STATUS_BAD        0

#define CAN_NETWORK_RX_QLEN             50
#define CAN_NETWORK_TX_QLEN             10
#define CAN_NETWORK_MVS_EVENT_QLEN      25
#define CAN_NETWORK_SP_EVENT_QLEN       10

#define CAN_FRAME_COMMAND_WRITE             1
#define CAN_FRAME_COMMAND_READ              2
#define CAN_FRAME_COMMAND_INIT              3
#define CAN_FRAME_COMMAND_BROADCAST         4
#define CAN_FRAME_COMMAND_RESPOND           5
#define CAN_FRAME_COMMAND_END               6
#define CAN_FRAME_COMMAND_HEARTBEAT         7
#define CAN_FRAME_COMMAND_EVENT             8
#define CAN_FRAME_COMMAND_FW_UPGRADE        9
#define CAN_FRAME_COMMAND_FW_READ           10
#define CAN_FRAME_COMMAND_CONFIG_UPDATE     11
#define CAN_FRAME_COMMAND_CONFIG_READ       12
#define CAN_FRAME_COMMAND_FW_CRC            13

#define POINT_TYPE_OUT8B        	1
#define POINT_TYPE_OUT32B_INT		  2
#define POINT_TYPE_OUT32B_FLOAT		3
#define POINT_TYPE_IN8B        		4
#define POINT_TYPE_IN32B_INT		  5
#define POINT_TYPE_IN32B_FLOAT		6



#define POINT_TYPE_ALL				0XFF

#define OFFSET_ADDRESS_IN_EXTID       	24
#define MASK_ADDRESS_IN_EXTID       	  (0x1f<< OFFSET_ADDRESS_IN_EXTID)
#define GET_ADDRESS_FROM_EXTID(ExtId)  	((ExtId&MASK_ADDRESS_IN_EXTID)>>OFFSET_ADDRESS_IN_EXTID)
#define SET_ADDRESS_TO_EXTID(address)   (address<<OFFSET_ADDRESS_IN_EXTID)

#define OFFSET_TYPE_IN_EXTID          	20
#define MASK_TYPE_IN_EXTID				      (0x0f<<OFFSET_TYPE_IN_EXTID)
#define GET_TYPE_FROM_EXTID(ExtId)		  ((ExtId&MASK_TYPE_IN_EXTID)>>OFFSET_TYPE_IN_EXTID)
#define SET_TYPE_TO_EXTID(type)      	  (type<<OFFSET_TYPE_IN_EXTID)

#define OFFSET_ORDER_IN_EXTID          	12
#define MASK_ORDER_IN_EXTID				      (0xff<<OFFSET_ORDER_IN_EXTID)
#define GET_ORDER_FROM_EXTID(ExtId)		  ((ExtId&MASK_ORDER_IN_EXTID)>>OFFSET_ORDER_IN_EXTID)
#define SET_ORDER_TO_EXTID(order)      	(order<<OFFSET_ORDER_IN_EXTID)

#define OFFSET_COMMAND_IN_EXTID         4
#define MASK_COMMAND_IN_EXTID			      (0xff<<OFFSET_COMMAND_IN_EXTID)
#define GET_COMMAND_FROM_EXTID(ExtId)	  ((ExtId&MASK_COMMAND_IN_EXTID)>>OFFSET_COMMAND_IN_EXTID)
#define SET_COMMAND_TO_EXTID(command)  	(command<<OFFSET_COMMAND_IN_EXTID)

#define OFFSET_RESERVE_IN_EXTID         0
#define MASK_RESERVE_IN_EXTID			      (0x0f<<OFFSET_RESERVE_IN_EXTID)
#define GET_RESERVE_FROM_EXTID(ExtId)	  ((ExtId&MASK_RESERVE_IN_EXTID)>>OFFSET_RESERVE_IN_EXTID)
#define SET_RESERVE_TO_EXTID(reserve)   (reserve&0x0f)

#define SET_EXTID(address,type,order,command,reserve)	SET_ADDRESS_TO_EXTID(address) \
															|SET_TYPE_TO_EXTID(type)  \
															|SET_ORDER_TO_EXTID(order)\
															|SET_COMMAND_TO_EXTID(command)\
															|SET_RESERVE_TO_EXTID(reserve)

#define GET_FW_ADDRESS(ExtId)  (GET_TYPE_FROM_EXTID(ExtId)<<12|GET_ORDER_FROM_EXTID(ExtId)<<4|GET_RESERVE_FROM_EXTID(ExtId) )

#define RXQ_TIME_TO_WAIT       50
#define TXQ_TIME_TO_WAIT       50

#define RX_ALIVE_TIMEOUT  (7*60*1000)
/* Private types -------------------------------------------------------------*/
//enum {
//	CAN_FRAME_CODE_INIT;
//}
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
struct sCanNetwork *CanNetwork;

/* Private function prototypes -----------------------------------------------*/
static void vTaskCanNetworkRx(void const * argument);
static void vTaskCanNetworkTx(void const * argument);
static void prvvParseMessage(CanRxMsgTypeDef RxMsg);
//static void prvvCanMemAlloc(void);
static void prvvParseInitFrame(CanRxMsgTypeDef RxMsg);
static void prvvParseDataFrame(CanRxMsgTypeDef RxMsg,uint8_t GenEvt);
static void Set_FilterConfig(uint8_t FilterNumber, uint32_t FilterID,uint32_t Mask);
static void handleTimeOut(void);

/* Private functions ---------------------------------------------------------*/
static void vTaskCanNetworkRx(void const * argument)
{
//	UBaseType_t uxNumberOfItems;

    /*## Start the Reception process and enable reception interrupt #########*/
  if(HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
//    Error_Handler();
  }
  
	BaseType_t xStatus;
	CanRxMsgTypeDef RxMsg;
	while(1)
	{
		xStatus = xQueueReceive(CanNetwork->RxQueue,&RxMsg,RXQ_TIME_TO_WAIT);
		if(xStatus == pdPASS)
		{

			prvvParseMessage(RxMsg);
		}
		handleTimeOut();
	}

}

static void vTaskCanNetworkTx(void const * argument)
{
	BaseType_t xStatus;
	vTransmitRemoteFrame(0x1f,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
	while(1)
	{
		xStatus = xQueueReceive(CanNetwork->TxQueue,&CanNetwork->TxMessage,TXQ_TIME_TO_WAIT);
		if(xStatus == pdPASS)
		{
			HAL_CAN_Transmit(&hcan1,0xff);
			if(HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0) != HAL_OK)
			{
				/* Reception Error */
//				Error_Handler();
			}
		}
	}
}

static void prvvParseMessage(CanRxMsgTypeDef RxMsg)
{
	uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId);
	if(address>env.BasicEnv.CanDevCount)
	{
//		printf("Wrong address!\r\n");
		return;
	}

	uint8_t command = GET_COMMAND_FROM_EXTID(RxMsg.ExtId);
	struct sCanDevice *dev = (struct sCanDevice*)(CanNetwork->CanDevList)[address-1];
	uint32_t now = HAL_GetTick();

	CanTxMsgTypeDef TxMsg;

	switch(command)
	{
		case CAN_FRAME_COMMAND_WRITE:
			break;

		case CAN_FRAME_COMMAND_READ:
			if(NULL == dev)
			{
				vTransmitRemoteFrame(address,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
			}
			else
			{
				dev->TimestampLast = now;
				prvvParseDataFrame(RxMsg,0);
			}
			break;

		case CAN_FRAME_COMMAND_INIT:
			prvvParseInitFrame(RxMsg);
			dev->TimestampLast = now;
			break;

		case CAN_FRAME_COMMAND_BROADCAST:
      if(NULL == dev)
			{
				vTransmitRemoteFrame(address,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
			}
			else
			{
				dev->TimestampLast = now;
				prvvParseDataFrame(RxMsg,0);
			}
			break;
		case CAN_FRAME_COMMAND_RESPOND:// todo :remove
			break;
		case CAN_FRAME_COMMAND_END:
			break;

		case CAN_FRAME_COMMAND_HEARTBEAT:
			if(NULL == dev)
			{
				vTransmitRemoteFrame(address,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
			}
			else
			{
				dev->TimestampLast = now;
			}
			break;

		case CAN_FRAME_COMMAND_EVENT:
			if(NULL == dev)
			{
				vTransmitRemoteFrame(address,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
			}
			else
			{
				dev->TimestampLast = now;
				prvvParseDataFrame(RxMsg,1);
			}

			break;
		case CAN_FRAME_COMMAND_FW_UPGRADE:
      TxMsg.IDE = CAN_ID_EXT;
      TxMsg.ExtId = RxMsg.ExtId;
      TxMsg.RTR = CAN_RTR_DATA;
      TxMsg.DLC = 4;
      memcpy(TxMsg.Data,&SystemEnv.fw_extend_size,4);

      xQueueSend(CanNetwork->TxQueue,&TxMsg,portMAX_DELAY);
		  break;
		case CAN_FRAME_COMMAND_FW_READ:

      TxMsg.IDE = CAN_ID_EXT;
      TxMsg.ExtId = RxMsg.ExtId;
      TxMsg.RTR = CAN_RTR_DATA;
      TxMsg.DLC = 8;

		  FLASH_If_Read(APPLICATION_ADDRESS_BACKUP + GET_FW_ADDRESS(RxMsg.ExtId),(uint32_t*)TxMsg.Data,2);

		  xQueueSend(CanNetwork->TxQueue,&TxMsg,portMAX_DELAY);
		  break;
		case CAN_FRAME_COMMAND_CONFIG_UPDATE:
		  break;
		case CAN_FRAME_COMMAND_CONFIG_READ:
		  break;
		case CAN_FRAME_COMMAND_FW_CRC:
      TxMsg.IDE = CAN_ID_EXT;
      TxMsg.ExtId = RxMsg.ExtId;
      TxMsg.RTR = CAN_RTR_DATA;
      TxMsg.DLC = 4;
      memcpy(TxMsg.Data,&SystemEnv.fw_extend_crc,4);

      xQueueSend(CanNetwork->TxQueue,&TxMsg,portMAX_DELAY);
		  break;
		default :
			break;
	}

}

static void prvvParseInitFrame(CanRxMsgTypeDef RxMsg)
{
	uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId) ;
	struct sCanDevice *dev = (struct sCanDevice*)(CanNetwork->CanDevList)[address-1];

	if(NULL == dev)
	{
		dev = calloc(1,sizeof(struct sCanDevice));
		struct sCanDevice **p = &(CanNetwork->CanDevList)[address-1];
		*p = dev;
		dev->data = calloc(1,sizeof(struct sDevData));

		dev->out8Count = RxMsg.Data[0];
		if(dev->out8Count)
		{
			dev->data->out8 = calloc(dev->out8Count,sizeof(uint8_t));
		}

		dev->out32intCount = RxMsg.Data[1];
		if(dev->out32intCount)
		{
			dev->data->out32int = calloc(dev->out32intCount,sizeof(int32_t));
		}

		dev->out32floatCount = RxMsg.Data[2];
		if(dev->out32floatCount)
		{
			dev->data->out32float = calloc(dev->out32floatCount,sizeof(float));
		}

		dev->in8Count = RxMsg.Data[3];
		if(dev->in8Count)
		{
			dev->data->in8 = calloc(dev->in8Count,sizeof(uint8_t));
		}

		dev->in32intCount = RxMsg.Data[4];
		if((dev->in32intCount))
		{
			dev->data->in32int = calloc(dev->in32intCount,sizeof(int32_t));
		}

		dev->in32floatCount = RxMsg.Data[5];
		if(dev->in32floatCount)
		{
			dev->data->in32float = calloc(dev->in32floatCount,sizeof(float));
		}

		vTransmitRemoteFrame(address,0x0f,0xff,CAN_FRAME_COMMAND_BROADCAST,0);
	}
}

static void prvvParseDataFrame(CanRxMsgTypeDef RxMsg,uint8_t GenEvt)
{
	uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId);
	uint8_t order = GET_ORDER_FROM_EXTID(RxMsg.ExtId);
	uint8_t type = GET_TYPE_FROM_EXTID(RxMsg.ExtId);
	struct sCanDevice * dev = (struct sCanDevice*)(CanNetwork->CanDevList)[address-1];
	switch(type)
	{
		case POINT_TYPE_OUT8B:
			break;
		case POINT_TYPE_OUT32B_INT:
			break;
		case POINT_TYPE_OUT32B_FLOAT:
			break;
		case POINT_TYPE_IN8B:
			break;
		case POINT_TYPE_IN32B_INT:
			break;
		case POINT_TYPE_IN32B_FLOAT:
			memcpy(&(dev->data->in32float[order-1]),RxMsg.Data,4);
      if(GenEvt)
      {
          struct sIn32FloatEvent mvs;
          mvs.address = address;
          mvs.order = order;
          mvs.val = dev->data->in32float[order-1];
          xQueueSend(CanNetwork->In32FloatEventQueue,&mvs,portMAX_DELAY);
      }
			break;
		default:
			break;
	}
}

static void handleTimeOut(void)
{

}

static void  Set_FilterConfig(uint8_t FilterNumber,uint32_t FilterID,uint32_t Mask)
{
	CAN_FilterConfTypeDef  sFilterConfig;
	sFilterConfig.FilterNumber=FilterNumber;
	sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
	sFilterConfig.FilterIdHigh= (((uint32_t)FilterID<<3)&0xFFFF0000)>>16; ;
	sFilterConfig.FilterIdLow= (((uint32_t)FilterID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;;
	sFilterConfig.FilterMaskIdHigh=(((uint32_t) Mask<<3)&0xFFFF0000)>>16;/**/
	sFilterConfig.FilterMaskIdLow=(((uint32_t) Mask<<3))&0x0000FFFF;
	sFilterConfig.FilterFIFOAssignment=0;
	sFilterConfig.FilterActivation=ENABLE ;
	sFilterConfig.BankNumber=14;
	HAL_CAN_ConfigFilter(&hcan1,&sFilterConfig );
}

/* Exported functions --------------------------------------------------------*/
void CanNetwork_init(void)
{
    if(env.BasicEnv.CanDevCount>0)
    {
      CanNetwork = calloc(1,sizeof(struct sCanNetwork));
  
      CanNetwork->CanDevList = calloc(env.BasicEnv.CanDevCount,sizeof(struct sCanDevice*));
  
      osThreadDef(CanNetworkRx, vTaskCanNetworkRx, osPriorityRealtime, 0, 128);
      CanNetwork->RxHandle = osThreadCreate(osThread(CanNetworkRx), NULL);
  
      osThreadDef(CanNetworkTx, vTaskCanNetworkTx, osPriorityHigh, 0, 128);
      CanNetwork->TxHandle = osThreadCreate(osThread(CanNetworkTx), NULL);
  
      CanNetwork->RxQueue = xQueueCreate(CAN_NETWORK_RX_QLEN,sizeof(CanRxMsgTypeDef));
//      vQueueAddToRegistry( CanNetwork->RxQueue, "CanNetworkRxQ" );
  
      CanNetwork->TxQueue = xQueueCreate(CAN_NETWORK_TX_QLEN,sizeof(CanTxMsgTypeDef));
//      vQueueAddToRegistry( CanNetwork->TxQueue, "CanNetworkTxQ" );
  
      CanNetwork->In32FloatEventQueue = xQueueCreate(CAN_NETWORK_MVS_EVENT_QLEN,sizeof(struct sIn32FloatEvent));
//      vQueueAddToRegistry( CanNetwork->MeasuredValueShortEventQueue, "MeasuredValueShortEventQ" );
  
      CanNetwork->In8EventQueue = xQueueCreate(CAN_NETWORK_SP_EVENT_QLEN,sizeof(struct sIn8Event));
//      vQueueAddToRegistry( CanNetwork->SinglePointEventQueue, "SinglePointEventQ" );

      hcan1.pTxMsg = &CanNetwork->TxMessage;
      hcan1.pRxMsg = &CanNetwork->RxMessage;

      MX_CAN1_Init();
      Set_FilterConfig(0,11<<24,0 );
    }
}

void vTransmitRemoteFrame(uint8_t address,uint8_t type,uint8_t order,uint8_t command,uint8_t reserve)
{
  CanTxMsgTypeDef TxMsg;
  TxMsg.IDE = CAN_ID_EXT;
  TxMsg.RTR = CAN_RTR_REMOTE;
  TxMsg.ExtId = SET_EXTID(address,type,order,command,reserve);
  TxMsg.DLC = 0;

  xQueueSend(CanNetwork->TxQueue,&TxMsg,portMAX_DELAY);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(CanNetwork->RxQueue,(void *)&CanNetwork->RxMessage,&xHigherPriorityTaskWoken);
  
	/* Receive */
	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */
//		Error_Handler();
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  MX_CAN1_Init();
  HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
}
