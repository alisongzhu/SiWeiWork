/*
 * task_usb_comm.c
 *
 *  Created on: 2019年9月27日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

#include "stm32_assert.h"

#include "task_list.h"

#include "main.h"


/* Private macros ------------------------------------------------------------*/
#define TASK_USB_COMM_THREAD_STACKSIZE    (400)
#define TASK_USB_COMM_THREAD_PRIORITY     (configMAX_PRIORITIES - 1)

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_usb_comm_handle;

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void task_usb_comm_func(void * args);

/* Private functions ---------------------------------------------------------*/
static void task_usb_comm_func(void * args)
{
  (void)(args);

  while(1)
  {
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void initialize_task_usb_comm(void)
{
  BaseType_t status ;

  status = xTaskCreate(task_usb_comm_func,
                       "usb_comm",
                       TASK_USB_COMM_THREAD_STACKSIZE,
                       NULL,
                       TASK_USB_COMM_THREAD_PRIORITY,
                       &task_usb_comm_handle);

  if (status != pdPASS)
  {
      vTaskDelete(task_usb_comm_handle);
  }
  assert_param(task_usb_comm_handle != NULL); // The task is created but handle is incorrect
}
