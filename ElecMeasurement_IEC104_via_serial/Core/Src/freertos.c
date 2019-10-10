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
#include "string.h"

#include <nvs/nvs.h>
#include <flash.h>
#include <ring_buffer.h>

#include <usart.h>
#include <stm32f1xx_it.h>

#include <rn8302b.h>
#include <bsp_nvs_config.h>
#include <iec104_via_rs485_handler.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RS485_BUF_SIZE       (256)

#define RS485_RING_BUF_BYTES       (1024)

RING_BUF_DECLARE(rs485_rb,RS485_RING_BUF_BYTES);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern struct device flash_dev;

SemaphoreHandle_t uart_sem;

static struct nvs_fs fs = {
  .sector_size = NVS_SECTOR_SIZE,
  .sector_count = NVS_SECTOR_COUNT,
  .offset = NVS_STORAGE_OFFSET,
};



static TaskHandle_t rs485_thread_handler;
static TaskHandle_t RN8302_thread_handler;

static uint8_t rxCount = 0;
static uint8_t rxbuf[RS485_BUF_SIZE] = {0};

//static u8_t _ring_buffer_data_rs485_rb[RS485_RING_BUF_BYTES];
//
//struct ring_buf rs485_rb = {
//  .size = (1024),
//  .buf = { .buf8 = _ring_buffer_data_rs485_rb}
//};

struct rs485_para rs485;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void rs485_thread(void *pvParameters);
static void rn8302_thread(void *pvParameters);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
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
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  UNUSED(argument);
  int rc = 0 ;

  rc = devive_init(&flash_dev);
  assert_param(rc == 0);

  rc = nvs_init(&fs, &flash_dev);
  assert_param(rc == 0);

  rc = nvs_read(&fs, RS485_PARA_ID, &rs485, sizeof( struct rs485_para ));
  if(rc < 0 )
  {
    rs485.address = 1;
    rs485.DataBits = 8;
    rs485.StopBits = 1;
    rs485.BaudRate = 9600;
//    rs485.Parity = MB_PAR_NONE;
  }

  if(xTaskCreate(rs485_thread,"rs485_IEC104",512,&rs485,3,&rs485_thread_handler) != pdPASS)
  {
    //error
  }

  sRN8302FirmParaFile_TypeDef rn8302b_para;

  rc = nvs_read(&fs, RN8302B_PARA_ID, &rn8302b_para, sizeof(sRN8302FirmParaFile_TypeDef));
  (rc < 0 )? RN8302B_init(NULL): RN8302B_init(&rn8302b_para);

  if(xTaskCreate(rn8302_thread,"rn8302",256,NULL,5,&RN8302_thread_handler) != pdPASS)
  {
   //error
  }

  vTaskDelete(defaultTaskHandle);
  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void rs485_thread(void *pvParameters)
{
//  struct rs485_para *para = (struct rs485_para *)pvParameters;
  UNUSED(pvParameters);

  uart_sem = xSemaphoreCreateBinary();

  ring_buf_init(&rs485_rb,RS485_RING_BUF_BYTES,rs485_rb.buf.buf8);

  MX_USART3_UART_Init();

  HAL_UART_Receive_DMA(&huart3,rxbuf,RS485_BUF_SIZE);

  vTaskIEC104(NULL);
}

static void rn8302_thread(void *pvParameters)
{
  UNUSED(pvParameters);
  for(;;)
  {
    rn8302b_poll();
    vTaskDelay(1000);
  }
}
/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if(__HAL_UART_GET_IT_SOURCE(&huart3,UART_IT_IDLE))
  {
    HAL_UART_DMAStop(&huart3);
    rxCount = RS485_BUF_SIZE - (__HAL_DMA_GET_COUNTER(huart3.hdmarx));

    ring_buf_put(&rs485_rb, rxbuf, rxCount);

    HAL_UART_Receive_DMA(&huart3,rxbuf,RS485_BUF_SIZE);

    xSemaphoreGiveFromISR(uart_sem,xHigherPriorityTaskWoken);
  }
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
  __HAL_UART_CLEAR_IDLEFLAG(&huart3);

//  vMBPortSetWithinException( FALSE );

  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  /* USER CODE END USART3_IRQn 1 */
}




/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
