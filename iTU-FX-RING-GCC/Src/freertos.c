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
 * Copyright (c) 2017 STMicroelectronics International N.V. 
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "tim.h"
#include "user_config_raw.h"
#include "iec104_handler.h"
#include "iwdg.h"
#include "gpio.h"
#include "env.h"
#include "can_network.h"
#include "rs485_network.h"

#include "DM8606C.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
#ifdef  __ICCARM__
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]@(0x10000000);
#elif defined(__CC_ARM)
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__((at(0x10000000))); //Redirect heap RAM of freertos to CCM, total size is 64K
#elif defined(__GNUC__)
uint8_t ucHeap[configTOTAL_HEAP_SIZE] __attribute__((section(".ccmram")));
#endif

osThreadId taskIOScanHandle;
osThreadId taskConfigHandle;
#define RAW_EVENT_QUEUE_LEN   128
xQueueHandle RawEventQueue;

__IO DiscreteInfo InternalDiscreteInfo[INTERNAL_DISCRETE_COUNT] =
  { 0 };
__IO DiscreteInfo LogicalDiscreteInfo[INTERNAL_DISCRETE_COUNT] =
  { 0 };

extern __IO uint64_t TickInMs;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void
StartDefaultTask (void const * argument);

void
MX_FREERTOS_Init (void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
static void
pvFirstScanInternalDiscrete (void);
//static void pvFirstScanLogicalDiscrete(void);

static void
pvScanInternalDiscrete (bool isFirstScan);
static void
pvScanLogicalDiscrete (bool isFirstScan);
//static void pvScanExternalDiscrete(void);
//static void handleSelectSwitchTimeout(void);

// task entry func
void
vTaskIOScan (void const * argument);
void
vTaskUserConfig (void const * argument);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */
void
vApplicationStackOverflowHook (xTaskHandle xTask, signed char *pcTaskName);
void
vApplicationMallocFailedHook (void);

/* USER CODE BEGIN 4 */
void
vApplicationStackOverflowHook (xTaskHandle xTask, signed char *pcTaskName)
{
  /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
  printf ("Task:%s stack overflowed.\r\n", pcTaskName);
  HAL_NVIC_SystemReset ();
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void
vApplicationMallocFailedHook (void)
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
//	printf("Malloc failed!\r\n");
  HAL_NVIC_SystemReset ();
}
/* USER CODE END 5 */

/* Init FreeRTOS */

void
MX_FREERTOS_Init (void)
{
  /* USER CODE BEGIN Init */
  if ( COIL_MODE_DIRECT == env.BasicEnv.CoilOutputMode)
    {
      HAL_GPIO_WritePin (SELECT_DO_GPIO_Port, SELECT_DO_Pin, GPIO_PIN_SET);
    }
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
  defaultTaskHandle = osThreadCreate (osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  if (1 == env.BasicEnv.iec104_enalbe)
    {
      osThreadDef(IOScan, vTaskIOScan, osPriorityRealtime, 0, 256);
      taskIOScanHandle = osThreadCreate (osThread(IOScan), NULL);
    }

  UserConfig_init ();
  CanNetwork_init ();
  RS485Network_init ();
  IEC104_init ();

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  RawEventQueue = xQueueCreate(RAW_EVENT_QUEUE_LEN, sizeof(DiscreteInfo));
//    vQueueAddToRegistry( RawEventQueue, "RawEventQ" ); //for getting more debug info.

  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void
StartDefaultTask (void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
//  MX_IWDG_Init ();
  uint32_t now = HAL_GetTick ();
  /* Infinite loop */
  for (;;)
    {
      HAL_IWDG_Refresh (&hiwdg);
      if (HAL_GetTick () - now > 1000)
	{
	  HAL_GPIO_TogglePin (LED_SPEC_GPIO_Port, LED_SPEC_Pin);
	  now += 1000;
	}

//			DM8606C_Supervisory();
//			DM8606C_Watchdog();
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
static void
pvFirstScanInternalDiscrete (void)
{
//    uint8_t tmp1=0,tmp2=0;
  // pull down HC165_CLK
  HAL_GPIO_WritePin ( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
  // pull down HC165
  HAL_GPIO_WritePin ( HC165_SS_GPIO_Port, HC165_SS_Pin, GPIO_PIN_RESET);
  // enable HC165 shift once
  HAL_GPIO_WritePin ( HC165_SS_GPIO_Port, HC165_SS_Pin, GPIO_PIN_SET);

  for (uint8_t i = 0; i < 8; i++)
    {
      if (1 == env.DiscreteIndividual[i].ReverseFlag)
	{
	  InternalDiscreteInfo[i].val = HAL_GPIO_ReadPin (HC165_QH1_GPIO_Port,
							  HC165_QH1_Pin);
	  InternalDiscreteInfo[i + 8].val = HAL_GPIO_ReadPin (
	      HC165_QH2_GPIO_Port, HC165_QH2_Pin);
	}
      else
	{
	  if (HAL_GPIO_ReadPin (HC165_QH1_GPIO_Port, HC165_QH1_Pin))
	    InternalDiscreteInfo[i].val = 0;
	  else
	    InternalDiscreteInfo[i].val = 1;
	  if (HAL_GPIO_ReadPin (HC165_QH2_GPIO_Port, HC165_QH2_Pin))
	    InternalDiscreteInfo[i + 8].val = 0;
	  else
	    InternalDiscreteInfo[i + 8].val = 1;
	}
    }
}

void
vTaskIOScan (void const * argument)
{
//		pvFirstScanInternalDiscrete();
  pvScanInternalDiscrete (true);
  pvScanLogicalDiscrete (true);

  MX_TIM13_Init ();    //to be modified to BSP version
  HAL_TIM_Base_Start_IT (&htim13);

  while (1)
    {
      ulTaskNotifyTake ( pdTRUE, portMAX_DELAY);
      pvScanInternalDiscrete (false);
      pvScanLogicalDiscrete (false);
    }
}

static void
pvScanInternalDiscrete (bool isFirstScan)
{
  uint8_t i, tmp1 = 0, tmp2 = 0;
//    DiscreteInfo item;
  // pull down HC165_CLK
  HAL_GPIO_WritePin ( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
  // pull down HC165
  HAL_GPIO_WritePin ( HC165_SS_GPIO_Port, HC165_SS_Pin, GPIO_PIN_RESET);
  // enable HC165 shift once
  HAL_GPIO_WritePin ( HC165_SS_GPIO_Port, HC165_SS_Pin, GPIO_PIN_SET);

  for (i = 0; i < 8; i++)
    {
      if (1 == env.DiscreteIndividual[i].ReverseFlag)
	{
	  tmp1 = HAL_GPIO_ReadPin (HC165_QH1_GPIO_Port, HC165_QH1_Pin);
	}
      else
	{
	  if (HAL_GPIO_ReadPin (HC165_QH1_GPIO_Port, HC165_QH1_Pin))
	    tmp1 = 0;
	  else
	    tmp1 = 1;
	}

      if (0 == InternalDiscreteInfo[i].flag)
	{
	  if (InternalDiscreteInfo[i].val != tmp1)
	    {
	      InternalDiscreteInfo[i].timestamp = HAL_GetTick ();
	      InternalDiscreteInfo[i].flag = 1;
	    }
	}
      else
	{
	  if ((InternalDiscreteInfo[i].timestamp
	      + env.DiscreteIndividual[i].Deadband) < HAL_GetTick ())
	    {
	      if ((InternalDiscreteInfo[i].val != tmp1))
		{
		  if (!isFirstScan)
		    {
		      InternalDiscreteInfo[i].HasEvent = 1;
		    }
		  else
		    InternalDiscreteInfo[i].HasEvent = 0;

		  InternalDiscreteInfo[i].val = tmp1;
		  InternalDiscreteInfo[i].flag = 0;

		}
	      else
		{
		  InternalDiscreteInfo[i].flag = 0;
		  InternalDiscreteInfo[i].timestamp = 0;
		}
	    }
	}

      if (1 == env.DiscreteIndividual[i + 8].ReverseFlag)
	{
	  tmp2 = HAL_GPIO_ReadPin (HC165_QH2_GPIO_Port, HC165_QH2_Pin);
	}
      else
	{
	  if (HAL_GPIO_ReadPin (HC165_QH2_GPIO_Port, HC165_QH2_Pin))
	    tmp2 = 0;
	  else
	    tmp2 = 1;
	}

      if (0 == InternalDiscreteInfo[i + 8].flag)
	{
	  if (InternalDiscreteInfo[i + 8].val != tmp2)
	    {
	      InternalDiscreteInfo[i + 8].timestamp = HAL_GetTick ();
	      InternalDiscreteInfo[i + 8].flag = 1;
	    }
	}
      else
	{
	  if ((InternalDiscreteInfo[i + 8].timestamp
	      + env.DiscreteIndividual[i + 8].Deadband) < HAL_GetTick ())
	    {
	      if (InternalDiscreteInfo[i + 8].val != tmp2)
		{
		  if (!isFirstScan)
		    {
		      InternalDiscreteInfo[i + 8].HasEvent = 1;
		    }
		  else
		    InternalDiscreteInfo[i + 8].HasEvent = 0;

		  InternalDiscreteInfo[i + 8].val = tmp2;
		  InternalDiscreteInfo[i + 8].flag = 0;

		}
	      else
		{
		  InternalDiscreteInfo[i + 8].flag = 0;
		  InternalDiscreteInfo[i + 8].timestamp = 0;
		}
	    }
	}
      HAL_GPIO_WritePin ( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin ( HC165_CLK_GPIO_Port, HC165_CLK_Pin, GPIO_PIN_RESET);
    }
}

static void
pvScanLogicalDiscrete (bool isFirstScan)
{
  DiscreteInfo item;
  struct sLogicalDiscreteIndividual *discrete;
  for (uint8_t i = 0; i < env.BasicEnv.InternalDiscreteCNT; i++)
    {

      discrete = &env.LogicalDiscreteIndividual[i];

      if (env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
	{
	  if (discrete->CombinationalMode == COMBINATIONAL_MODE_SP)
	    {
	      LogicalDiscreteInfo[i].val =
		  InternalDiscreteInfo[discrete->combination.sp].val;
	      if ((!isFirstScan)
		  && (1
		      == InternalDiscreteInfo[discrete->combination.sp].HasEvent))
		{
		  item.flag = i;    // use flag field to pass discrete order No.
		  item.val = LogicalDiscreteInfo[i].val;
		  item.timestamp = TickInMs;
		  xQueueSendToBack(RawEventQueue, &item, 0);
		  InternalDiscreteInfo[discrete->combination.sp].HasEvent = 0;
		}
	    }
//            else{
//                printf("Wrong internal discrete combination of point %d ", i);
//            }
	}

      if (env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
	{
	  if (discrete->CombinationalMode == COMBINATIONAL_MODE_SP)
	    {
	      LogicalDiscreteInfo[i].val =
		  InternalDiscreteInfo[discrete->combination.sp].val + 1;
	      if ((!isFirstScan)
		  && (1
		      == InternalDiscreteInfo[discrete->combination.sp].HasEvent))
		{
		  item.flag = i;    // use flag field to pass discrete order No.
		  item.val = LogicalDiscreteInfo[i].val;
		  item.timestamp = TickInMs;
		  xQueueSendToBack(RawEventQueue, &item, 0);
		  InternalDiscreteInfo[discrete->combination.sp].HasEvent = 0;
		}
	    }
	  else
	    {
	      LogicalDiscreteInfo[i].val =
		  InternalDiscreteInfo[discrete->combination.dp.off].val
		      + (InternalDiscreteInfo[discrete->combination.dp.on].val
			  << 1);
	      if ((!isFirstScan)
		  && (InternalDiscreteInfo[discrete->combination.dp.off].HasEvent
		      + InternalDiscreteInfo[discrete->combination.dp.on].HasEvent)
		      > 0)
		{
		  item.flag = i;
		  item.val = LogicalDiscreteInfo[i].val;
		  item.timestamp = TickInMs;
		  xQueueSendToBack(RawEventQueue, &item, 0);
		  InternalDiscreteInfo[discrete->combination.dp.off].HasEvent =
		      0;
		  InternalDiscreteInfo[discrete->combination.dp.on].HasEvent =
		      0;
		}
	    }
	}
    }
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
