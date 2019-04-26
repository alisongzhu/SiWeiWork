/*
 * analog_input.h
 *
 *  Created on: 2019年3月31日
 *      Author: pt
 */

#ifndef CORE_INC_ANALOG_INPUT_H_
#define CORE_INC_ANALOG_INPUT_H_

#include "task.h"
/* Includes ------------------------------------------------------------------*/
 
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/
TaskHandle_t analog_scan_handler;
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void AnalogInputInit(void);
void AnalogScan(uint32_t *Rdata);
#endif /* CORE_INC_ANALOG_INPUT_H_ */
