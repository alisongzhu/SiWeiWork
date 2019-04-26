/*
 * fram.h
 *
 *  Created on: 2018��11��26��
 *      Author: pt
 */

#ifndef CORE_USER_FRAM_H_
#define CORE_USER_FRAM_H_

#include <stdint.h>

#define FRAM_USE_SPI

int fram_write(uint16_t addr, uint8_t* data,uint16_t num_bytes);

int fram_read(uint16_t addr, uint8_t* data, uint16_t num_bytes);

void fram_init(void);

#endif /* CORE_USER_FRAM_H_ */
