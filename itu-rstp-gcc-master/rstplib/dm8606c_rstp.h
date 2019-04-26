/*
 * dm8606c_rstp.h
 *
 *  Created on: 2018年5月18日
 *      Author: alien
 */

#ifndef DM8606C_RSTP_H_
#define DM8606C_RSTP_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
/* The STP port check timer interval in milliseconds. */
#define STP_PORT_SCAN_TMR_INTERVAL            50

/* The STP stack timer interval in milliseconds. */
#define STP_ONE_SECOND_TMR_INTERVAL           1000
/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
void dm8606c_rstp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* DM8606C_RSTP_H_ */
