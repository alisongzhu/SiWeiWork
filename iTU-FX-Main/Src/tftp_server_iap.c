/*
 * dtu.c
 *  
 *  Created on: Jan 6 2018
 *      Author: LH
 */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "tftpserver.h"
#include "flash_if.h"
#include "api.h"
#include "env.h"
#include <string.h>

/* Private macros ------------------------------------------------------------*/
#define CONN_RECV_TIMEOUT          10
/* Private types -------------------------------------------------------------*/
typedef struct
{
  int op;    /*WRQ */
  /* last block read */
//  char data[TFTP_DATA_PKT_LEN_MAX];
  int  data_len;
  /* destination ip:port */
  ip_addr_t to_ip;
  int to_port;
  /* next block number */
  int block;
  /* total number of bytes transferred */
  int tot_bytes;
  /* timer interrupt count when last packet was sent */
  /* this should be used to resend packets on timeout */
  unsigned long long last_time;
}tftp_connection_args;

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
typedef  void (*pFunction)(void);
static pFunction JumpToApplication;
static uint32_t JumpAddress;

static uint32_t Flash_Write_Address;
static struct netconn *conn,*conn_data;
static __IO uint32_t total_count=0;

tftp_connection_args *args ;

static TaskHandle_t handle;

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static tftp_opcode IAP_tftp_decode_op(char *buf);
static void IAP_wrq_recv(void *handle, struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port);
static void IAP_tftp_recv(void *handle, struct pbuf *p, const ip_addr_t *source_ip, u16_t port);
static int IAP_tftp_process_write(void *handle, const ip_addr_t *to, int to_port);
static void IAP_tftp_cleanup_wr(void *handle, tftp_connection_args *args);
static tftp_opcode IAP_tftp_decode_op(char *buf);
static u16_t IAP_tftp_extract_block(char *buf);
static void IAP_tftp_set_block(char* packet, u16_t block);
static err_t IAP_tftp_send_ack_packet(void *handle, const ip_addr_t *to, int to_port, int block);

static void vTaskTftpd(void *argument);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief Returns the TFTP opcode 
  * @param buf: pointer on the TFTP packet 
  * @retval None
  */
static tftp_opcode IAP_tftp_decode_op(char *buf)
{
  return (tftp_opcode)(buf[1]);
}

/**
  * @brief  Extracts the block number
  * @param  buf: pointer on the TFTP packet 
  * @retval block number
  */
static u16_t IAP_tftp_extract_block(char *buf)
{
  u16_t *b = (u16_t*)buf;
  return ntohs(b[1]);
}

/**
  * @brief Sets the TFTP opcode 
  * @param  buffer: pointer on the TFTP packet
  * @param  opcode: TFTP opcode
  * @retval None
  */
static void IAP_tftp_set_opcode(char *buffer, tftp_opcode opcode)
{
  buffer[0] = 0;
  buffer[1] = (u8_t)opcode;
}

/**
  * @brief Sets the TFTP block number 
  * @param packet: pointer on the TFTP packet 
  * @param  block: block number
  * @retval None
  */
static void IAP_tftp_set_block(char* packet, u16_t block)
{
  u16_t *p = (u16_t *)packet;
  p[1] = htons(block);
}

/**
  * @brief Sends TFTP ACK packet  
  * @param handle: pointer on connection handler
  * @param to: pointer on the receive IP address structure
  * @param to_port: receive port number
  * @param block: block number
  * @retval: err_t: error code 
  */
static err_t IAP_tftp_send_ack_packet(void *handle, const ip_addr_t *dst, int port, int block)
{
    err_t err;
    struct netbuf *buf = netbuf_new();
    
    char *p = netbuf_alloc(buf,TFTP_ACK_PKT_LEN);

    /* define the first two bytes of the packet */
    IAP_tftp_set_opcode(p, TFTP_ACK);

    /* Specify the block number being ACK'd.
     * If we are ACK'ing a DATA pkt then the block number echoes that of the DATA pkt being ACK'd (duh)
     * If we are ACK'ing a WRQ pkt then the block number is always 0
     * RRQ packets are never sent ACK pkts by the server, instead the server sends DATA pkts to the
     * host which are, obviously, used as the "acknowledgement".  This saves from having to sEndTransferboth
     * an ACK packet and a DATA packet for RRQs - see RFC1350 for more info.  */
    IAP_tftp_set_block(p, block);

    /* Sending packet by UDP protocol */
    err = netconn_sendto((struct netconn*)handle, buf, dst, port);

    /* free the buffer netbuf */
    netbuf_delete(buf);

    return err;
}

/**
  * @brief  Processes data transfers after a TFTP write request
  * @param  _args: used as pointer on TFTP connection args
  * @param  upcb: pointer on udp_pcb structure
  * @param  pkt_buf: pointer on a pbuf stucture
  * @param  ip_addr: pointer on the receive IP_address structure
  * @param  port: receive port address
  * @retval None
  */
static void IAP_wrq_recv(void *handle, struct pbuf *pkt_buf, const ip_addr_t *addr, u16_t port)
{

  uint32_t data_buffer[128];
  uint16_t count=0;

  if (pkt_buf->len != pkt_buf->tot_len)
  {
    return;
  }

  /* Does this packet have any valid data to write? */
  if ((pkt_buf->len > TFTP_DATA_PKT_HDR_LEN) &&
      (IAP_tftp_extract_block(pkt_buf->payload) == (args->block + 1)))
  {
    /* copy packet payload to data_buffer */
    pbuf_copy_partial(pkt_buf, data_buffer, pkt_buf->len - TFTP_DATA_PKT_HDR_LEN,
                      TFTP_DATA_PKT_HDR_LEN);
    
    total_count += pkt_buf->len - TFTP_DATA_PKT_HDR_LEN; 
    
    count = (pkt_buf->len - TFTP_DATA_PKT_HDR_LEN)/4;
    if (((pkt_buf->len - TFTP_DATA_PKT_HDR_LEN)%4)!=0) 
    count++;
     
    /* Write received data in Flash */
    FLASH_If_Write(&Flash_Write_Address, data_buffer ,count);
       
    /* update our block number to match the block number just received */
    args->block++;
    /* update total bytes  */
    (args->tot_bytes) += (pkt_buf->len - TFTP_DATA_PKT_HDR_LEN);

    /* This is a valid pkt but it has no data.  This would occur if the file being
       written is an exact multiple of 512 bytes.  In this case, the args->block
       value must still be updated, but we can skip everything else.    */
  }
  else if (IAP_tftp_extract_block(pkt_buf->payload) == (args->block + 1))
  {
    /* update our block number to match the block number just received  */
    args->block++;
  }
  
  /* Send the appropriate ACK pkt*/
  IAP_tftp_send_ack_packet(handle, addr, port, args->block);   

  /* If the last write returned less than the maximum TFTP data pkt length,
   * then we've received the whole file and so we can quit (this is how TFTP
   * signals the EndTransferof a transfer!)
   */
  if (pkt_buf->len < TFTP_DATA_PKT_LEN_MAX)
  {
    IAP_tftp_cleanup_wr(handle, args);
    pbuf_free(pkt_buf);
		
		//todo: add need_upgrade flag ,jump to bootloader
		SystemEnv.need_reload = 0;
		SystemEnv.config_flag = 0xff;
		sys_env_save();
		__disable_irq();
		if (((*(__IO uint32_t*)FLASH_BASE) & 0x2FFE0000 ) == 0x20000000)
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
  else
  {
    pbuf_free(pkt_buf);
    return;
  }
}

/**
  * @brief  Processes TFTP write request
  * @param  to: pointer on the receive IP address
  * @param  to_port: receive port number
  * @retval None
  */
static int IAP_tftp_process_write(void *handle, const ip_addr_t *to, int to_port)
{
  args = pvPortMalloc(sizeof *args);

  args->op = TFTP_WRQ;
  args->to_ip.addr = to->addr;
  args->to_port = to_port;
  /* the block # used as a positive response to a WRQ is _always_ 0!!! (see RFC1350)  */
  args->block = 0;
  args->tot_bytes = 0;
  
  total_count =0;

  /* init flash */
  FLASH_If_Init();
  
  /* erase user flash area */
  FLASH_If_Erase(APPLICATION_ADDRESS_BACKUP,APPLICATION_SECTORS);
 
  Flash_Write_Address = APPLICATION_ADDRESS_BACKUP;    
  /* initiate the write transaction by sending the first ack */
  IAP_tftp_send_ack_packet(handle, to, to_port, args->block);

  return 0;
}

/**
  * @brief  Processes traffic received on UDP port 69
  * @param  handle: pointer on connection handler
  * @param  pbuf: pointer on packet buffer
  * @param  addr: pointer on the receive IP address
  * @param  port: receive port number
  * @retval None
  */
static void IAP_tftp_recv(void *handle, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
    tftp_opcode op;
    err_t err;
    
    conn_data = netconn_new(NETCONN_UDP);
    netconn_set_recvtimeout(conn_data,CONN_RECV_TIMEOUT);
    if(!conn_data)
    {
        return;
    }
    /* bind to port 0 to receive next available free port */
    /* NOTE:  This is how TFTP works.  There is a UDP PCB for the standard port
    * 69 which al transactions begin communication on, however, _all_ subsequent
    * transactions for a given "stream" occur on another port  */
    err = netconn_bind(conn_data, IP_ADDR_ANY, 0);   
    if(err != ERR_OK)
    {
        netconn_delete(conn);
        return ;
    }    
    
    op = IAP_tftp_decode_op(p->payload);
    if (op != TFTP_WRQ)
    {
        netconn_delete(conn);
    }
    else
    {
        IAP_tftp_process_write(conn_data, addr, port);
    }
    pbuf_free(p);
}

static void vTaskTftpd(void *arg)
{
    err_t err;
    struct netbuf *buf;

    unsigned port = 69; /* 69 is the port used for TFTP protocol initial transaction */

    /* create a new UDP PCB structure  */
    conn = netconn_new(NETCONN_UDP);
    netconn_set_recvtimeout(conn,CONN_RECV_TIMEOUT);

    netconn_bind(conn,IP_ADDR_ANY,port);
        
    do
    {
        err = netconn_recv(conn,&buf);
        if(err == ERR_OK)
        {
            IAP_tftp_recv(conn,buf->p,&buf->addr,buf->port);
        }
        
        if(conn_data != NULL)
        {
            err = netconn_recv(conn_data,&buf);
            if(err == ERR_OK)
            {
                IAP_wrq_recv(conn_data,buf->p,&buf->addr,buf->port);
            }
        }
        if(buf != NULL)
        {
            netbuf_delete(buf);
        } 
    }while(1);
}

/**
  * @brief  disconnect and close the connection 
  * @param  handle: pointer on connection handler
  * @param  args: pointer on tftp_connection arguments
  * @retval None
  */
static void IAP_tftp_cleanup_wr(void *handle, tftp_connection_args *args)
{
  /* Free the tftp_connection_args structure */
  vPortFree(args);

  /* Disconnect the udp_pcb */
  netconn_disconnect(handle);
  
  /* close the connection */
  netconn_delete(handle);
  
//  /* reset the callback function */
//  udp_recv(handle, IAP_tftp_recv_callback, NULL);
 
}
/* Exported functions --------------------------------------------------------*/

void IAP_tftpd_init(void)
{
    BaseType_t status;
   
    status = xTaskCreate((TaskFunction_t)vTaskTftpd,"tftp_iap",TFTP_STACK_SIZE,NULL,TFTP_THREAD_PRIO,&handle);
    if(status != pdPASS)
    {
        
    }   
}


