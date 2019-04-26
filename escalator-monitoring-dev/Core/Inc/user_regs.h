/*
 * user_regs.h
 *
 *  Created on: 2019年1月12日
 *      Author: alien
 */

#ifndef CORE_INC_USER_REGS_H_
#define CORE_INC_USER_REGS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
#define REG_INPUT_START                 (1001)

#define REG_INPUT_RN8302B_OFFSET         0
#define REG_INPUT_RN8302B_LIST_SIZE     (37)//float
#define REG_INPUT_RN8302B_NREGS         (REG_INPUT_RN8302B_LIST_SIZE*2)

#define REG_INPUT_ANALOG_OFFSET         REG_INPUT_RN8302B_NREGS
#define REG_INPUT_ANALOG_LIST_SIZE      (32)//float
#define REG_INPUT_ANALOG_NREGS          (REG_INPUT_ANALOG_LIST_SIZE*2)

#define REG_INPUT_NREGS                 (REG_INPUT_RN8302B_NREGS + REG_INPUT_ANALOG_NREGS)

#define REG_DISCRETE_START      (1001)
#define REG_DISCRETE_LIST_SIZE  (2)//uint8_t
#define REG_DISCRETE_NREGS      (16)
/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/
extern uint16_t   usRegInputBuf[REG_INPUT_NREGS];
extern uint8_t    ucDiscreteBuf[REG_DISCRETE_LIST_SIZE];
/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_USER_REGS_H_ */
