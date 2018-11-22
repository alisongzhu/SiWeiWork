/*
 * can_proto.c
 *
 *  Created on: Dec 7, 2017
 *      Author: PT
 */
 
#ifndef INC_CAN_NETWORK_H_
#include "cmsis_os.h"

#define CAN_FRAME_COMMAND_WRITE		    1
#define CAN_FRAME_COMMAND_READ		    2
#define CAN_FRAME_COMMAND_INIT        3
#define CAN_FRAME_COMMAND_BROADCAST   4
#define CAN_FRAME_COMMAND_RESPOND     5
#define CAN_FRAME_COMMAND_END         6
#define CAN_FRAME_COMMAND_HEARTBEAT   7
#define CAN_FRAME_COMMAND_EVENT       8

void CanNetwork_init(void);

extern void prvvDataTxFrame(uint8_t Addr,uint8_t Cmd, float Data);
#endif /* INC_CAN_NETWORK_H_ */
