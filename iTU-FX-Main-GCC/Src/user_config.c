/*
 * user_config.c
 *
 *  Created on: Mar 20 2017
 *      Author: LH
 */

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "user_config.h"
#include "string.h"
#include "socket.h"
#include "gpio.h"
#include "env.h"
#include "cmsis_os.h"
#include "tftpserver.h"
#include "flash_if.h"
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define bzero(a,b)     memset(a,0,b)

#define USER_CONFIG_PORT    50000

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
#define SYSWARE_MSG_HEADER_SIZE      12

#define USER_MSG_BUF_SIZE   1024

/* Private constants ---------------------------------------------------------*/
static const uint16_t Coil_Pin[INTERNAL_COIL_COUNT] = {
        DO1_Pin,
        DO2_Pin,
        DO3_Pin,
        DO4_Pin,
        DO5_Pin,
        DO6_Pin,
        DO7_Pin,
        DO8_Pin,
        DO9_Pin,
        DO10_Pin,
        DO11_Pin,
        DO12_Pin
};

static GPIO_TypeDef* Coil_Port[INTERNAL_COIL_COUNT] = {
        DO1_GPIO_Port,
        DO2_GPIO_Port,
        DO3_GPIO_Port,
        DO4_GPIO_Port,
        DO5_GPIO_Port,
        DO6_GPIO_Port,
        DO7_GPIO_Port,
        DO8_GPIO_Port,
        DO9_GPIO_Port,
        DO10_GPIO_Port,
        DO11_GPIO_Port,
        DO12_GPIO_Port
};
/* Private variables ---------------------------------------------------------*/
static int socket_udp;
static in_port_t peer_port;
static uint8_t user_msgbuf[USER_MSG_BUF_SIZE] = {0};
static osThreadId taskConfigHandle;
/* Global variables ----------------------------------------------------------*/
extern uint32_t UID[3];
extern __IO DiscreteInfo InternalDiscreteInfo[INTERNAL_DISCRETE_COUNT];

/* Private function prototypes -----------------------------------------------*/
static uint8_t parseUDPMSG(void);

static void send_discover_ack(void);
static void save_config(void);
static void send_supervisory_ack(void);

static void process_control_msg(void);
static void process_reset_msg(void);
static void process_readprop_msg(void);
static void process_restore_msg(void);

static void protocol_init(void);
static void protocol_run(void);
static void vTaskUserConfig(void const * argument);

/* Private functions ---------------------------------------------------------*/
static uint8_t parseUDPMSG(void)
{
	uint8_t type = MSG_TYPE_UNKNOWN;
  
	uint16_t size = 0;
	socklen_t peerAddr_len;
  
	struct sockaddr_in peerAddr;
 
	peerAddr_len=sizeof(peerAddr);

	size = recvfrom(socket_udp,user_msgbuf,sizeof(user_msgbuf),0,(struct sockaddr *)&peerAddr,&peerAddr_len);
	peer_port = peerAddr.sin_port;
	if(size >= (SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE))
    {
//        if((memcmp((char *)user_msgbuf,(char*)user_msg_header,SYSWARE_MSG_HEADER_SIZE) == 0)
//            ||(memcmp((char *)user_msgbuf,(char *)UID,SYSWARE_MSG_HEADER_SIZE) == 0))
        type = user_msgbuf[SYSWARE_MSG_HEADER_SIZE];

    }
    return type;
}

static void send_discover_ack(void)
{   
    struct sockaddr_in peerAddr;
    socklen_t peerAddr_len; 
    bzero((void *)&peerAddr,sizeof(peerAddr));
    
    peerAddr.sin_family=AF_INET;  
    peerAddr.sin_addr.s_addr=htonl(IPADDR_BROADCAST);  
    peerAddr.sin_port=peer_port;
    
    peerAddr_len=sizeof(peerAddr);
    uint8_t tmp = MSG_TYPE_DISCOVER;
    memcpy(user_msgbuf,UID,sizeof(UID));
    memcpy(user_msgbuf+sizeof(UID),&tmp,sizeof(tmp));
//    memcpy(user_msgbuf+sizeof(UID),&env,sizeof(env));
    sendto(socket_udp,user_msgbuf,sizeof(UID)+sizeof(tmp),0,(struct sockaddr *)&peerAddr, peerAddr_len);
}

static void save_config(void)
{
    //check UID 
    if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0))
    {
//		memcpy(&env,user_msgbuf + SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE,sizeof(env));
//		SystemEnv.config_flag = 0;		
			FLASH_If_Init();
			if(FLASHIF_OK == FLASH_If_Erase(USER_ENV_ADDRESS,USER_ENV_SECTORS))
			{
				user_env_address = USER_ENV_ADDRESS;
				if(FLASHIF_OK ==FLASH_If_Write(&user_env_address,(uint32_t*)&user_msgbuf[SYSWARE_MSG_HEADER_SIZE+MSG_TYPE_SIZE],sizeof(env)/4))
				{
					SystemEnv.config_flag = 0;
				}
				else SystemEnv.config_flag = 1;
			}
//			sys_env_save();
	    if(0 == sys_env_save())
	    {
				//TODO: NEED JUMP TO BOOTLOADER
				HAL_NVIC_SystemReset();
	    }
	    else{
		  //TODO: send erasing error message back to client
	    }
    }
}

static void send_supervisory_ack(void)
{
    if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0)){
        struct sockaddr_in peerAddr;
        socklen_t peerAddr_len;
        bzero((void *)&peerAddr,sizeof(peerAddr));

        peerAddr.sin_family=AF_INET;
        peerAddr.sin_addr.s_addr=htonl(IPADDR_BROADCAST);
        peerAddr.sin_port=peer_port;

        peerAddr_len=sizeof(peerAddr);
//        uint8_t tmp = MSG_TYPE_SUPERVISORY;
//        memcpy(user_msgbuf,UID,sizeof(UID));
//        memcpy(user_msgbuf+sizeof(UID),&tmp,sizeof(tmp));
        uint8_t *p,i;
        p = user_msgbuf + SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE;
        for(i=0;i<INTERNAL_DISCRETE_COUNT;i++){
            *p++ = InternalDiscreteInfo[i].val;
        }
    
        sendto(socket_udp,user_msgbuf,(SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE+INTERNAL_DISCRETE_COUNT),0,(struct sockaddr *)&peerAddr, peerAddr_len);
    }
}

static void process_control_msg(void)
{
    if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0)){
        uint8_t *p;
        p = user_msgbuf;
        p += SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE;
    
        env.BasicEnv.CoilOutputMode = COIL_MODE_DIRECT;
        HAL_GPIO_WritePin(SELECT_DO_GPIO_Port,SELECT_DO_Pin,GPIO_PIN_SET);
        HAL_Delay(100);
        uint8_t i;
        for(i=0;i<INTERNAL_COIL_COUNT;i++)
        {
            if(*(p+i))
            {
              ExecuteInternalCoilOutputON(i);
            }
            else 
              ExecuteInternalCoilOutputOFF(i);
        }
    }
}

static void process_reset_msg(void)
{
    if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0)){
        HAL_NVIC_SystemReset();
    }
}

static void process_readprop_msg(void)
{
    if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0))
    {
        //
        struct sockaddr_in peerAddr;
        socklen_t peerAddr_len;
        bzero((void *)&peerAddr,sizeof(peerAddr));

        peerAddr.sin_family=AF_INET;
        peerAddr.sin_addr.s_addr=htonl(IPADDR_BROADCAST);
        peerAddr.sin_port=peer_port;

        peerAddr_len=sizeof(peerAddr);
        
        memcpy(user_msgbuf+SYSWARE_MSG_HEADER_SIZE+ MSG_TYPE_SIZE,&env,sizeof(env));
        sendto(socket_udp,user_msgbuf,(SYSWARE_MSG_HEADER_SIZE + MSG_TYPE_SIZE + sizeof(env)),0,(struct sockaddr *)&peerAddr, peerAddr_len);
    }    
}

static void process_restore_msg(void)
{
	if((memcmp((char *)user_msgbuf,(char *)UID,sizeof(UID)) == 0))
	{
		SystemEnv.config_flag = 0xff;
		sys_env_save();
		HAL_NVIC_SystemReset();
	}
}

static void protocol_init(void)
{  
  if((socket_udp = socket(AF_INET,SOCK_DGRAM,0)) >= 0)
  {
    struct sockaddr_in svrAddr;  
    bzero((void *)&svrAddr,sizeof(svrAddr));  
  
    svrAddr.sin_family=AF_INET;  
    svrAddr.sin_addr.s_addr=htonl(INADDR_ANY);  
    svrAddr.sin_port=htons(USER_CONFIG_PORT);   
    
    if(bind(socket_udp,(struct sockaddr *)&svrAddr,sizeof(svrAddr)) >= 0){

    }
  }  
  memset(user_msgbuf,0,sizeof(user_msgbuf));   // clear user_msgbuf
}

static void protocol_run(void)
{
	uint8_t type=0;
	type = parseUDPMSG();
	switch (type)
	{
	case MSG_TYPE_UNKNOWN:
		break;
	case MSG_TYPE_DISCOVER:
		send_discover_ack();
		break;
	case MSG_TYPE_CONFIG:
		save_config();
		break;
	case MSG_TYPE_SUPERVISORY:
		send_supervisory_ack();
		break;
	case MSG_TYPE_DIRECT_CONTROL:
		process_control_msg();
		break;
	case MSG_TYPE_RESET:
		process_reset_msg();
		break;
	case MSG_TYPE_READ_PROP:
		process_readprop_msg();
		break;
	case MSG_TYPE_UPGRADE:
		IAP_tftpd_init();
		break;
	case MSG_TYPE_RESTORE:
		process_restore_msg();
		break;
	default:
		break;
	}
}

static void vTaskUserConfig(void const * argument)
{
  protocol_init();
  for(;;)
  {      
      protocol_run();
  } 
}
/* Exported functions --------------------------------------------------------*/
void UserConfig_init(void)
{
    osThreadDef(UserConfig, vTaskUserConfig, osPriorityNormal, 0, 128);
    taskConfigHandle = osThreadCreate(osThread(UserConfig), NULL);
}


void ExecuteInternalCoilOutputOFF(uint8_t CoilNo)
{
    HAL_GPIO_WritePin(Coil_Port[CoilNo], Coil_Pin[CoilNo], GPIO_PIN_RESET);
}

void ExecuteInternalCoilOutputON(uint8_t CoilNo)
{
    HAL_GPIO_WritePin(Coil_Port[CoilNo], Coil_Pin[CoilNo], GPIO_PIN_SET);
}
