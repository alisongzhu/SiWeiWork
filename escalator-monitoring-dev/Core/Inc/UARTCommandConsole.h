/*
 * UARTCommandConsole.h
 *
 *  Created on: 2018年12月20日
 *      Author: alien
 */

#ifndef CORE_INC_UARTCOMMANDCONSOLE_H_
#define CORE_INC_UARTCOMMANDCONSOLE_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
 /* FreeRTOS includes. */
 #include "FreeRTOS.h"
 #include "task.h"
/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/
void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );
void vOutputString( const char * const pcMessage );
/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_UARTCOMMANDCONSOLE_H_ */
