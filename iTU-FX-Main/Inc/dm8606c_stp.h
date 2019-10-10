/*
 * dm8606c_stp.h
 *
 *  Created on: Nov 23 2017
 *      Author: LH
 */
 
#ifndef _DM8606C_STP_H_
#define _DM8606C_STP_H_

#include "stp.h"
#include <stdint.h>

#define DM8606C_SPECIAL_TAG     1
#define SIZEOF_SPECIAL_TAG      4
#define ETHTYPE_SPECIAL_TAG     0x8100U

#define SIZEOF_LLC_FIELD        3

#define STP_SCAN_TMR_INTERVAL            100   /* The STP port check timer interval in milliseconds. */
#define STP_STACK_TMR_INTERVAL           1000  /* The STP stack timer interval in milliseconds. */

extern struct STP_BRIDGE* bridge;

extern uint32_t stp_timestamp;

uint32_t PortIndexMapToPhy(uint32_t PortIndex);
uint32_t PortPhyMapToIndex(uint32_t PortPhy);

void stp_scan_tmr(void);
void stp_stack_tmr(void);

void stp_init(void);

#endif  /*_DM8606C_STP_H_*/
