/*
 * user_config_raw.c
 *
 *  Created on: 2018年6月5日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include "user_config_raw.h"

#if LWIP_UDP
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"
#include "env.h"
#include "gpio.h"

#include <string.h>
#include "build_defs.h"
/* Private macros ------------------------------------------------------------*/
#define MSG_TYPE_UNKNOWN            0
#define MSG_TYPE_DISCOVER           1
#define MSG_TYPE_CONFIG             2
#define MSG_TYPE_SUPERVISORY        3
#define MSG_TYPE_DIRECT_CONTROL     4
#define MSG_TYPE_RESET              5
#define MSG_TYPE_READ_PROP          6
#define MSG_TYPE_UPGRADE            7
#define MSG_TYPE_RESTORE            9

#define MSG_TYPE_SIZE               1
#define MSG_HEADER_SIZE             12
/* Private types -------------------------------------------------------------*/
typedef struct{
  uint8_t header[12];
  uint8_t code;
}USER_MSG_T;
/* Private constants ---------------------------------------------------------*/
static char ver[_VERSION_LENGTH_] = _COMMIT_VERSION_;
/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
extern uint32_t UID[3];
extern DiscreteInfo InternalDiscreteInfo[INTERNAL_DISCRETE_COUNT];

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void
recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  USER_MSG_T *msg = (USER_MSG_T *)p->payload;
  if((p->tot_len < (MSG_TYPE_SIZE + MSG_HEADER_SIZE))
    ||(((msg->code != MSG_TYPE_DISCOVER)&&(memcmp(msg->header,&UID,MSG_HEADER_SIZE) != 0))))
  {
    LWIP_DEBUGF(USER_CONIG_DEBUG | LWIP_DBG_TRACE, ("recv invalid packet,type:%d\n",msg->code));
    goto free_and_return;
  }

  struct pbuf *tx ;
//  char ver[_VERSION_LENGTH_] = _COMMIT_VERSION_;
  switch(msg->code)
  {
    case MSG_TYPE_DISCOVER       :
      tx = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(MSG_TYPE_SIZE + MSG_HEADER_SIZE + _VERSION_LENGTH_), PBUF_RAM);

      pbuf_take(tx,&UID,MSG_HEADER_SIZE);
      pbuf_take_at(tx,&msg->code,MSG_TYPE_SIZE,MSG_HEADER_SIZE);
      pbuf_take_at(tx,&ver,_VERSION_LENGTH_,MSG_TYPE_SIZE + MSG_HEADER_SIZE);

      udp_sendto(upcb, tx, IP_ADDR_BROADCAST, port);
      pbuf_free(tx);
      break;

    case MSG_TYPE_CONFIG         :
      pbuf_header(p,(s16_t)-(MSG_TYPE_SIZE + MSG_HEADER_SIZE));
      if(user_env_save((EnvTable *)p->payload) == 0)
      {
        SystemEnv.config_flag = 0;
        sys_env_save(&SystemEnv);
        HAL_NVIC_SystemReset();
      }

      break;

    case MSG_TYPE_SUPERVISORY    :
      tx = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(MSG_TYPE_SIZE + MSG_HEADER_SIZE + INTERNAL_DISCRETE_COUNT), PBUF_RAM);
      pbuf_take(tx,&UID,MSG_HEADER_SIZE);
      pbuf_take_at(tx,&msg->code,MSG_TYPE_SIZE,MSG_HEADER_SIZE);

      for(uint8_t i=0; i<INTERNAL_DISCRETE_COUNT; i++){
        pbuf_take_at(tx,&InternalDiscreteInfo[i].val, 1,MSG_TYPE_SIZE + MSG_HEADER_SIZE + i);
      }

      udp_sendto(upcb, tx, IP_ADDR_BROADCAST, port);
      pbuf_free(tx);
      break;

    case MSG_TYPE_DIRECT_CONTROL :
      pbuf_header(p,(s16_t)-(MSG_TYPE_SIZE + MSG_HEADER_SIZE));
      uint8_t *ptr = p->payload;

      env.BasicEnv.CoilOutputMode = COIL_MODE_DIRECT;
      HAL_GPIO_WritePin(SELECT_DO_GPIO_Port,SELECT_DO_Pin,GPIO_PIN_SET);
      HAL_Delay(100);

      for(uint8_t i=0;i<INTERNAL_COIL_COUNT;i++)
      {
        *ptr++ ? ExecuteInternalCoilOutputON(i):ExecuteInternalCoilOutputOFF(i);
      }
      break;

    case MSG_TYPE_RESET          :
      HAL_NVIC_SystemReset();
      break;

    case MSG_TYPE_READ_PROP      :
      tx = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(MSG_TYPE_SIZE + MSG_HEADER_SIZE + sizeof(env)), PBUF_RAM);

      pbuf_take(tx,&UID,MSG_HEADER_SIZE);
      pbuf_take_at(tx,&msg->code, MSG_TYPE_SIZE ,MSG_HEADER_SIZE);
      pbuf_take_at(tx,&env, sizeof(env), MSG_TYPE_SIZE + MSG_HEADER_SIZE);

      udp_sendto(upcb, tx, IP_ADDR_BROADCAST, port);
      pbuf_free(tx);
      break;

    case MSG_TYPE_RESTORE        :
      SystemEnv.config_flag = 0xff;
      sys_env_save(&SystemEnv);
      HAL_NVIC_SystemReset();
      break;

    default:
      LWIP_DEBUGF(USER_CONIG_DEBUG | LWIP_DBG_TRACE, ("unknown command,type:%d\n",msg->code));
      break;
  }

free_and_return:
  pbuf_free(p);
  return;
}
/* Exported functions --------------------------------------------------------*/
err_t user_config_raw_init(void)
{
  err_t ret;

  /* LWIP_ASSERT_CORE_LOCKED(); is checked by udp_new() */
  struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (pcb == NULL) {
    return ERR_MEM;
  }

  ret = udp_bind(pcb, IP_ANY_TYPE, USER_CONFIG_PORT);
  if (ret != ERR_OK) {
    udp_remove(pcb);
    return ret;
  }

  udp_recv(pcb, recv, NULL);

  return ERR_OK;
}

#endif /* LWIP_UDP */
