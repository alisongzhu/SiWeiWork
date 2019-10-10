/*
 * protocol_powerbus.c
 *
 *  Created on: 2019年9月28日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/


#include "include/protocol_powerbus.h"

#include "bsp.h"
#include "lib/include/crc8.h"
/* Private macros ------------------------------------------------------------*/
#define PROTO_MSG_MIN_SIZE     4

#define PROTO_CMD_UNKNOWN      0
#define PROTO_CMD_DUTY_CALL    1


#define CMD_DUTY_CALL_RX_PAYLOAD_SIZE      0

/* Private types -------------------------------------------------------------*/
typedef struct pdu_cmd_duty_call_{
  module_address_param_t address;
  uint8_t command;

}__packed pdu_t;

typedef enum proto_cmd_{
  duty_call = 1,

  unknown
} proto_cmd_t;
/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void proc_command(protocol_t *proto,uint8_t *msg,uint16_t len)
{
  uint8_t cmd = msg[0];
  len --;
  switch (cmd) {
    case PROTO_CMD_UNKNOWN:
      break;
    case PROTO_CMD_DUTY_CALL:
      if(len != CMD_DUTY_CALL_RX_PAYLOAD_SIZE)
      {
        return;
      }

      break;
    default:
      break;
  }
}

/* Exported functions --------------------------------------------------------*/
int proc_msg(protocol_t *proto,uint8_t *msg,uint16_t len)
{
  int rc = 0;
  uint8_t tx_flag = 0;

  if(len < PROTO_MSG_MIN_SIZE)
  {
    return 1; /* invalid length*/
  }

  if(crc8_ccitt(0xff,msg,len) != 0)
  {
    return 2;/* invalid crc8*/
  }

  module_address_param_t *param = (module_address_param_t *)msg;
  if(param->net == address.net && param->id == address.id)
  {
    if(uxQueueMessagesWaiting(proto->tx_queue))
    {
      proto->start_tx();
      tx_flag = 1;
    }

    msg = msg + sizeof(module_address_param_t);
    len = len - sizeof(module_address_param_t) -1;

    return 0;
  }

  if((param->net != 0xff)||(param->net != address.net))
  {
    return 3;/* invalid net*/
  }

  if(param->id != 0x00 || param->id != address.id)
  {
    return 4;/* invalid id*/
  }


  return rc;
}
