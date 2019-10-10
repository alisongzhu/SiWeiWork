/*
 * dtu.c
 *  
 *  Created on: Nov 29 2017
 *      Author: LH
 */

/* Includes ------------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
    
/* Exported functions --------------------------------------------------------*/

/******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "api.h"
#include "usart.h"
#include "env.h"
#include "dtu.h"
#include "dma.h"
#include "cmsis_os.h"

/* Private macros ------------------------------------------------------------*/
#define UART_RX_BUFSIZE    256
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t uart_rx_buf[UART_RX_BUFSIZE] = {0};
static struct netconn *dtu_server,*dtu_client;
static uint8_t connected = 0; //only single connection 

static TaskHandle_t DTU_TCP_RX_Handle;
static TaskHandle_t DTU_COM_RX_Handle;

//static SemaphoreHandle_t sem;

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void vTaskDTU_TCP_RX(void const * argument);
static void vTaskDTU_COM_RX(void const * argument);

/* Private functions ---------------------------------------------------------*/
static void vTaskDTU_COM_RX(void const * argument)
{
//	if(xSemaphoreTake(sem,0) == pdTRUE)
//	{
//	uint16_t len = UART_RX_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);
//	netconn_write(dtu_client,uart_rx_buf,len,NETCONN_COPY);
//	}
	while(1)
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY ); 
		
		if(connected)
		{
			uint16_t len = UART_RX_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);
			netconn_write(dtu_client,uart_rx_buf,len,NETCONN_COPY);
		}
		HAL_UART_Receive_DMA(&huart6,uart_rx_buf,UART_RX_BUFSIZE);		
	}
}

static void vTaskDTU_TCP_RX(void const * argument)
{
	err_t err, accept_err;
	struct netbuf *buf;
	void *data;
	u16_t len;
	
	struct sComEnv *com = &env.ComEnv[1];
	MX_DMA_Init();
	BSP_USART6_UART_Init(com->baudrate,com->ucDataBits,(eMBParity)com->eParity);

	__HAL_UART_ENABLE_IT(&huart6,UART_IT_IDLE);
	
	HAL_UART_Receive_DMA(&huart6,uart_rx_buf,UART_RX_BUFSIZE);
	
	/* Create a new connection identifier. */
	dtu_server = netconn_new(NETCONN_TCP);
//	netconn_set_recvtimeout(dtu_server,10);
	if(dtu_server != NULL)
	{
		/* Bind connection to configured port number. */
		err = netconn_bind(dtu_server, NULL, com->port);
		
		if(err == ERR_OK)
		{
			/* Tell connection to go into listening mode. */
			netconn_listen(dtu_server);
			
			while(1)
			{
				
				if(connected == 0)
				{
					/* Grab new connection. */
					accept_err = netconn_accept(dtu_server, &dtu_client);
					
					if(accept_err == ERR_OK) 
					{
//						netconn_set_recvtimeout(dtu_client,10);
						connected = 1;
					}
				}
				
				/* Process the new connection. */
				if (connected == 1) 
				{

					while (netconn_recv(dtu_client, &buf) == ERR_OK) 
					{
						do
						{
							netbuf_data(buf,&data,&len);
							
							HAL_GPIO_WritePin(RS485_USART6_T_R_GPIO_Port,RS485_USART6_T_R_Pin, GPIO_PIN_SET); 				
							HAL_UART_Transmit(&huart6,data,len,0xff);
							HAL_GPIO_WritePin(RS485_USART6_T_R_GPIO_Port,RS485_USART6_T_R_Pin,GPIO_PIN_RESET);				
						}
						while(netbuf_next(buf) >= 0);
						netbuf_delete(buf);
						

					}
					/* Close connection and discard connection identifier. */
					netconn_close(dtu_client);
					netconn_delete(dtu_client);
					connected = 0;
				}
			}
			
		}
	}
}    
/* Exported functions --------------------------------------------------------*/
void DTU_init(void)
{
	BaseType_t status;
	
//	sem = xSemaphoreCreateBinary();
	status = xTaskCreate((TaskFunction_t)vTaskDTU_TCP_RX,"TCP_RX",200,NULL,3,&DTU_TCP_RX_Handle);
	xTaskCreate((TaskFunction_t)vTaskDTU_COM_RX,"COM_RX",200,NULL,3,&DTU_COM_RX_Handle);
	if(status != pdPASS)
	{
		//TODO: error
	}
}

void USART6_IRQHandler(void)
{
	if(__HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE)!= RESET)
	{
		HAL_UART_DMAStop(&huart6);
		__HAL_UART_CLEAR_IDLEFLAG(&huart6);
		if(__HAL_DMA_GET_COUNTER(&hdma_usart6_rx) < UART_RX_BUFSIZE)
		{
			//TODO: release semphore
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//			xSemaphoreGiveFromISR(sem,&xHigherPriorityTaskWoken);
			vTaskNotifyGiveFromISR(DTU_COM_RX_Handle,&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
		}
		else
		{
			HAL_UART_Receive_DMA(&huart6,uart_rx_buf,UART_RX_BUFSIZE);
		}
	}
	
}

//void DMA2_Stream6_IRQHandler(void)
//{
//  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

//  /* USER CODE END DMA2_Stream6_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_usart6_tx);
//  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

//  /* USER CODE END DMA2_Stream6_IRQn 1 */
//}

