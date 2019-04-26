/*
 * flashdef.c
 *
 *  Created on: 2019年4月12日
 *      Author: pt
 */

#ifndef CORE_INC_FLASHDEF_C_
#define CORE_INC_FLASHDEF_C_


/* Includes ------------------------------------------------------------------*/
 
/* Private macros ------------------------------------------------------------*/
 /* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */
#define ADDR_FLASH_END          ((uint32_t)0x08100000) /* Base @ of Sector 11, 128 Kbyte */

#define APPLICATION_ADDRESS          (ADDR_FLASH_SECTOR_4+0x400)
#define APPLICATION_ADDRESS_BACKUP   ADDR_FLASH_SECTOR_8
#define APPLICATION_PAGE  (ADDR_FLASH_END- APPLICATION_ADDRESS_BACKUP) /* firmware max 128*4K byte */
#define FRIMWARE_ADDRESS        ADDR_FLASH_SECTOR_4
/* Private types -------------------------------------------------------------*/
struct DevLogdef{
	const char flag;
	const char version;
	const char BuildDate[20];
	const char BuildTime[20];
};
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
#endif /* CORE_INC_FLASHDEF_C_ */
