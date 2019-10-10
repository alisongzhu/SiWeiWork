/*
 * bsp_config.h
 *
 *  Created on: 2019年9月28日
 *      Author: luoh
 */

#ifndef CORE_INC_BSP_CONFIG_H_
#define CORE_INC_BSP_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* Macros --------------------------------------------------------------------*/
//#define MODULE_TYPE_RELAYS_6
//#define MODULE_TYPE_RELAYS_12

#define NVS_SECTOR_SIZE   FLASH_PAGE_SIZE /* Multiple of FLASH_PAGE_SIZE */
#define NVS_SECTOR_COUNT  3 /* At least 2 sectors */
#define NVS_STORAGE_OFFSET (60*FLASH_PAGE_SIZE)/* Start address of the
                  * filesystem in flash
                  */
#define NVS_PARAM_MODULE_BASIC_ID         (1)
#define NVS_PARAM_MODULE_AREA_ID          (2)
#define NVS_PARAM_MODULE_SCENE_START_ID          (100)
#define NVS_PARAM_GPIO_ID_OFFSET          (10)



/* rn8302b nvs id*/
#define NVS_PARAM_RN8302B_ID_OFFSET       (200)

/* relays nvs id*/
#define NVS_PARAM_RELAYS_ID_OFFSET
#define NVS_PARAM_RELAYS_DETECTION_ENABLE_ID
#define NVS_PARAM_RELAYS_FAS_ACTION_ID

#define NVS_PARAM_RELAY_SWITCH_TIMES_ID   (1)
/*
SWITCH_TIMES_ID

DETECTION_ENABLE_ID
DETECTION_CURRENT_ID

LOAD_FEEDBACK_ENABLE_ID



DIGEST_INFO_ID





*/
/* Types ---------------------------------------------------------------------*/



/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_BSP_CONFIG_H_ */
