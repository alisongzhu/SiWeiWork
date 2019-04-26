/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <misc/printk.h>

//#include <board.h>
#include <device.h>
#include <string.h>
#include <nvs/nvs.h>


#define NVS_SECTOR_SIZE FLASH_ERASE_BLOCK_SIZE /* Multiple of FLASH_PAGE_SIZE */
#define NVS_SECTOR_COUNT 4 /* At least 2 sectors */
#define NVS_STORAGE_OFFSET FLASH_AREA_STORAGE_OFFSET /* Start address of the
                                                      * filesystem in flash
                                                      */
void main(void)
{
	printk("Hello World! %s\n", CONFIG_ARCH);
}
