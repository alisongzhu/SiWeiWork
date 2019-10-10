/*
 * protocol_powerbus.h
 *
 *  Created on: 2019年9月28日
 *      Author: luoh
 */

#ifndef INCLUDE_PROTOCOL_POWERBUS_H_
#define INCLUDE_PROTOCOL_POWERBUS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <sys/cdefs.h>

#include "FreeRTOS.h"
#include "queue.h"
/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

typedef struct protocol_{
  void (*start_tx)(void);
  QueueHandle_t tx_queue;
}__packed protocol_t;
/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_PROTOCOL_POWERBUS_H_ */
