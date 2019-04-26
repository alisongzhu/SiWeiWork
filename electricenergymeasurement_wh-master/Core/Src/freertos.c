/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "iwdg.h"
#include "nvs/nvs.h"
#include "flash.h"
#include "usart.h"
#include "stm32f1xx_it.h"
#include "modbus_regs.h"
#include "mb.h"
#include "bsp_nvs_config.h"
#include "rn8302b.h"
#include "UARTCommandConsole.h"
#include "FreeRTOS_CLI.h"
#include "bsp_DS18b20.h"
#include "modbus_regs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RS485_BUF_SIZE       256


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern struct device flash_dev;

extern volatile UCHAR ucRTUBuf[];
extern volatile USHORT usRcvBufferPos;


static struct rs485_para rs485;

static TaskHandle_t rs485_thread_handler;
static TaskHandle_t RN8302_thread_handler;

static const UCHAR ucSlaveID[] = { 0xAA, 0xBB, 0xCC };

static uint8_t rxbuf[RS485_BUF_SIZE] = { 0 };


/* USER CODE END Variables */
osThreadId defaultTaskHandle;
struct nvs_fs fs = {
    .sector_size = NVS_SECTOR_SIZE,
    .sector_count = NVS_SECTOR_COUNT,
    .offset = NVS_STORAGE_OFFSET,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void rs485_thread(void *pvParameters);
static void rn8302_thread(void *pvParameters);
static BaseType_t prvSetUart( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReadDevPara( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetAddr( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static const CLI_Command_Definition_t xSetUartCommand =
{
  "set_uart",
  "\r\nset_uart\r\n set_uart :BaudRate DataBits StopBits Parity  ;\r\n",
  prvSetUart,
  4
};
static const CLI_Command_Definition_t xSetDevAddrCommand =
{
  "set_addr",
  "\r\nset_addr\r\n set addr (1-250) ;\r\n",
  prvSetAddr,
  1
};
static const CLI_Command_Definition_t xReadDevParaCommand =
{
  "read_dev",
  "\r\nread_dev\r\n read dev_para;\r\n",
  prvReadDevPara,
  0
};
static const CLI_Command_Definition_t xResetCommand =
{
  "reset",
  "\r\nreset\r\n reset device ;\r\n",
  prvReset,
  0
};
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask,
                                          signed char *pcTaskName)
{
  /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
__weak void vApplicationMallocFailedHook(void)
{
  /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  vUARTCommandConsoleStart(256,3);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument) {

  /* USER CODE BEGIN StartDefaultTask */
  UNUSED(argument);
  int rc = 0;

  rc = devive_init(&flash_dev);
  assert_param(rc == 0);

  rc = nvs_init(&fs, &flash_dev);
  assert_param(rc == 0);
  if (xTaskCreate(rs485_thread, "rs485_Modbus", 256, NULL, 3,
                  &rs485_thread_handler) != pdPASS) {
    //error
  }
  sRN8302FirmParaFile_TypeDef rn8302b_para;
  if (xTaskCreate(rn8302_thread, "rn8302", 400, NULL, 5,
                  &RN8302_thread_handler) != pdPASS) {
    //error
  }

  FreeRTOS_CLIRegisterCommand(&xReadDevParaCommand);
  FreeRTOS_CLIRegisterCommand(&xSetUartCommand);
  FreeRTOS_CLIRegisterCommand(&xSetDevAddrCommand);
  FreeRTOS_CLIRegisterCommand(&xResetCommand);

  //vTaskDelete(NULL);
  /* Infinite loop */
  for (;;) {
	HAL_IWDG_Refresh(&hiwdg);
    vTaskDelay(100);
    HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void rs485_thread(void *pvParameters) {
  struct rs485_para *para = (struct rs485_para *) &rs485;
  int rc = 0;
  rc = nvs_read(&fs, RS485_PARA_ID, &rs485, sizeof(struct rs485_para));
  if (rc < 0) {
	  rs485 = defultRs485_para;
  }
  eMBInit(MB_RTU, (UCHAR) para->address, 1, para->BaudRate, para->Parity,
          para->DataBits, para->StopBits);

  eMBSetSlaveID((UCHAR) para->address, TRUE, ucSlaveID, 3);

  eMBEnable();

  HAL_UART_Receive_DMA(&huart3, rxbuf, RS485_BUF_SIZE);
  for (;;) {
    eMBPoll();
  }
}

static void rn8302_thread(void *pvParameters)
{
  UNUSED(pvParameters);
  int32_t *temp = &usRegInputBuf[READ_Tem_NUM] ;
  int rc = 0;
  sRN8302FirmParaFile_TypeDef rn8302b_para;
  rc = nvs_read(&fs, RN8302B_PARA_ID, &rn8302b_para,
                sizeof(sRN8302FirmParaFile_TypeDef));
  (rc < 0) ? RN8302B_init(NULL) : RN8302B_init(&rn8302b_para);
  taskENTER_CRITICAL();
  DS18B20_Init();
  taskEXIT_CRITICAL();

  for (;;) {
    rn8302b_poll();
	HAL_IWDG_Refresh(&hiwdg);
	vTaskDelay(2000);
    taskENTER_CRITICAL();
    DS18B20_Init();
    *temp=(int32_t) (DS18B20_GetTemp_SkipRom()*100);
    taskEXIT_CRITICAL();
  }
}
static BaseType_t prvSetUart( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	const char * pcParameter[5] = {NULL,NULL,NULL,NULL,NULL};
	uint32_t xStringLength1 = 0;
	uint8_t i,xstlen;
	int rc ;
	struct rs485_para dev_para;
	for(i=0;i<4;i++)
	{
		 pcParameter[i] = FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i+1,(BaseType_t *)&xStringLength1);
	}
	rc = nvs_read(&fs,RS485_PARA_ID,&dev_para,sizeof(struct rs485_para));
	if(rc < 0)
	{
		dev_para = defultRs485_para;
	}
//	dev_para.address  = strtol((const char *)pcParameter[0],NULL,10);
	dev_para.Parity   = strtol((const char *)pcParameter[2],NULL,10);
	dev_para.StopBits = strtol((const char *)pcParameter[3],NULL,10);
	dev_para.DataBits = strtol((const char *)pcParameter[1],NULL,10);
	dev_para.BaudRate = strtol((const char *)pcParameter[0],NULL,10);
	rc = nvs_write(&fs, RS485_PARA_ID, &dev_para, sizeof(struct rs485_para));
	if(rc>0)
	{
		strncpy(pcWriteBuffer,"Write success\r\n",xWriteBufferLen);
	}
	else
	{
		strncpy(pcWriteBuffer,"Write  error e.g:(  BaudRate DataBits  StopBits Parity )\r\n",xWriteBufferLen);
	}
	return xReturn;
}
static BaseType_t prvSetAddr( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	const char * pcParameter = NULL;
	uint32_t xStringLength1 = 0;
	uint8_t i=0,xstlen;
	int rc ;
	struct rs485_para dev_para;
	pcParameter = FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i+1,(BaseType_t *)&xStringLength1);
	rc = nvs_read(&fs,RS485_PARA_ID,&dev_para,sizeof(struct rs485_para));
	if(rc < 0)
	{
		dev_para = defultRs485_para;
	}
	dev_para.address  = strtol((const char *)pcParameter,NULL,10);
	rc = nvs_write(&fs, RS485_PARA_ID, &dev_para, sizeof(struct rs485_para));
	if(rc>0)
	{
		strncpy(pcWriteBuffer,"Set success\r\n",xWriteBufferLen);
	}
	else
	{
		strncpy(pcWriteBuffer,"set  error e.g:(  set_addr 1-250 )\r\n",xWriteBufferLen);
	}
	return xReturn;
}

static BaseType_t prvReadDevPara( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	int rc = 0;
	struct rs485_para dev_para;
	rc = nvs_read(&fs, RS485_PARA_ID, &dev_para, sizeof(struct rs485_para));
	if(rc>0)
	{
		snprintf(pcWriteBuffer,xWriteBufferLen,"address=%d  BaudRate=%d DataBits=%d Parity=%d StopBits=%d  \r\n",dev_para.address,dev_para.BaudRate,dev_para.DataBits,dev_para.Parity,
				dev_para.StopBits);
	}
	else
	{
		strncpy(pcWriteBuffer,"default Para: 254 9600/8/1/N \r\n",xWriteBufferLen);
	}
	return xReturn;
}

static BaseType_t prvReset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	BaseType_t xReturn = pdFALSE;
	__set_CONTROL(0);
	__disable_irq();
	__set_FAULTMASK(1);
	HAL_NVIC_SystemReset();
	return xReturn;
}
/**
 * @brief This function handles USART3 global interrupt.
 */
void USART3_IRQHandler(void) {
  /* USER CODE BEGIN USART3_IRQn 0 */
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  vMBPortSetWithinException( TRUE);

  if (__HAL_UART_GET_IT_SOURCE(&huart3, UART_IT_IDLE)) {
    HAL_UART_DMAStop(&huart3);
    usRcvBufferPos = RS485_BUF_SIZE - (__HAL_DMA_GET_COUNTER(huart3.hdmarx));

    memcpy(ucRTUBuf, rxbuf, usRcvBufferPos);

    HAL_UART_Receive_DMA(&huart3, rxbuf, RS485_BUF_SIZE);
    /*
     if(!usRcvBufferPos)
     {
     HAL_UART_Receive_DMA(&huart3,ucRTUBuf,RS485_BUF_SIZE);
     }*/
    xMBPortEventPost(EV_FRAME_RECEIVED);
  }
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
  __HAL_UART_CLEAR_IDLEFLAG(&huart3);

  vMBPortSetWithinException( FALSE);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  /* USER CODE END USART3_IRQn 1 */
}


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
