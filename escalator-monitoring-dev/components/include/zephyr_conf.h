/*
 * zephyr_conf.h
 *
 *  Created on: 2019年1月8日
 *      Author: alien
 */

#ifndef CORE_INC_ZEPHYR_CONF_H_
#define CORE_INC_ZEPHYR_CONF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx_hal.h"
/* Macros --------------------------------------------------------------------*/
#define CONFIG_SOC_SERIES_STM32F4X

#define CONFIG_FLASH_PAGE_LAYOUT

#define DT_FLASH_DEV_BASE_ADDRESS FLASH_R_BASE

#define CONFIG_FLASH_BASE_ADDRESS FLASH_BASE

#define CONFIG_FLASH_SIZE         1024

#define FLASH_WRITE_BLOCK_SIZE   4

//#define FLASH_SECTOR_TOTAL        12
/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_ZEPHYR_CONF_H_ */
