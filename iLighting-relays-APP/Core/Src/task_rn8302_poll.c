/*
 * task_rn8302_poll.c
 *
 *  Created on: 2019年9月26日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

#include "stm32_assert.h"

#include "task_list.h"

#include "main.h"
#include "rn8302b/include/rn8302b.h"

#include "bsp.h"

/* Private macros ------------------------------------------------------------*/
#define TASK_RN8302_POLL_THREAD_STACKSIZE    (400)
#define TASK_RN8302_POLL_THREAD_PRIORITY     (configMAX_PRIORITIES - 1)


/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_rn8302b_poll_handle;

/* Global variables ----------------------------------------------------------*/

struct rn8302b_dev chip0;
struct rn8302b_dev chip1;
struct rn8302b_dev chip2;
struct rn8302b_dev chip3;

/* Private function prototypes -----------------------------------------------*/
static void task_rn8302b_poll(void * args);

/* Private functions ---------------------------------------------------------*/
static void task_rn8302b_poll(void * args)
{
  (void)args;

  while(1)
  {
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void initialize_task_rn8302b_poll(void)
{
  chip0.nvs.fs = &fs;
  chip0.nvs.id_offset = NVS_PARAM_RN8302B_ID_OFFSET;
  chip0.spi.handle = SPI1;
  chip0.spi.port = SPI1_CS1_GPIO_Port;
  chip0.spi.PinMask = SPI1_CS1_Pin;
  chip0.spi.lock = xSemaphoreCreateMutex();
  assert_param(chip0.spi.lock != NULL);
//  chip0.table = NULL;
//  chip0.ratio = NULL;
//  chip0.deadband = NULL;


  BaseType_t status ;

  status = xTaskCreate(task_rn8302b_poll,
                       "rn8302b_poll",
                       TASK_RN8302_POLL_THREAD_STACKSIZE,
                       NULL,
                       TASK_RN8302_POLL_THREAD_PRIORITY,
                       &task_rn8302b_poll_handle);

  if (status != pdPASS)
  {
      vTaskDelete(task_rn8302b_poll_handle);
  }
  assert_param(task_rn8302b_poll_handle != NULL); // The task is created but handle is incorrect
}
