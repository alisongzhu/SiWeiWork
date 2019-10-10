/*
 * task_gpio.c
 *
 *  Created on: 2019年9月25日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

#include "stm32_assert.h"

#include "task_list.h"

/* Private macros ------------------------------------------------------------*/
#define TASK_GPIO_THREAD_STACKSIZE    (400)
#define TASK_GPIO_THREAD_PRIORITY     (configMAX_PRIORITIES - 1)

#define TASK_GPIO_EXEC_QUEUE_SIZE      (5)

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_gpio_handle;


/* Global variables ----------------------------------------------------------*/
QueueHandle_t exec_queue;

/* Private function prototypes -----------------------------------------------*/
static void task_gpio_func(void * args);

/* Private functions ---------------------------------------------------------*/
static void task_gpio_func(void * args)
{
  (void)(args);

  while(1)
  {
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }

  vTaskDelete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void initialize_task_gpio(void)
{
  BaseType_t status ;

//  exec_queue = xQueueCreate(TASK_GPIO_EXEC_QUEUE_SIZE,)

  status = xTaskCreate(task_gpio_func,
                       "gpio",
                       TASK_GPIO_THREAD_STACKSIZE,
                       NULL,
                       TASK_GPIO_THREAD_PRIORITY,
                       &task_gpio_handle);

  if (status != pdPASS)
  {
      vTaskDelete(task_gpio_handle);
  }
  assert_param(task_gpio_handle != NULL); // The task is created but handle is incorrect

}

