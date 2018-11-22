/*
 * user_cmd.h
 *
 *  Created on: 2018Äê7ÔÂ17ÈÕ
 *      Author: pt
 */

#ifndef USER_CMD_H_
#define USER_CMD_H_
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include <string.h>
#define user_log(data,size)    HAL_UART_Transmit(&huart1, (uint8_t*)data, size, 0xff);
#define USER_COMMAND1      "help"
#define USER_COMMAND2      "reset"

typedef struct Cmd_listTypeDef{
	 struct Cmd_listTypeDef* nextnode;
	 uint8_t                 CommandWord ;
	 char*                   CommandString;
	 uint8_t                  CommandStringSize;
	 void (*commandhabdle)(struct Cmd_listTypeDef*);
}Cmd_listTypeDef;

extern uint8_t command_match(char*CmdString,uint8_t size);
extern uint8_t command_add(void (*commandhabdle)(Cmd_listTypeDef*),char*CmdString);
extern void user_Command1(Cmd_listTypeDef*);
extern void user_Command2(Cmd_listTypeDef*);
extern void user_paraPrintf(Cmd_listTypeDef* Command1);
extern void user_ReadData(Cmd_listTypeDef* Command1);
extern void user_ClearEnergy(Cmd_listTypeDef* Command1);
extern void user_readTemperature(Cmd_listTypeDef* Command1);
extern void user_MeterCheck(Cmd_listTypeDef* Command1);
#endif /* USER_CMD_H_ */
