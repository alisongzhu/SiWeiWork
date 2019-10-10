/*
 * user_common.h
 *
 *  Created on: 2019年9月27日
 *      Author: luoh
 */

#ifndef CORE_INC_USER_COMMON_H_
#define CORE_INC_USER_COMMON_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Macros --------------------------------------------------------------------*/
#define NVS_PARAM_ID_

/* Types ---------------------------------------------------------------------*/
typedef struct nvs_uart_config_{
  uint32_t BaudRate;
  uint32_t Parity;
  uint32_t DataWidth;
  uint32_t StopBits;
} nvs_uart_config_t;


/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_USER_COMMON_H_ */
