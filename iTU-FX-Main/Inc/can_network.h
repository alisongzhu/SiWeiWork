/*
 * can_network.h
 *
 *  Created on: Dec 1, 2017
 *      Author: LH
 */

#ifndef INC_CAN_NETWORK_H_
#define INC_CAN_NETWORK_H_

#include "cmsis_os.h"
#include "can.h"

#define CAN_NETWORK_MAX_DEV_COUNT 16

struct sIn32FloatEvent{
    uint8_t address;
    uint8_t order;
    float val;
};

struct sIn8Event{
    uint8_t address;
    uint8_t order;
    uint8_t val;
};

struct sDevData{
	uint8_t* out8;
	uint8_t* in8;
	float* out32float;
	float* in32float;
	int32_t* out32int;
	int32_t* in32int;
};

struct sCanDevice{
	uint8_t inited;
	uint8_t connected;
	uint8_t out8Count;
	uint8_t in8Count;
	uint8_t out32floatCount;
	uint8_t in32floatCount;
	uint8_t out32intCount;
	uint8_t in32intCount;
	uint32_t TimestampLast;
	struct sDevData *data;
};

struct sCanNetwork{
	osThreadId RxHandle;
	osThreadId TxHandle;
	xQueueHandle RxQueue;
	xQueueHandle TxQueue;
	xQueueHandle In32FloatEventQueue;//MeasuredValueShort
	xQueueHandle In8EventQueue;//MeasuredValueShort
	CanTxMsgTypeDef TxMessage;
	CanRxMsgTypeDef RxMessage;
	struct sCanDevice **CanDevList;
};
extern struct sCanNetwork *CanNetwork;

void CanNetwork_init(void);


#endif /* INC_CAN_NETWORK_H_ */
