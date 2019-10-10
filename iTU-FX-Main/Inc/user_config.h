/*
 * user_config.h
 *
 *  Created on: Mar 20 2017
 *      Author: LH
 */
 
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#include <stdint.h>

void UserConfig_init(void);
//void vTaskUserConfig(void const * argument);
//void protocol_init(void);
//void protocol_run(void);

void ExecuteInternalCoilOutputOFF(uint8_t CoilNo);
void ExecuteInternalCoilOutputON(uint8_t CoilNo);


#endif  /*_USER_CONFIG_H_*/
