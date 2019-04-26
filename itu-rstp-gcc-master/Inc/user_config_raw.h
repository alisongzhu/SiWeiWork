/*
 * user_config_raw.h
 *
 *  Created on: 2018年6月5日
 *      Author: alien
 */

#ifndef INC_USER_CONFIG_RAW_H_
#define INC_USER_CONFIG_RAW_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/err.h"
/**
* Enable debug messages
*/
#if !defined USER_CONIG_DEBUG || defined __DOXYGEN__
#define USER_CONIG_DEBUG            LWIP_DBG_OFF
#endif

/**
* USER CONFIG server port
*/
#if !defined USER_CONFIG_PORT || defined __DOXYGEN__
#define USER_CONFIG_PORT            50000
#endif
/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
err_t user_config_raw_init(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_USER_CONFIG_RAW_H_ */
