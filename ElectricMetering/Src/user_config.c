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
#include "usart.h"
#include "user_config.h"
#include "flash.h"
#include "spi.h"
#include "spi_network.h"
#include "env.h"
/* Private macros ------------------------------------------------------------*/
 
/* Private types -------------------------------------------------------------*/
#define UARTTODMAMAXSIZE  500
#define KEY_EVE1    1        /*key events*/
#define KEY_EVE2    2        /*key events*/
#define BUTTON_TIME_PARA_CAL  5000  //校验按键时间
#define BUTTON_TIME_1         2000 //校验按键时间
#define USART_CONFIG_EVE  (1<<3)
#define USART_CMD1  (1<<3)  /*Serial read configuration parameter commands*/
#define USART_CMD2  (1<<4)  /*Serial write configuration parameter commands*/
#define CMD_Frame_Head 0xFE
#define CMD_Frame_Length  22
#define CMD_Frame_FLAG    0x11
#define CHIPIDADDR        (uint32_t*)(0X1FFFF7E8)
/* Private constants ---------------------------------------------------------*/
 enum {
  ReadParaCmd=0x00000001,
	WriteParaCmd,
  CheckParaCmd,
  DevRestCmd,
  ReadDataCmd	 
 }CommandCode;
/* Private variables ---------------------------------------------------------*/
static struct sSystemProtcolFrame CummunictionFrame;
/* Global variables ----------------------------------------------------------*/
 osThreadId taskUserConfigHandle;
 extern Config_ParameterTypeDef DevConfig_Parameter;
 uint8_t UartRx_buffer[UARTTODMAMAXSIZE ]={0};
/* Private function prototypes -----------------------------------------------*/
static void taskUserConfig(void const * argument);
static void UsartParseCmdFrame(struct sSystemProtcolFrame* RX_Frame);
static void MeterDataSend(void);
/* Private functions ---------------------------------------------------------*/
 static void UsartParseCmdFrame(struct sSystemProtcolFrame* RX_Frame)
 {
	 uint32_t cmd=RX_Frame->Command;
	 memcpy(&cmd,&RX_Frame->Command,sizeof(uint32_t ));
	switch(cmd)
	{
		case ReadParaCmd :
			memcpy(&CummunictionFrame.SourceAddress ,CHIPIDADDR ,12);
		  memcpy (&CummunictionFrame.Config_Parameter,&DevConfig_Parameter,sizeof(Config_ParameterTypeDef ));
		  CummunictionFrame.Command=ReadParaCmd;
			HAL_UART_Transmit (&huart1,(uint8_t*)&CummunictionFrame,sizeof (struct sSystemProtcolFrame),1000);
			break ;
		case WriteParaCmd :
			if(0==memcmp(&RX_Frame->destinationAddress,CHIPIDADDR,12))
			{
				memcpy(&DevConfig_Parameter  ,&RX_Frame->Config_Parameter ,sizeof(struct sSystemProtcolFrame) );	
				SaveFlashConfig(&DevConfig_Parameter );
				printf("OK");
				HAL_NVIC_SystemReset();/*配置完成复位*/
			}
			break ;
		case CheckParaCmd :
			HAL_GPIO_WritePin(GPIOB,CHECK_LED_Pin,GPIO_PIN_RESET);
			ParameterVerification(&hspi1,&DevConfig_Parameter.RN8302FirmPara[0]);
			ParameterVerification(&hspi2,&DevConfig_Parameter.RN8302FirmPara[1]);
			SaveFlashConfig(&DevConfig_Parameter );
		  printf("Check OK");
			HAL_NVIC_SystemReset();/*配置完成复位*/
			break ;
		case DevRestCmd :
			
			HAL_NVIC_SystemReset();/*配置完成复位*/
		  break;
		case ReadDataCmd:
			MeterDataSend();
			break ;
		default :break ;
		
	}
 
 }
 static void MeterDataSend(void)
 {
		uint8_t i,j;
	  for(i=0;i<2;i++)
		{
		  for(j=0;j<RN8302_READ_POINT_COUNT;j++)
			{
				 HAL_UART_Transmit (&huart1,(uint8_t*)&io[i][j],4,1000);
			}
		}
 }
 
/* Exported functions --------------------------------------------------------*/
void UserConfig_Init(void)
{
	osThreadDef(UserConfig, taskUserConfig,osPriorityRealtime, 0, 128);
	taskUserConfigHandle = osThreadCreate(osThread(UserConfig), NULL);
}
void taskUserConfig(void const * argument)
{
	 BaseType_t xResult=0;
	 const TickType_t xMaxBlockTime = pdMS_TO_TICKS(500); /* 设置最大等待时间为 500ms */
	 uint32_t ulValue=0;
	 __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE );
	 HAL_UART_Receive_DMA(&huart1,UartRx_buffer,UARTTODMAMAXSIZE );
	 printf("OK");
	 while(1)
	 {
			 
    xResult = xTaskNotifyWait(  0x00000000,
																0xFFFFFFFF,
																&ulValue,  
																xMaxBlockTime);  
		if( xResult == pdPASS )
		{
		 if((ulValue & KEY_EVE1) == KEY_EVE1)
		 {
			 HAL_GPIO_WritePin(GPIOB,CHECK_LED_Pin,GPIO_PIN_RESET);
			 ParameterVerification(&hspi1,&DevConfig_Parameter .RN8302FirmPara[0]);
			 ParameterVerification(&hspi2,&DevConfig_Parameter .RN8302FirmPara[1]);
			 DevConfig_Parameter .Config_flag =CMD_Frame_FLAG;
			 SaveFlashConfig(&DevConfig_Parameter );
			 HAL_NVIC_SystemReset();/*配置完成复位*/
//			   TX_Buffer [0]=CMD_Frame_Head;
//				 memcpy(&TX_Buffer[1],&Config_Parameter.MD_Adrr ,sizeof (Config_ParameterTypeDef)-1);
//				 HAL_UART_Transmit (&huart1,TX_Buffer,sizeof (Config_ParameterTypeDef),1000);
		 }
		 if((ulValue & KEY_EVE2) == KEY_EVE2)
		 {
				  
		 }
		 if((ulValue & USART_CONFIG_EVE) == USART_CONFIG_EVE)/*读取配置参数*/
		 {
       UsartParseCmdFrame((struct sSystemProtcolFrame *)UartRx_buffer);
		 }
//		 if((ulValue & USART_CMD1) == USART_CMD1)/*读取配置参数*/
//		 {
////			  TX_Buffer [0]=CMD_Frame_Head;
////				memcpy(&TX_Buffer[1],&Config_Parameter.MD_Adrr ,sizeof (Config_ParameterTypeDef)-1);
////				HAL_UART_Transmit (&huart1,TX_Buffer,sizeof (Config_ParameterTypeDef),1000);
//		 }
//		 if((ulValue & USART_CMD2) == USART_CMD2)
//		 {
////			   BSP_Config_Parameter(CMD_Frame_Length);/*配置参数*/
////			   TX_Buffer [0]=CMD_Frame_Head;
////				 memcpy(&TX_Buffer[1],&Config_Parameter.MD_Adrr ,sizeof (Config_ParameterTypeDef)-1);
////				 HAL_UART_Transmit (&huart1,TX_Buffer,sizeof (Config_ParameterTypeDef),1000);
//		  
//		 } 
		}
   osDelay(10);
	 }
	

}	
	
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t time_num=0;
	uint32_t time_now;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* Prevent unused argument(s) compilation warning */
   if(GPIO_Pin ==BUTTON_Pin)
	 { 
		  HAL_Delay (5);
	   if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin)==GPIO_PIN_RESET)
			 {
				time_num=HAL_GetTick();
			 }
			 HAL_Delay (5);
		 time_now=HAL_GetTick();
		
		 if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin)==GPIO_PIN_SET&&time_now-time_num>BUTTON_TIME_1
			 &&time_now-time_num<BUTTON_TIME_PARA_CAL)
		 {
		  
		   xTaskNotifyFromISR( taskUserConfigHandle, 
													KEY_EVE2,  /*key events*/
													eSetBits, 
													&xHigherPriorityTaskWoken);
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
		   
		 }
		  /*When the key time is greater than the set value, and less than three times. The key effective*/
		 if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin)==GPIO_PIN_SET
			 &&time_now-time_num>BUTTON_TIME_PARA_CAL&&time_now-time_num<3*BUTTON_TIME_PARA_CAL )
		 {
		   
		   xTaskNotifyFromISR( taskUserConfigHandle, 
													KEY_EVE1,  /*key events*/
													eSetBits, 
													&xHigherPriorityTaskWoken);
												 
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			  
		 }
		 __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_Pin);
	 }
 }

   void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{  
	 static uint32_t Rx_Count=0;
	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 /*读取接收字节数*/
	 Rx_Count=UARTTODMAMAXSIZE- __HAL_DMA_GET_COUNTER(huart->hdmarx);
	 HAL_UART_DMAStop(&huart1);
	 if(Rx_Count>=UARTTODMAMAXSIZE)
	 {
			HAL_UART_Receive_DMA(&huart1,UartRx_buffer,UARTTODMAMAXSIZE );/*restart DMA*/
	 }
	 else if(Rx_Count>0)
	 {
		  xTaskNotifyFromISR(taskUserConfigHandle, 
														 USART_CONFIG_EVE,  /*Serial read configuration parameter commands*/
														 eSetBits,  
														 &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
			Rx_Count=0;
			HAL_UART_Receive_DMA(&huart1,UartRx_buffer,UARTTODMAMAXSIZE );/*restart DMA*/
//			if(UartRx_buffer[0] ==CMD_Frame_Head)/*Command frame head*/
//		 {
//			 /*When the number of bytes received is equal to 5, and the data is the same as the read parameter command */
//			 if(Rx_Count==5&&memcmp(UartRx_buffer ,cmd_Read ,5)==0)
//			 {
//					
//					xTaskNotifyFromISR(taskUserConfigHandle, 
//														 USART_CMD1,  /*Serial read configuration parameter commands*/
//														 eSetBits,  
//														 &xHigherPriorityTaskWoken);
//												 
//				 // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//					Rx_Count=0;
//					HAL_UART_Receive_DMA(&huart1,UartRx_buffer,UARTTODMAMAXSIZE );/*restart DMA*/
//				 }
//			 
//			else  if(Rx_Count>=CMD_Frame_Length)
//			 {  
//			   xTaskNotifyFromISR(taskUserConfigHandle, 
//														USART_CMD2,  /*Serial write configuration parameter commands*/
//														eSetBits, 
//														&xHigherPriorityTaskWoken);
//													 
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//			  Rx_Count=0;
//				//HAL_UART_Receive_IT( &huart1,Rx_buffer,1);
//		   }
//		 }
		 
	 }
	 else
	 {
		 HAL_UART_Receive_DMA(&huart1,UartRx_buffer,UARTTODMAMAXSIZE );
	 }
 } 

 