/************************************************************************ 
 * RSTP library - Rapid Spanning Tree (802.1t, 802.1w) 
 * Copyright (C) 2001-2003 Optical Access 
 * Author: Alex Rozin 
 * 
 * This file is part of RSTP library. 
 * 
 * RSTP library is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License as published by the 
 * Free Software Foundation; version 2.1 
 * 
 * RSTP library is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with RSTP library; see the file COPYING.  If not, write to the Free 
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
 * 02111-1307, USA. 
 **********************************************************************/

/* This file contains system dependent API
   from the RStp to a operation system (see stp_to.h) */

/* stp_to API for Linux */

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "base.h"
#include "stpm.h"
#include "stp_in.h"
#include "stp_to.h"

#include "stm32f4xx_hal.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"

extern BITMAP_T        enabled_ports;

extern ETH_HandleTypeDef heth;
extern uint8_t MAC[6] ;
extern struct netif gnetif;
/*************
void
stp_trace (const char *format, ...)
{
  #define MAX_MSG_LEN  128
  char     msg[MAX_MSG_LEN];
  va_list  args;

  va_start(args, format);
  vsnprintf (msg, MAX_MSG_LEN-1, format, args);
  printf ("%s\n", msg);
  va_end(args);
  
}
***********/

#ifdef STRONGLY_SPEC_802_1W
int
STP_OUT_set_learning (int port_index, int vlan_id, int enable)
{
  return STP_OK;
}

int
STP_OUT_set_forwarding (int port_index, int vlan_id, int enable)
{
  return STP_OK;
}
#else
/* 
 * In many kinds of hardware the state of ports may
 * be changed with another method
 */
int
STP_OUT_set_port_state (IN int port_index, IN int vlan_id,
            IN RSTP_PORT_STATE state)
{
  uint32_t reg = 0;

  switch(state)
  {
    case UID_PORT_DISCARDING:
      reg = 1u;
      break;
    case UID_PORT_FORWARDING:
      reg = 0u;
      break;
    case UID_PORT_LEARNING:
      reg = 2u;
      break;
    default:

      break;
  }

  heth.Init.PhyAddress = 0x08 + port_index-1 ;
  HAL_ETH_WritePHYRegister(&heth, 0x19,reg);

  return STP_OK;
  //return AR_INT_STP_set_port_state (port_index, vlan_id, state);
}
#endif


void
STP_OUT_get_port_mac (int port_index, unsigned char *mac)
{
  memcpy (mac, MAC, 6);
  mac[5] += port_index;
  //memcpy (mac, STP_MAIN_get_port_mac (port_index), 6);
}

int             /* 1- Up, 0- Down */
STP_OUT_get_port_link_status (int port_index)
{
  uint32_t reg = 0;
  int val = 0;

  heth.Init.PhyAddress = 0x08 + port_index-1 ;
  HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);

  if(reg&1u)
  {
    BitmapSetBit (&enabled_ports, port_index - 1);
    val = 1;
  }
  else
  {
    BitmapClearBit (&enabled_ports, port_index - 1);
    val = 0;
  }

  return val;
}

int
STP_OUT_flush_lt (IN int port_index, IN int vlan_id, LT_FLASH_TYPE_T type, char* reason)
{
#if STP_DBG
  stp_trace("clearFDB (%d, %s, '%s')",
        port_index, 
        (type == LT_FLASH_ALL_PORTS_EXCLUDE_THIS) ? "Exclude" : "Only", 
        reason);
#endif

//#define DM8606_FDB_RESET_CLEAR
#ifdef DM8606_FDB_RESET_CLEAR
  heth.Init.PhyAddress = 0x10U;
  HAL_ETH_WritePHYRegister(&heth, 0x11u, 1);
#else
  uint32_t reg = 0;
  HAL_StatusTypeDef status;
  heth.Init.PhyAddress = 0x15;
  //address table access busy flag: 1= busy,0 = available.
  do{
    status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
  }while((reg >> 15) ||(status != HAL_OK));

  if(port_index == 0)
  {
//    HAL_ETH_WritePHYRegister(&heth, 0x11,1);
//    HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    osDelay(5);
//
//    do{
//      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
//    }while((reg >> 15) ||(status != HAL_OK));
//
//    HAL_ETH_WritePHYRegister(&heth, 0x11,2);
//    HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    osDelay(5);
//
//    do{
//      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
//    }while((reg >> 15) ||(status != HAL_OK));
//
//    HAL_ETH_WritePHYRegister(&heth, 0x11,3);
//    HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    osDelay(5);
//
//    do{
//      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
//    }while((reg >> 15) ||(status != HAL_OK));
//
//    HAL_ETH_WritePHYRegister(&heth, 0x11,5);
//    HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    osDelay(5);
//
//    do{
//      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
//    }while((reg >> 15) ||(status != HAL_OK));
//
//    HAL_ETH_WritePHYRegister(&heth, 0x11,0x3f);
//    HAL_ETH_WritePHYRegister(&heth, 0x10,0x31u);
//
//    osDelay(5);
//
//    do{
//      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
//    }while((reg >> 15) ||(status != HAL_OK));
    return STP_OK;
  }

  if(LT_FLASH_ONLY_THE_PORT == type)
  {
    /*clear unicast table only the port*/
    HAL_ETH_WritePHYRegister(&heth, 0x11,port_index-1);
    HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);

//    osDelay(5);

    do{
      status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
    }while((reg >> 15) ||(status != HAL_OK));

    /* clear multicast table only the port */
    HAL_ETH_WritePHYRegister(&heth, 0x11,1<<(port_index-1));
    HAL_ETH_WritePHYRegister(&heth, 0x10,0x31u);

  }

  if(LT_FLASH_ALL_PORTS_EXCLUDE_THIS == type)
  {
    for(uint8_t i = 1; i< 6; i++)
    {
      if((port_index - 1) == i) continue;

//      if(i == 3) continue;

      HAL_ETH_WritePHYRegister(&heth, 0x11,i);
      HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);

      // wait at least 4.5ms
//      osDelay(5);

      do{
        status = HAL_ETH_ReadPHYRegister(&heth, 0x10,&reg);
      }while((reg >> 15) ||(status != HAL_OK));
    }
    HAL_ETH_WritePHYRegister(&heth, 0x11,0x3f);
    HAL_ETH_WritePHYRegister(&heth, 0x10,0x31u);

//    if(port_index == 2)
//    {
//      HAL_ETH_WritePHYRegister(&heth, 0x11,2);
//      HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    }
//    else if(port_index == 3)
//    {
//      HAL_ETH_WritePHYRegister(&heth, 0x11,1);
//      HAL_ETH_WritePHYRegister(&heth, 0x10,0x30u);
//    }

  }

#endif
  return STP_OK;
}

int
STP_OUT_set_hardware_mode (int vlan_id, UID_STP_MODE_T mode)
{
  switch(mode)
  {
    case STP_DISABLED:
      heth.Init.PhyAddress = 0x14;
      HAL_ETH_WritePHYRegister(&heth, 0x12, 0);
      break;
    case STP_ENABLED:
      heth.Init.PhyAddress = 0x14;
      HAL_ETH_WritePHYRegister(&heth, 0x12, 1);
      break;
  }
  return STP_OK;
  //return AR_INT_STP_set_mode (vlan_id, mode);
}


int
STP_OUT_tx_bpdu (int port_index, int vlan_id,
         unsigned char *bpdu, size_t bpdu_len)
{
  if(port_index == 6) return 0;

  struct pbuf *p = pbuf_alloc(PBUF_RAW_TX, bpdu_len + 21, PBUF_RAM);

  memcpy(p->payload,bpdu,12);

//  char special_tag[4];
//  special_tag[0] = 0x86;
//  special_tag[1] = 0x06;
//  special_tag[2] = 0x40 | (1<<(port_index-1));
//  special_tag[3] = 0x3e;
//  memcpy(p->payload+12,special_tag,4);

  char *tmp = (char*)p->payload + 12;
  *tmp++ =  0x86;
  *tmp++ =  0x06;
  *tmp++ =  0x40 | (1<<(port_index-1));
  *tmp++ =  0x3e;

  memcpy(p->payload+16,bpdu+12,bpdu_len+5);

  gnetif.linkoutput(&gnetif, p);
  pbuf_free(p);

  return 0;
}

const char *
STP_OUT_get_port_name (IN int port_index)
{
  static char tmp[4];
  sprintf (tmp, "p%02d", (int) port_index);
  return tmp;
  //return port2str (port_index, &sys_config);
}

unsigned long
STP_OUT_get_deafult_port_path_cost (IN unsigned int portNo)
{
  return 200000;//802.1D Page 154
}

unsigned long STP_OUT_get_port_oper_speed (unsigned int portNo)
{
  return 100000L;// unit: kb/s
}

int             /* 1- Full, 0- Half */
STP_OUT_get_duplex (IN int port_index)
{
  return 1;
}

int
STP_OUT_get_init_stpm_cfg (IN int vlan_id,
                           INOUT UID_STP_CFG_T* cfg)
{
  cfg->bridge_priority =        DEF_BR_PRIO;
  cfg->max_age =                DEF_BR_MAXAGE;
  cfg->hello_time =             DEF_BR_HELLOT;
  cfg->forward_delay =          DEF_BR_FWDELAY;
  cfg->force_version =          NORMAL_RSTP;

  return STP_OK;
}
  

int
STP_OUT_get_init_port_cfg (IN int vlan_id,
                           IN int port_index,
                           INOUT UID_STP_PORT_CFG_T* cfg)
{
  cfg->port_priority =                  DEF_PORT_PRIO;
  cfg->admin_non_stp =                  DEF_ADMIN_NON_STP;
  cfg->admin_edge =                     DEF_ADMIN_EDGE;
  cfg->admin_port_path_cost =           ADMIN_PORT_PATH_COST_AUTO;
  cfg->admin_point2point =              DEF_P2P;

  return STP_OK;
}



