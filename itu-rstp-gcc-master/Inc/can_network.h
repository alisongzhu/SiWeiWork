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

#define CAN_FRAME_COMMAND_WRITE       1
#define CAN_FRAME_COMMAND_READ        2
#define CAN_FRAME_COMMAND_INIT        3
#define CAN_FRAME_COMMAND_BROADCAST   4
#define CAN_FRAME_COMMAND_RESPOND     5
#define CAN_FRAME_COMMAND_END         6
#define CAN_FRAME_COMMAND_HEARTBEAT   7
#define CAN_FRAME_COMMAND_EVENT       8

#define CAN_FRAME_COMMAND_FW_UPGRADE       9
#define CAN_FRAME_COMMAND_FW_READ          10
#define CAN_FRAME_COMMAND_CONFIG_UPDATE    11
#define CAN_FRAME_COMMAND_CONFIG_READ      12
#define CAN_FRAME_COMMAND_FW_CRC           13

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
	uint8_t initialise;
	uint8_t connected;
	uint8_t out8Count;//output 8bit point,intended for use of digital output.
	uint8_t in8Count;//input 8bit point,intended for use of digital input.
	uint8_t out32floatCount;//output 32bit float point
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
void vTransmitRemoteFrame(uint8_t address,uint8_t type,uint8_t order,uint8_t command,uint8_t reserve);

#endif /* INC_CAN_NETWORK_H_ */
