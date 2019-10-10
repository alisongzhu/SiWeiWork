/*
 * bsp.h
 *
 *  Created on: 2019年9月27日
 *      Author: luoh
 */

#ifndef CORE_INC_BSP_H_
#define CORE_INC_BSP_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nvs/include/nvs.h"

#include "bsp_config.h"
/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/
 typedef struct module_address_param_{
   uint8_t net;
   uint8_t id;
 }__packed module_address_param_t;
/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/
extern struct nvs_fs fs;

extern module_address_param_t address;
/* Function Prototypes -------------------------------------------------------*/
uint32_t bsp_get_uid(void);
void bsp_get_version(uint8_t *data);

void bsp_nvs_init(void);
void bsp_get_address(module_address_param_t *address);


void bsp_uart_powerbus_init(void);
void bsp_uart_usb_comm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_BSP_H_ */
