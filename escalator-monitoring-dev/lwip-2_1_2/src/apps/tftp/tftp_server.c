/****************************************************************//**
 *
 * @file tftp_server.c
 *
 * @author   Logan Gunthorpe <logang@deltatee.com>
 *           Dirk Ziegelmeier <dziegel@gmx.de>
 *
 * @brief    Trivial File Transfer Protocol (RFC 1350)
 *
 * Copyright (c) Deltatee Enterprises Ltd. 2013
 * All rights reserved.
 *
 ********************************************************************/

/*
 * Redistribution and use in source and binary forms, with or without
 * modification,are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Author: Logan Gunthorpe <logang@deltatee.com>
 *         Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

/**
 * @defgroup tftp TFTP server
 * @ingroup apps
 *
 * This is simple TFTP server for the lwIP raw API.
 */

#include "lwip/apps/tftp_server.h"

#if LWIP_UDP

#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "lwip/debug.h"


//#include "env.h"
//#include "can_network.h"
#include "crc.h"
#define NVS_EN
#ifdef NVS_EN
#include "flash.h"
#include "flashdef.h"
#include "bsp_nvs_config.h"
#else if
#include "flash_if.h"
#endif
#define TFTP_MAX_PAYLOAD_SIZE 512
#define TFTP_HEADER_LENGTH    4

#define TFTP_MAX_OPTION_LEN       10
#define TFTP_MAX_OPTION_VALLEN    20

#define TFTP_RRQ   1
#define TFTP_WRQ   2
#define TFTP_DATA  3
#define TFTP_ACK   4
#define TFTP_ERROR 5

enum tftp_error {
  TFTP_ERROR_FILE_NOT_FOUND    = 1,
  TFTP_ERROR_ACCESS_VIOLATION  = 2,
  TFTP_ERROR_DISK_FULL         = 3,
  TFTP_ERROR_ILLEGAL_OPERATION = 4,
  TFTP_ERROR_UNKNOWN_TRFR_ID   = 5,
  TFTP_ERROR_FILE_EXISTS       = 6,
  TFTP_ERROR_NO_SUCH_USER      = 7
};

#include <string.h>

struct tftp_state {
  const struct tftp_context *ctx;
  void *handle;
  struct pbuf *last_data;
  struct udp_pcb *upcb;
  ip_addr_t addr;
  u16_t port;
  int timer;
  int last_pkt;
  u16_t blknum;
  u8_t retries;
  u8_t mode_write;
};

typedef  void (*pFunction)(void);

static pFunction JumpToApplication;

typedef enum{
  FW_UNKNOWN = 0,
  FW_ITU,
  FW_EXTENDIO
}FW_INDICATOR;

static FW_INDICATOR indicator;

static uint32_t Flash_Write_Address;
static uint32_t JumpAddress;

static struct tftp_state tftp_state;

static void tftp_tmr(void* arg);

static void
close_handle(void)
{
  tftp_state.port = 0;
  ip_addr_set_any(0, &tftp_state.addr);

  if(tftp_state.last_data != NULL) {
    pbuf_free(tftp_state.last_data);
    tftp_state.last_data = NULL;
  }

  sys_untimeout(tftp_tmr, NULL);

  if (tftp_state.handle) {
    tftp_state.ctx->close(tftp_state.handle);
    tftp_state.handle = NULL;
    LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: closing\n"));
  }
}

static void
send_error(const ip_addr_t *addr, u16_t port, enum tftp_error code, const char *str)
{
  int str_length = strlen(str);
  struct pbuf* p;
  u16_t* payload;

  p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(TFTP_HEADER_LENGTH + str_length + 1), PBUF_RAM);
  if(p == NULL) {
    return;
  }

  payload = (u16_t*) p->payload;
  payload[0] = PP_HTONS(TFTP_ERROR);
  payload[1] = lwip_htons(code);
  MEMCPY(&payload[2], str, str_length + 1);

  udp_sendto(tftp_state.upcb, p, addr, port);
  pbuf_free(p);
}

static void
send_ack(u16_t blknum)
{
  struct pbuf* p;
  u16_t* payload;

  p = pbuf_alloc(PBUF_TRANSPORT, TFTP_HEADER_LENGTH, PBUF_RAM);
  if(p == NULL) {
    return;
  }
  payload = (u16_t*) p->payload;

  payload[0] = PP_HTONS(TFTP_ACK);
  payload[1] = lwip_htons(blknum);
  udp_sendto(tftp_state.upcb, p, &tftp_state.addr, tftp_state.port);
  pbuf_free(p);
}

static void
resend_data(void)
{
  struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, tftp_state.last_data->len, PBUF_RAM);
  if(p == NULL) {
    return;
  }

  if(pbuf_copy(p, tftp_state.last_data) != ERR_OK) {
    pbuf_free(p);
    return;
  }

  udp_sendto(tftp_state.upcb, p, &tftp_state.addr, tftp_state.port);
  pbuf_free(p);
}

static void
send_data(void)
{
  u16_t *payload;
  int ret;

  if(tftp_state.last_data != NULL) {
    pbuf_free(tftp_state.last_data);
  }

  tftp_state.last_data = pbuf_alloc(PBUF_TRANSPORT, TFTP_HEADER_LENGTH + TFTP_MAX_PAYLOAD_SIZE, PBUF_RAM);
  if(tftp_state.last_data == NULL) {
    return;
  }

  payload = (u16_t *) tftp_state.last_data->payload;
  payload[0] = PP_HTONS(TFTP_DATA);
  payload[1] = lwip_htons(tftp_state.blknum);

  ret = tftp_state.ctx->read(tftp_state.handle, &payload[2], TFTP_MAX_PAYLOAD_SIZE);
  if (ret < 0) {
    send_error(&tftp_state.addr, tftp_state.port, TFTP_ERROR_ACCESS_VIOLATION, "Error occured while reading the file.");
    close_handle();
    return;
  }

  pbuf_realloc(tftp_state.last_data, (u16_t)(TFTP_HEADER_LENGTH + ret));
  resend_data();
}

static void
recv(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
  u16_t *sbuf = (u16_t *) p->payload;
  int opcode;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(upcb);

  if (((tftp_state.port != 0) && (port != tftp_state.port)) ||
      (!ip_addr_isany_val(tftp_state.addr) && !ip_addr_cmp(&tftp_state.addr, addr))) {
    send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Only one connection at a time is supported");
    pbuf_free(p);
    return;
  }

  opcode = sbuf[0];

  tftp_state.last_pkt = tftp_state.timer;
  tftp_state.retries = 0;

  switch (opcode) {
    case PP_HTONS(TFTP_RRQ): /* fall through */
    case PP_HTONS(TFTP_WRQ):
    {
      const char tftp_null = 0;
      char filename[TFTP_MAX_FILENAME_LEN];
      char mode[TFTP_MAX_MODE_LEN];

      u16_t filename_end_offset;
      u16_t mode_end_offset;

//      char option[TFTP_MAX_OPTION_LEN];
//      char op_val[TFTP_MAX_OPTION_VALLEN];
//      u16_t option_end_offset;
//      u16_t option_val_end_offset;

      if(tftp_state.handle != NULL) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Only one connection at a time is supported");
        break;
      }

      sys_timeout(TFTP_TIMER_MSECS, tftp_tmr, NULL);

      /* find \0 in pbuf -> end of filename string */
      filename_end_offset = pbuf_memfind(p, &tftp_null, sizeof(tftp_null), 2);
      if((u16_t)(filename_end_offset-2) > sizeof(filename)) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Filename too long/not NULL terminated");
        break;
      }
      pbuf_copy_partial(p, filename, filename_end_offset-2, 2);

      /* find \0 in pbuf -> end of mode string */
      mode_end_offset = pbuf_memfind(p, &tftp_null, sizeof(tftp_null), filename_end_offset+1);
      if((u16_t)(mode_end_offset-filename_end_offset) > sizeof(mode)) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Mode too long/not NULL terminated");
        break;
      }
      pbuf_copy_partial(p, mode, mode_end_offset-filename_end_offset, filename_end_offset+1);

      tftp_state.handle = tftp_state.ctx->open(filename, mode, opcode == PP_HTONS(TFTP_WRQ));
      tftp_state.blknum = 1;

      if (!tftp_state.handle) {
        send_error(addr, port, TFTP_ERROR_FILE_NOT_FOUND, "Unable to open requested file.");
        break;
      }

      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: %s request from ", (opcode == PP_HTONS(TFTP_WRQ)) ? "write" : "read"));
      ip_addr_debug_print(TFTP_DEBUG | LWIP_DBG_STATE, addr);
      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, (" for '%s' mode '%s'\n", filename, mode));



      ip_addr_copy(tftp_state.addr, *addr);
      tftp_state.port = port;

      if (opcode == PP_HTONS(TFTP_WRQ)) {
        tftp_state.mode_write = 1;
        send_ack(0);
      } else {
        tftp_state.mode_write = 0;
        send_data();
      }

      break;
    }

    case PP_HTONS(TFTP_DATA):
    {
      int ret;
      u16_t blknum;

      if (tftp_state.handle == NULL) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "No connection");
        break;
      }

      if (tftp_state.mode_write != 1) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Not a write connection");
        break;
      }

      blknum = lwip_ntohs(sbuf[1]);
      pbuf_header(p, -TFTP_HEADER_LENGTH);

      ret = tftp_state.ctx->write(tftp_state.handle, p);
      if (ret < 0) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "error writing file");
        close_handle();
      } else {
        send_ack(blknum);
      }

      if (p->tot_len < TFTP_MAX_PAYLOAD_SIZE) {
        close_handle();

        if(indicator == FW_ITU)
        {
//          SystemEnv.fw_iTU_size = TFTP_MAX_PAYLOAD_SIZE*( blknum - 1) + p->tot_len;
//
//          SystemEnv.fw_iTU_crc = HAL_CRC_Calculate(&hcrc,(uint32_t *)APPLICATION_ADDRESS_BACKUP,SystemEnv.fw_iTU_size/4);
//          /*modify need_upgrade flag ,jump to bootloader*/
//          SystemEnv.need_reload = 0;
//          sys_env_save(&SystemEnv);

          __disable_irq();
  //        if (((*(__IO uint32_t*)FLASH_BASE) & 0x2FFE0000 ) == 0x20000000)
          if ((*(__IO uint32_t*)APPLICATION_ADDRESS)  == 0x20020000)
          {
            /* Jump to user application */
            JumpAddress = *(__IO uint32_t*) (FLASH_BASE + 4);
            JumpToApplication = (pFunction) JumpAddress;

            __set_CONTROL(0);

            /* Initialize user application's Stack Pointer */
            __set_MSP(*(__IO uint32_t*) FLASH_BASE);
            JumpToApplication();
          }
        }
//        else if(indicator == FW_EXTENDIO)
//        {
//          SystemEnv.fw_extend_size = TFTP_MAX_PAYLOAD_SIZE*( blknum - 1) + p->tot_len;
//          SystemEnv.fw_extend_crc = HAL_CRC_Calculate(&hcrc,(uint32_t *)APPLICATION_ADDRESS_BACKUP,SystemEnv.fw_extend_size/4);
//          sys_env_save(&SystemEnv);
//          vTransmitRemoteFrame(0x1f,0x0f,0xff,CAN_FRAME_COMMAND_FW_UPGRADE,0);
//        }
      }
      break;
    }

    case PP_HTONS(TFTP_ACK):
    {
      u16_t blknum;
      int lastpkt;

      if (tftp_state.handle == NULL) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "No connection");
        break;
      }

      if (tftp_state.mode_write != 0) {
        send_error(addr, port, TFTP_ERROR_ACCESS_VIOLATION, "Not a read connection");
        break;
      }

      blknum = lwip_ntohs(sbuf[1]);
      if (blknum != tftp_state.blknum) {
        send_error(addr, port, TFTP_ERROR_UNKNOWN_TRFR_ID, "Wrong block number");
        break;
      }

      lastpkt = 0;

      if (tftp_state.last_data != NULL) {
        lastpkt = tftp_state.last_data->tot_len != (TFTP_MAX_PAYLOAD_SIZE + TFTP_HEADER_LENGTH);
      }

      if (!lastpkt) {
        tftp_state.blknum++;
        send_data();
      } else {
        close_handle();
      }

      break;
    }

    default:
      send_error(addr, port, TFTP_ERROR_ILLEGAL_OPERATION, "Unknown operation");
      break;
  }

  pbuf_free(p);
}

static void
tftp_tmr(void* arg)
{
  LWIP_UNUSED_ARG(arg);

  tftp_state.timer++;

  if (tftp_state.handle == NULL) {
    return;
  }

  sys_timeout(TFTP_TIMER_MSECS, tftp_tmr, NULL);

  if ((tftp_state.timer - tftp_state.last_pkt) > (TFTP_TIMEOUT_MSECS / TFTP_TIMER_MSECS)) {
    if ((tftp_state.last_data != NULL) && (tftp_state.retries < TFTP_MAX_RETRIES)) {
      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout, retrying\n"));
      resend_data();
      tftp_state.retries++;
    } else {
      LWIP_DEBUGF(TFTP_DEBUG | LWIP_DBG_STATE, ("tftp: timeout\n"));
      close_handle();
    }
  }
}

static void* IAP_open(const char* fname, const char* mode, u8_t write)
{
  if(!write) return NULL;
  if(strcmp(mode,"octet")) return NULL;
#ifdef NVS_EN
	((struct flash_driver_api*)(flash_dev.driver_api))->erase(&flash_dev,APPLICATION_ADDRESS_BACKUP-FLASH_BASE	,APPLICATION_PAGE);
#else
  /* init flash */
  FLASH_If_Init();
  /* erase user flash area */
  FLASH_If_Erase(APPLICATION_ADDRESS_BACKUP,APPLICATION_SECTORS);
#endif
  Flash_Write_Address = APPLICATION_ADDRESS_BACKUP;

  if(!memcmp(fname,"update_iTU.bin",14))
  {
    indicator = FW_EXTENDIO;
  }
  else if(!memcmp(fname,"escalator-monitoring.bin",strlen("escalator-monitoring.bin")))
  {
    indicator = FW_ITU;
  }
  else
    indicator = FW_UNKNOWN;
  return (void*)1;
//  return NULL;
}

static void IAP_close(void* handle)
{
#ifdef NVS_EN
	((struct flash_driver_api*)(flash_dev.driver_api))->write_protection(&flash_dev,DISABLE);
#else
  HAL_FLASH_Lock();
#endif
  handle = NULL;
}

static int IAP_write(void* handle, struct pbuf* p)
{
#ifdef NVS_EN
     uint32_t FlsahAddr = Flash_Write_Address;
	 Flash_Write_Address += p->tot_len;
	return ((struct flash_driver_api*)(flash_dev.driver_api))->write(&flash_dev,FlsahAddr - FLASH_BASE, p->payload,p->tot_len);
#else
	  return FLASH_If_Write(&Flash_Write_Address, p->payload ,p->tot_len/4);
#endif
	  Flash_Write_Address += p->tot_len;
}


static struct tftp_context IAP_ctx = {
    IAP_open,
    IAP_close,
    NULL,
    IAP_write
};

/** @ingroup tftp
 * Initialize TFTP server.
 * @param ctx TFTP callback struct
 */
err_t
tftp_init(const struct tftp_context *ctx)
{
  err_t ret;

  struct udp_pcb *pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
  if (pcb == NULL) {
    return ERR_MEM;
  }

  ret = udp_bind(pcb, IP_ANY_TYPE, TFTP_PORT);
  if (ret != ERR_OK) {
    udp_remove(pcb);
    return ret;
  }

  tftp_state.handle    = NULL;
  tftp_state.port      = 0;
  tftp_state.ctx       = ctx?ctx:&IAP_ctx;
  tftp_state.timer     = 0;
  tftp_state.last_data = NULL;
  tftp_state.upcb      = pcb;

  udp_recv(pcb, recv, NULL);

  return ERR_OK;
}

#endif /* LWIP_UDP */
