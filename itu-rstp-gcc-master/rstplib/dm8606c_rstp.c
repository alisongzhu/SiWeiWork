/*
 * dm8606c_rstp.c
 *
 *  Created on: 2018年5月18日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "base.h"
//#include "bitmap.h"
//#include "uid_stp.h"
#include "stp_in.h"
#include "stp_to.h"
//#include "stpm.h"

#include "dm8606c_rstp.h"
#include "lwip/timeouts.h"
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
BITMAP_T    enabled_ports;
extern char MAC[6];
/* Private function prototypes -----------------------------------------------*/
static int dm8606c_rstp_start(void);
static void bridge_shutdown (void);

static void rstp_port_scan_tmr(void *arg);
static void rstp_one_second_tmr(void *arg);
/* Private functions ---------------------------------------------------------*/
static int dm8606c_rstp_start(void)
{
  BITMAP_T  ports;
  UID_STP_CFG_T uid_cfg;
  register int  iii;

  STP_IN_init (NUMBER_OF_PORTS);

  BitmapClear(&enabled_ports);

  BitmapClear(&ports);
  for (iii = 1; iii <= NUMBER_OF_PORTS; iii++) {
    BitmapSetBit(&ports, iii - 1);
  }

//  BitmapClear(&ports);
//  BitmapSetBit(&ports, 1);/*enable P1*/
//  BitmapSetBit(&ports, 2);/*enable P2*/
//
//  BitmapSetBit(&ports, 3);/*enable P3*/
////  BitmapSetBit(&ports, 5);/*enable P5*/

  uid_cfg.field_mask = BR_CFG_STATE;
  uid_cfg.stp_enabled = STP_ENABLED;

  unsigned int  tmp = 0;
  memcpy(&tmp,&MAC[2],4);

  snprintf (uid_cfg.vlan_name, NAME_LEN - 1, "Br-%08X", ntohl(tmp));
  iii = STP_IN_stpm_set_cfg (0, &ports, &uid_cfg);
  if (STP_OK != iii) {
    printf ("FATAL: can't enable:%s\n",
               STP_IN_get_error_explanation (iii));
    return (-1);
  }

  UID_STP_PORT_CFG_T pt_cfg;

  STP_IN_port_get_cfg(0,1,&pt_cfg);
  pt_cfg.field_mask = PT_CFG_NON_STP;
  pt_cfg.admin_non_stp = True;
  STP_IN_set_port_cfg(0,&pt_cfg);

  STP_IN_port_get_cfg(0,4,&pt_cfg);
  pt_cfg.field_mask = PT_CFG_NON_STP;
  pt_cfg.admin_non_stp = True;
  STP_IN_set_port_cfg(0,&pt_cfg);

  return 0;
}

static void bridge_shutdown (void)
{
  int       rc;

  rc = STP_IN_stpm_delete (0);
  if (STP_OK != rc) {
    printf ("FATAL: can't delete:%s\n",STP_IN_get_error_explanation (rc));
  }
}

static void rstp_port_scan_tmr(void *arg)
{
  (void) arg;
  register STPM_T* this;
  register PORT_T* port;

  int old_status,new_status;

//  if(True == STP_IN_get_is_stpm_enabled(0))
  {
    this = stpapi_stpm_find(0);

    for (port = this->ports; port; port = port->next)
    {
      old_status = BitmapGetBit(&enabled_ports, port->port_index - 1) > 0?1:0;
      new_status = STP_OUT_get_port_link_status(port->port_index);
      if(old_status != new_status)
      {
#if STP_DBG
        stp_trace("port%d status changed from %d to %d. \r\n ",port->port_index,old_status,new_status);
#endif
        STP_IN_enable_port(port->port_index,new_status);
      }
//      STP_IN_enable_port(port->port_index,new_status);
    }
  }
  sys_timeout(STP_PORT_SCAN_TMR_INTERVAL, rstp_port_scan_tmr, NULL);
}

static void rstp_one_second_tmr(void *arg)
{
  (void) arg;
  if(True == STP_IN_get_is_stpm_enabled(0))
  {
    STP_IN_one_second();
  }

  sys_timeout(STP_ONE_SECOND_TMR_INTERVAL, rstp_one_second_tmr, NULL);
}

/* Exported functions --------------------------------------------------------*/
void dm8606c_rstp_init(void)
{
  if(0 == dm8606c_rstp_start())
  {
    sys_timeout(STP_ONE_SECOND_TMR_INTERVAL, rstp_one_second_tmr, NULL);
    sys_timeout(STP_PORT_SCAN_TMR_INTERVAL, rstp_port_scan_tmr, NULL);
  }
  else bridge_shutdown();
}

//char* sprint_time_stump (void)
//{
//  time_t      clock;
//  struct      tm *local_tm;
//  static char time_str[20];
//
//  time(&clock);
//  local_tm = localtime (&clock);
//  strftime(time_str, 20 - 1, "%H:%M:%S", local_tm);
//  return time_str;
//}
