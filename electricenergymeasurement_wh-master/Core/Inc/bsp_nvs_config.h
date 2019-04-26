/*
 * bsp_nvs_config.h
 *
 *  Created on: 2018骞�12鏈�5鏃�
 *      Author: alien
 */

#ifndef CORE_INC_BSP_NVS_CONFIG_H_
#define CORE_INC_BSP_NVS_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rn8302b.h"
#include "mb.h"
/* Macros --------------------------------------------------------------------*/
#define NVS_SECTOR_SIZE   FLASH_PAGE_SIZE /* Multiple of FLASH_PAGE_SIZE */
#define NVS_SECTOR_COUNT  2 /* At least 2 sectors */
#define NVS_STORAGE_OFFSET (62*FLASH_PAGE_SIZE)/* Start address of the
                  * filesystem in flash
                  */

#define ADDRESS_ID               1
#define RS485_PARA_ID            2
#define RN8302B_PARA_ID          3
#define RN8302B_RATIO_PARA_ID    4
#define RN8302B_DEADZONE_PARA_ID 5
/* Types ---------------------------------------------------------------------*/
struct rs485_para{
  uint8_t address;
  uint8_t Parity;
  uint8_t StopBits;
  uint8_t DataBits;
  uint32_t BaudRate;
};


/* Constants -----------------------------------------------------------------*/
static const struct rs485_para defultRs485_para ={
	     .address = 245,
	     .DataBits = 8,
	     .StopBits = 1,
	    .BaudRate = 9600,
	    .Parity = MB_PAR_NONE,
};
static const sRN8302RatioPara_TypeDef default_RN8302Ratio = {
 //   .LostVoltage = 0,
    .IRmsConst = {1,1,1},
    .VRmsConst = {1,1,1},
  //  .PRmsConst = 1,
};
static const sRN8302DeadZonePara_TypeDef default_RN8302DeadZone = {
    .VDzConst= {10,10,10},
    .IDzConst = {0.1,0.1,0.1},
};

static const sRN8302FirmParaFile_TypeDef default_RN8302FirmPara = {
    .PHSU = {201,202,173},
    .VGain = {5449,5644,5358},
    .IGain = {4662,4360,4086,3690 },

};
/* Global Variables ----------------------------------------------------------*/
extern  struct nvs_fs fs;
/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_BSP_NVS_CONFIG_H_ */
