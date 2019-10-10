/*
 * bsp.c
 *
 *  Created on: 2019年9月27日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "lib/include/device.h"

//#include "bsp_config.h"
#include "bsp.h"
#include "build_defs.h"

//#include "main.h"
#include "stm32f1xx_ll_crc.h"
#include "stm32f1xx_ll_utils.h"
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

struct nvs_fs fs = {
    .sector_size = NVS_SECTOR_SIZE,
    .sector_count = NVS_SECTOR_COUNT,
    .offset = NVS_STORAGE_OFFSET,
};

module_address_param_t address;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
    
/* Exported functions --------------------------------------------------------*/
uint32_t bsp_get_uid(void)
{
  LL_CRC_ResetCRCCalculationUnit(CRC);

  LL_CRC_FeedData32(CRC,LL_GetUID_Word0());
  LL_CRC_FeedData32(CRC,LL_GetUID_Word1());
  LL_CRC_FeedData32(CRC,LL_GetUID_Word2());

  return LL_CRC_ReadData32(CRC);
}


void bsp_get_version(uint8_t *data)
{
  strncpy((char*)data,_GIT_VERSION_,_GIT_VERSION_LENGTH);
}

void bsp_nvs_init(void)
{
  int rc = 0;

  extern struct device flash_dev;
  rc = devive_init(&flash_dev);
  assert_param(rc == 0);

  rc = nvs_init(&fs, &flash_dev);
  assert_param(rc == 0);
}

void bsp_get_address(module_address_param_t *addr)
{
  int rc = 0;
  size_t len;

  len = sizeof(module_address_param_t);
  rc = nvs_read(&fs,NVS_PARAM_MODULE_BASIC_ID,addr,len);
  if(rc != len)
  {
    addr->net = 0;
    addr->id = 1;
  }
}


void bsp_uart_powerbus_init(void)
{

}

void bsp_uart_usb_comm_init(void)
{

}
