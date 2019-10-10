/*
 * zephyr_conf.h
 *
 *  Created on: 2018年11月27日
 *      Author: alien
 */

#ifndef CORE_INC_ZEPHYR_CONF_H_
#define CORE_INC_ZEPHYR_CONF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* Macros --------------------------------------------------------------------*/
#define CONFIG_SOC_SERIES_STM32F1X

#define CONFIG_FLASH_PAGE_LAYOUT

#define DT_FLASH_DEV_BASE_ADDRESS FLASH_R_BASE

#define CONFIG_FLASH_BASE_ADDRESS 0x8000000//FLASH_BASE

#define CONFIG_FLASH_SIZE         64

#define FLASH_WRITE_BLOCK_SIZE    2
/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_ZEPHYR_CONF_H_ */
