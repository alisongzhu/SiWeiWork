/*
 * rs485_network.h
 *
 *  Created on: 2017年12月6日
 *      Author: alien
 */

#ifndef INC_RS485_NETWORK_H_
#define INC_RS485_NETWORK_H_

#include "cmsis_os.h"
#include "usart.h"
#include "mb.h"
#include "mb_m.h"

#define RS485_PORT_COUNT		2

struct sRS485Network{
	TaskHandle_t handler;
	eMBErrorCode    eStatus;
};

void RS485Network_init(void);

#endif /* INC_RS485_NETWORK_H_ */
