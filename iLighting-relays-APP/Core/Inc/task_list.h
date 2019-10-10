/*
 * task_gpio.h
 *
 *  Created on: 2019年9月25日
 *      Author: luoh
 */

#ifndef CORE_INC_TASK_LIST_H_
#define CORE_INC_TASK_LIST_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
void initialize_task_gpio(void);

void initialize_task_rn8302b_poll(void);

void initialize_task_powerbus(void);

void initialize_task_usb_comm(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_TASK_LIST_H_ */
