/*
 * modbus_regs.h
 *
 *  Created on: 2019年1月23日
 *      Author: pt
 */

#ifndef CORE_INC_MODBUS_REGS_H_
#define CORE_INC_MODBUS_REGS_H_


/* Includes ------------------------------------------------------------------*/
#include "port.h"
/* Private macros ------------------------------------------------------------*/
#define READ_LIST_NUM         37
#define READ_Tem_NUM          READ_LIST_NUM*2

#define REG_INPUT_START                 ( 1)
#define REG_INPUT_NREGS                  (READ_LIST_NUM*2+2)

#define REG_HOLDING_START               ( 1000 )
#define REG_HOLDING_NREGS               ( 32 )



extern  USHORT usRegInputBuf[];
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* CORE_INC_MODBUS_REGS_H_ */
