/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
 
#include "cmsis_os.h"
#include "can.h"
#include "can_network.h"
#include "env.h"
#include "spi_network.h"
/* Private macros ------------------------------------------------------------*/
#define CAN_NETWORK_RX_QLEN   100
#define CAN_NETWORK_TX_QLEN   10

#define  POINT_TYPE_OUT8B_DataSize          0  
#define  POINT_TYPE_OUT32B_INT_Datasize     0
#define  POINT_TYPE_OUT32B_FLOAT_Datasize   0
#define  POINT_TYPE_IN8B_Datasize           0
#define  POINT_TYPE_IN32B_INT_Datasize      0
#define  POINT_TYPE_IN32B_FLOAT_Datasize    RN8302_READ_POINT_COUNT*2

#define CAN_FRAME_COMMAND_WRITE		  1
#define CAN_FRAME_COMMAND_READ		  2
#define CAN_FRAME_COMMAND_INIT        3
#define CAN_FRAME_COMMAND_BROADCAST   4
#define CAN_FRAME_COMMAND_RESPOND     5
#define CAN_FRAME_COMMAND_END         6
#define CAN_FRAME_COMMAND_HEARTBEAT   7
#define CAN_FRAME_COMMAND_EVENT       8


#define POINT_TYPE_OUT8B        	1
#define POINT_TYPE_OUT32B_INT		  2
#define POINT_TYPE_OUT32B_FLOAT		3
#define POINT_TYPE_IN8B        		4
#define POINT_TYPE_IN32B_INT		  5
#define POINT_TYPE_IN32B_FLOAT		6

#define POINT_TYPE_ALL				0X0F

#define OFFSET_ADDRESS_IN_EXTID       	24
#define MASK_ADDRESS_IN_EXTID       	(0x1f<< OFFSET_ADDRESS_IN_EXTID)
#define GET_ADDRESS_FROM_EXTID(ExtId)  	((ExtId&MASK_ADDRESS_IN_EXTID)>>OFFSET_ADDRESS_IN_EXTID)
#define SET_ADDRESS_TO_EXTID(address)   (address<<OFFSET_ADDRESS_IN_EXTID)

#define OFFSET_TYPE_IN_EXTID          	20
#define MASK_TYPE_IN_EXTID				(0x0f<<OFFSET_TYPE_IN_EXTID)
#define GET_TYPE_FROM_EXTID(ExtId)		((ExtId&MASK_TYPE_IN_EXTID)>>OFFSET_TYPE_IN_EXTID)
#define SET_TYPE_TO_EXTID(type)      	(type<<OFFSET_TYPE_IN_EXTID)

#define OFFSET_ORDER_IN_EXTID          	12
#define MASK_ORDER_IN_EXTID				(0xff<<OFFSET_ORDER_IN_EXTID)
#define GET_ORDER_FROM_EXTID(ExtId)		((ExtId&MASK_ORDER_IN_EXTID)>>OFFSET_ORDER_IN_EXTID)
#define SET_ORDER_TO_EXTID(order)      	(order<<OFFSET_ORDER_IN_EXTID)

#define OFFSET_COMMAND_IN_EXTID         4
#define MASK_COMMAND_IN_EXTID			(0xff<<OFFSET_COMMAND_IN_EXTID)
#define GET_COMMAND_FROM_EXTID(ExtId)	((ExtId&MASK_COMMAND_IN_EXTID)>>OFFSET_COMMAND_IN_EXTID)
#define SET_COMMAND_TO_EXTID(command)  	(command<<OFFSET_COMMAND_IN_EXTID)

#define OFFSET_RESERVE_IN_EXTID         0
#define MASK_RESERVE_IN_EXTID			(0x0f<<OFFSET_RESERVE_IN_EXTID)
#define GET_RESERVE_FROM_EXTID(ExtId)	((ExtId&MASK_RESERVE_IN_EXTID)>>OFFSET_RESERVE_IN_EXTID)
#define SET_RESERVE_TO_EXTID(reserve)   (reserve&0x0f)

#define SET_EXTID(address,type,order,command,reserve)	SET_ADDRESS_TO_EXTID(address)|SET_TYPE_TO_EXTID(type)\
																														|SET_ORDER_TO_EXTID(order)\
																														|SET_COMMAND_TO_EXTID(command)\
																														|SET_RESERVE_TO_EXTID(reserve)

#define MailNotEmptyMax     10000
#define HeartbeatTime       10000

#define RXQ_TIMEOUT       500
#define TXQ_TIMEOUT       500 
/* Private types -------------------------------------------------------------*/
 
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static osThreadId taskCanTxHandle;
static osThreadId taskCanRxHandle;
static xQueueHandle RxQueue;
static xQueueHandle TxQueue;
static CanTxMsgTypeDef TxMessage;
static CanRxMsgTypeDef RxMessage;

/* Global variables ----------------------------------------------------------*/
 
/* Private function prototypes -----------------------------------------------*/
static void vTaskCanNetworkRx(void const * argument);
static void vTaskCanNetworkTx(void const * argument);
static void prvvParseMessage(CanRxMsgTypeDef RxMsg);
static void prvvParseInitFrame(CanRxMsgTypeDef RxMsg);
static void prvvParseDataFrame(CanRxMsgTypeDef RxMsg);
static void prvvTransmitRemoteFrame(uint8_t address,uint8_t type,uint8_t order,uint8_t command,uint8_t reserve);
static void Set_FilterConfig(uint8_t Numer,uint32_t FilterID,uint32_t Mask);
static void handleTimeOut(void);
static void prvvDealBoadcastFrame(CanRxMsgTypeDef RxMsg) ;


/* Private functions ---------------------------------------------------------*/
static void  Set_FilterConfig(uint8_t Numer,uint32_t FilterID,uint32_t Mask)
{
	CAN_FilterConfTypeDef  sFilterConfig;
  sFilterConfig.FilterNumber=Numer;
  sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh= (((uint32_t)FilterID<<3)&0xFFFF0000)>>16; ;
  sFilterConfig.FilterIdLow= (((uint32_t)FilterID<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;;
  sFilterConfig .FilterMaskIdHigh=(((uint32_t) Mask<<3)&0xFFFF0000)>>16;/**/
  sFilterConfig .FilterMaskIdLow=(((uint32_t) Mask<<3))&0x0000FFFF;
  sFilterConfig .FilterFIFOAssignment=0;
  sFilterConfig .FilterActivation=ENABLE ;
  sFilterConfig .BankNumber=14;
  HAL_CAN_ConfigFilter(&hcan,&sFilterConfig );
} 

static void vTaskCanNetworkRx(void const * argument)
{
//	UBaseType_t uxNumberOfItems;
	BaseType_t xStatus;
	CanRxMsgTypeDef RxMsg;
	while(1)
	{
		xStatus = xQueueReceive(RxQueue,&RxMsg,RXQ_TIMEOUT);
		if(xStatus == pdPASS)
		{
			prvvParseMessage(RxMsg);
		}
		handleTimeOut();
		osDelay(10);
	}

}

static void vTaskCanNetworkTx(void const * argument)
{
	BaseType_t xStatus;
	uint32_t  timeout;
	while(1)
	{
		xStatus = xQueueReceive(TxQueue,&TxMessage,TXQ_TIMEOUT);
		if(xStatus == pdPASS)
		{
			 timeout=HAL_GetTick ();
			/*Wait for the mailbox to be empty*/
			 while(((hcan.Instance->TSR&CAN_TSR_TME0) != CAN_TSR_TME0) && \
						 ((hcan.Instance->TSR&CAN_TSR_TME1) != CAN_TSR_TME1) && \
						 ((hcan.Instance->TSR&CAN_TSR_TME2) != CAN_TSR_TME2))
				{ 
					if(HAL_GetTick()-timeout  >MailNotEmptyMax)/*Mailbox Not empty wait max time*/
					{
							break;
					}
				}
				
			 if( HAL_CAN_Transmit(&hcan, 1000 )!=HAL_OK)
				{	
						HAL_CAN_Transmit(&hcan, 5000 );/*Try again once*/
				}
			 else 
				 timeout=HAL_GetTick ();
				 
				HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
		}
		if( HAL_GetTick()-timeout >HeartbeatTime)/*Send heartbeat packets */
		{
		 
				TxMessage.IDE=CAN_ID_EXT;
				TxMessage.RTR =CAN_RTR_REMOTE ;
				TxMessage.ExtId=SET_EXTID(DevConfig_Parameter.MD_Adrr,0x0f,0xff,CAN_FRAME_COMMAND_HEARTBEAT,0);
				HAL_CAN_Transmit(&hcan, 1000 ); 
				HAL_CAN_Receive_IT(&hcan, CAN_FIFO0);
				timeout=HAL_GetTick ();
		}
		osDelay(100);
	}
}

static void prvvParseMessage(CanRxMsgTypeDef RxMsg)
{
	uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId);
	if(address != DevConfig_Parameter.MD_Adrr&&address !=BROADCAST_ADDRRSS)
	{
		printf("Wrong address!\r\n");
		return;
	}
//	uint8_t type = GET_TYPE_FROM_EXTID(RxMsg.ExtId);
//	uint8_t order = GET_ORDER_FROM_EXTID(RxMsg.ExtId);
	uint8_t command = GET_COMMAND_FROM_EXTID(RxMsg.ExtId);
	uint32_t now = HAL_GetTick();
	switch(command)
	{
		case CAN_FRAME_COMMAND_WRITE:
			break;

		case CAN_FRAME_COMMAND_READ:
    
			break;

		case CAN_FRAME_COMMAND_INIT:
			prvvParseInitFrame(RxMsg);
//		dev->TimestampLast = now;
			break;

		case CAN_FRAME_COMMAND_BROADCAST:
      prvvDealBoadcastFrame( RxMsg) ;
			break;
			 
		case CAN_FRAME_COMMAND_RESPOND:// todo :remove
			break;
		case CAN_FRAME_COMMAND_END:
			break;

		case CAN_FRAME_COMMAND_HEARTBEAT:
      break;

		case CAN_FRAME_COMMAND_EVENT:
			break;

		default :
			break;
	}

}
static void prvvDealBoadcastFrame(CanRxMsgTypeDef RxMsg) 
{
	//uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId);
	//uint8_t order = GET_ORDER_FROM_EXTID(RxMsg.ExtId);
  //uint8_t type = GET_TYPE_FROM_EXTID(RxMsg.ExtId);
	uint8_t i;
	 for(i=0;i<RN8302_READ_POINT_COUNT;i++)
		{
			prvvDataTxFrame(i+1,CAN_FRAME_COMMAND_BROADCAST , io[0][i]);
			prvvDataTxFrame(i+1+RN8302_READ_POINT_COUNT,CAN_FRAME_COMMAND_BROADCAST , io[1][i]);
		}

}
static void prvvParseInitFrame(CanRxMsgTypeDef RxMsg)
{
  CanRxMsgTypeDef pTxMsg;
	pTxMsg.IDE=CAN_ID_EXT;
	pTxMsg.RTR =CAN_RTR_DATA;
	pTxMsg.ExtId=SET_EXTID(DevConfig_Parameter.MD_Adrr,0x0f,0xff,CAN_FRAME_COMMAND_INIT,0);
  
	pTxMsg.Data [0]= POINT_TYPE_OUT8B_DataSize ;
	pTxMsg.Data [1]= POINT_TYPE_OUT32B_INT_Datasize;
	pTxMsg.Data [2]= POINT_TYPE_OUT32B_FLOAT_Datasize;
	pTxMsg.Data [3]= POINT_TYPE_IN8B_Datasize;
	pTxMsg.Data [4]= POINT_TYPE_IN32B_INT_Datasize ;
	pTxMsg.Data [5]= POINT_TYPE_IN32B_FLOAT_Datasize;
	pTxMsg .DLC=6;
  xQueueSend(TxQueue, (void *)&pTxMsg  ,portMAX_DELAY );

}
static void prvvTransmitRemoteFrame(uint8_t address,uint8_t type,uint8_t order,uint8_t command,uint8_t reserve)
{
	CanTxMsgTypeDef TxMsg;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.RTR = CAN_RTR_REMOTE;
	TxMsg.ExtId = SET_EXTID(address,type,order,command,reserve);
 	xQueueSend(TxQueue,&TxMsg,portMAX_DELAY);
}

static void prvvParseDataFrame(CanRxMsgTypeDef RxMsg)
{
	uint8_t address = GET_ADDRESS_FROM_EXTID(RxMsg.ExtId);
	uint8_t order = GET_ORDER_FROM_EXTID(RxMsg.ExtId);
	uint8_t type = GET_TYPE_FROM_EXTID(RxMsg.ExtId);
	uint8_t Addr_order=0;
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
			if(order<=RN8302_READ_POINT_COUNT)
			{
				Addr_order=order-1;
				prvvDataTxFrame( order,CAN_FRAME_COMMAND_READ ,io[0][Addr_order]);
			}
			else if(order<=(RN8302_READ_POINT_COUNT*2))
			{
				Addr_order=order-RN8302_READ_POINT_COUNT-1;
        prvvDataTxFrame( order,CAN_FRAME_COMMAND_READ ,io[1][Addr_order]);
			}
			break;
		default:
			break;
	}
}

static void handleTimeOut(void)
{

}
/* Exported functions --------------------------------------------------------*/
void CanNetwork_init(void)
{
	osThreadDef(CanNetworkRx, vTaskCanNetworkRx, osPriorityRealtime, 0, 128);
	taskCanTxHandle = osThreadCreate(osThread(CanNetworkRx), NULL);
	
	osThreadDef(CanNetworkTx, vTaskCanNetworkTx, osPriorityRealtime, 0, 128);
	taskCanRxHandle = osThreadCreate(osThread(CanNetworkTx), NULL);
	
	RxQueue = xQueueCreate(CAN_NETWORK_RX_QLEN,sizeof(CanRxMsgTypeDef));
	vQueueAddToRegistry(RxQueue, "CanNetworkRxQ" );
	
	TxQueue = xQueueCreate(CAN_NETWORK_TX_QLEN,sizeof(CanTxMsgTypeDef));
	vQueueAddToRegistry(TxQueue, "CanNetworkTxQ" );
	
	
	hcan.pTxMsg = &TxMessage;
	hcan.pRxMsg = &RxMessage;
	MX_CAN_Init();
  Set_FilterConfig( 0,DevConfig_Parameter.MD_Adrr<<24,0x1f<<24);// Setting CAN filter Addr
	Set_FilterConfig( 1,CAN_FRAME_COMMAND_INIT<<4, 0xff<<4);// Setting CAN filter Cmd
  /*## Start the Reception process and enable reception interrupt #########*/
  if(HAL_CAN_Receive_IT(&hcan, CAN_FIFO0) != HAL_OK)
  {
    /* Reception Error */
    Error_Handler();
  }
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR(RxQueue,(void *)&RxMessage,&xHigherPriorityTaskWoken);
	/* Receive */
	if(HAL_CAN_Receive_IT(hcan, CAN_FIFO0) != HAL_OK)
	{
		/* Reception Error */
		//Error_Handler();
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

  void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  MX_CAN_Init();
	Set_FilterConfig( 0,DevConfig_Parameter.MD_Adrr<<OFFSET_ADDRESS_IN_EXTID,MASK_ADDRESS_IN_EXTID);// Setting CAN filter Addr
	Set_FilterConfig( 1,BROADCAST_ADDRRSS|CAN_FRAME_COMMAND_INIT<<4, MASK_ADDRESS_IN_EXTID|MASK_COMMAND_IN_EXTID	);// Setting CAN filter Cmd
	HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_ErrorCallback can be implemented in the user file
   */
}
void prvvDataTxFrame(uint8_t Addr,uint8_t Cmd, float Data)
{
	CanRxMsgTypeDef pTxMsg;
	pTxMsg.IDE=CAN_ID_EXT;
	pTxMsg.RTR =CAN_RTR_DATA;
	pTxMsg.ExtId=SET_EXTID(DevConfig_Parameter.MD_Adrr,POINT_TYPE_IN32B_FLOAT, Addr,Cmd,0);
  pTxMsg .DLC=4;
	memcpy(pTxMsg.Data ,&Data, pTxMsg.DLC);
  xQueueSend(TxQueue, (void *)&pTxMsg  ,portMAX_DELAY );
}
