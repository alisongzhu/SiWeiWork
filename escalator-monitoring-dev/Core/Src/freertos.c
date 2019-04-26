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
  * Copyright (c) 2019 STMicroelectronics International N.V. 
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
#include "UARTCommandConsole.h"

#include "bsp_nvs_config.h"
#include "nvs/nvs.h"
#include "flash.h"

#include "rn8302b.h"

#include "mb.h"

#include "tim.h"

#include "user_regs.h"
#include "analog_input.h"
//#include "lwip/apps/tftp_server.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODBUS_TCP_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define PROG                    "FreeModbus"

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
/* Redirect heap RAM of freertos to CCM, total size is 64K */
#ifdef  __ICCARM__
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]@(0x10000000);
#elif defined(__CC_ARM)
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((at(0x10000000)));
#elif defined(__GNUC__)
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__((section(".ccmram")));
#endif
struct device flash_dev;

struct nvs_fs fs = {
    .sector_size = NVS_SECTOR_SIZE,
    .sector_count = NVS_SECTOR_COUNT,
    .offset = NVS_STORAGE_OFFSET,
};


static TaskHandle_t discrete_scan_handler;

static TaskHandle_t modbus_tcp_handler;
static TaskHandle_t RN8302_thread_handler;


//float  analogs[32] = {0};

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static void hc165_read(void);

static void discrete_scan_thread(void *pvParameters);

static void analog_scan_thread(void *pvParameters);

static void modbus_tcp_thread(void *pvParameters);

static void rn8302_thread(void *pvParameters);

static void hc595_output(uint8_t data);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_LWIP_Init(void);
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
  /* start freertos-cli thread */

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
__weak void StartDefaultTask(void const * argument)
{
  UNUSED(argument);
  int rc = 0;

  rc = devive_init(&flash_dev);
  assert_param(rc == 0);

  rc = nvs_init(&fs, &flash_dev);
  assert_param(rc == 0);

  /* init code for LWIP */
  LWIP_Init();

  /* USER CODE BEGIN StartDefaultTask */

   /* add command*/
  AddCommand_CLI();
  /* discrete scan thread */
  if (xTaskCreate(discrete_scan_thread, "discrete", 100, NULL, 4,
                  &discrete_scan_handler) != pdPASS)
  {
    while(1);
  }

  /* analog scan thread */
  if (xTaskCreate(analog_scan_thread, "analog", 200, NULL, 6,
                  &analog_scan_handler) != pdPASS)
  {
    while(1);
  }

  /* modbus tcp thread */
  if (xTaskCreate(modbus_tcp_thread, "modbustcp", 256, NULL, 6,
                  &modbus_tcp_handler) != pdPASS)
  {
    while(1);
  }

  /* rn8302b scan thread */
  if (xTaskCreate(rn8302_thread, "rn8302", 256, NULL, 4,
                  &RN8302_thread_handler) != pdPASS)
  {
    while(1);
  }

  tftp_init(NULL);
  vUARTCommandConsoleStart(250,5);
  /* Infinite loop */
  for(;;)
  {
    LL_IWDG_ReloadCounter(IWDG);
    osDelay(500);
    LL_GPIO_TogglePin(LED_SPECIAL_GPIO_Port, LED_SPECIAL_Pin);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

static void hc165_read(void)
{
//  taskENTER_CRITICAL();
  uint8_t tmp1 = 0 ,tmp2 = 0 ;
  uint8_t val1=0,val2=0;
  LL_GPIO_ResetOutputPin( HC165_CLK_GPIO_Port, HC165_CLK_Pin);

  LL_GPIO_SetOutputPin( HC165_SS_GPIO_Port, HC165_SS_Pin);

  uint8_t i;
  for (i=0; i<8 ; i++)
  {
    tmp1 = LL_GPIO_IsInputPinSet(HC165_SERIAL_INPUT1_GPIO_Port, HC165_SERIAL_INPUT1_Pin) ;
    tmp2 = LL_GPIO_IsInputPinSet(HC165_SERIAL_INPUT2_GPIO_Port, HC165_SERIAL_INPUT2_Pin);

    LL_GPIO_SetOutputPin( HC165_CLK_GPIO_Port, HC165_CLK_Pin);
    LL_GPIO_ResetOutputPin( HC165_CLK_GPIO_Port, HC165_CLK_Pin);

    val1 += tmp1 << i;
    val2 += tmp2 << i;
  }
  ucDiscreteBuf[0] = ~val1;
  ucDiscreteBuf[1] = ~val2;
  LL_GPIO_ResetOutputPin( HC165_SS_GPIO_Port, HC165_SS_Pin);

//  taskEXIT_CRITICAL();
}

static void discrete_scan_thread(void *pvParameters)
{
  UNUSED(pvParameters);
//  hc165_read();
  while(1)
  {
    hc165_read();
    vTaskDelay(100);
  }
}

static void analog_scan_thread(void *pvParameters)
{
  UNUSED(pvParameters);
  AnalogInputInit();
  while(1)
  {
	  AnalogScan((uint32_t *)&usRegInputBuf[REG_INPUT_ANALOG_OFFSET]);
      vTaskDelay(10);
  }
}

static void modbus_tcp_thread(void *pvParameters)
{
  UNUSED(pvParameters);
  eMBErrorCode    xStatus;
  for( ;; )
  {
      if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
      {
          fprintf( stderr, "%s: can't initialize modbus stack!\r\n", PROG );
      }
      else if( eMBEnable(  ) != MB_ENOERR )
      {
          fprintf( stderr, "%s: can't enable modbus stack!\r\n", PROG );
      }
      else
      {
          do
          {
              xStatus = eMBPoll(  );
          }
          while( xStatus == MB_ENOERR );
      }
      /* An error occurred. Maybe we can restart. */
      ( void )eMBDisable(  );
      ( void )eMBClose(  );
  }
}

static void rn8302_thread(void *pvParameters)
{
  UNUSED(pvParameters);
  int rc = 0;
  sRN8302FirmParaFile_TypeDef rn8302b_para;
  rc = nvs_read(&fs, RN8302B_PARA_ID, &rn8302b_para,
                sizeof(sRN8302FirmParaFile_TypeDef));
  (rc < 0) ? RN8302B_init(NULL) : RN8302B_init(&rn8302b_para);

//  taskENTER_CRITICAL();
//  DS18B20_Init();
//  taskEXIT_CRITICAL();
  for (;;) {
    rn8302b_poll();
//  HAL_IWDG_Refresh(&hiwdg);
    vTaskDelay(1000);
//    taskENTER_CRITICAL();
//    DS18B20_Init();
//    usRegInputBuf[READ_LIST_SIZE-1] = DS18B20_GetTemp_SkipRom();
//    taskEXIT_CRITICAL();
  }
}





/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
