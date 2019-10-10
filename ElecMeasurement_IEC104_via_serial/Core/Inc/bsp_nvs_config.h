/*
 * bsp_nvs_config.h
 *
 *  Created on: 2018年12月5日
 *      Author: alien
 */

#ifndef CORE_INC_BSP_NVS_CONFIG_H_
#define CORE_INC_BSP_NVS_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
#define NVS_SECTOR_SIZE   FLASH_PAGE_SIZE /* Multiple of FLASH_PAGE_SIZE */
#define NVS_SECTOR_COUNT  2 /* At least 2 sectors */
#define NVS_STORAGE_OFFSET (60*FLASH_PAGE_SIZE)/* Start address of the
                  * filesystem in flash
                  */

#define ADDRESS_ID            1
#define RS485_PARA_ID         2
#define RN8302B_PARA_ID       3

/* Types ---------------------------------------------------------------------*/
struct rs485_para{
  uint8_t address;
  uint8_t Parity;
  uint8_t StopBits;
  uint8_t DataBits;
  uint32_t BaudRate;
};


/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_BSP_NVS_CONFIG_H_ */
