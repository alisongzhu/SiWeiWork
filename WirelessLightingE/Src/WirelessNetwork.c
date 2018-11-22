/*
 * wireless.c
 *
 *  Created on: 6 15, 2018
 *      Author: PT
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <stdlib.h>
#include <string.h>
#include "mb.h"
#include "mbport.h"
#include "mbutils.h"
#include "mbrtu.h"
#include "flash.h"
#include "stdio.h"

#include "device.h"
#include "port.h"

/* Private macros ------------------------------------------------------------*/
#define  GATEWAY_JOIN_REQUEST_FRAME   0xF1
#define  GATEWAY_JOIN_ACK_FRAME       0xF2
#define  ROUTER_JOIN_REQUEST_FRAME    0xF3
#define  ROUTER_JOIN_ACK_FRAME        0xF4
#define  ROUTER_HEART_BEAT_FRAME      0xF5
#define  ROUTER_HEART_BEAT_ACK_FRAME  0xF6
#define Join_Ack_Delay                3000  
#define ROUTER_HEARTBEAT_TIME         (10*60*1000)
#define ROUTER_HEARTBEAT_TIMEAGAIN     (12*60*1000)
#define ROUTER_HEARTBEAT_TIMEOUT      (15*60*1000)
#define ROUTER_HEARTBEATACK_TIMEOUT   (20*60*1000)
#define ROUTER_HEART_BEAT             100//(10*60*1000)
#define MAX_ROUTER_NUM                3
#define Retry_Couter_TO_GAY_NUM       10
#define Retry_Couter_TO_ROUTER_NUM    1


#define Join_Ack_Delay         3000  
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
WPANStateType WPANState=WPAN_READY;	
uint8_t  WireLess_NET_DATA[3] ;/*入网数据*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void WirelessSend(uint8_t * data,uint8_t size)
{
	HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle, data, size);
	HAL_UART_Transmit_DMA(RTU_Device.BusUsartHandle, data, size);
}

/* USER CODE BEGIN 4 */
/*添加从机地址到路由表*/
uint8_t Router_List_Addr_Add(uint8_t  Addr )
{
    Router_list *Router_Node;//,*New_Node;
	Router_Node=&RTU_Device.Router_Node_Head;

	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(Router_Node->Node_addr==Addr)
		 return TRUE;
		if(Router_Node->Router_next==NULL)/*链表尾部插入*/
		{
		  Router_Node->Router_next=(Router_list*)malloc(sizeof( Router_list));
			Router_Node->Router_next->Router_next=NULL;
			Router_Node->Router_next->Node_addr=Addr;
			Router_Node->Router_next->HeartBeatTime=HAL_GetTick();
		  Router_Node->Router_next->HeartBeatflag=HEARTBEAR_FIRST;
			return TRUE;
		
		}
			
	}
//	if(Router_Node==NULL)
//		 {
//			if (	(Router_Node=(struct Router_list*)malloc(sizeof(struct Router_list)))!=NULL)
//			{  //Router_Node=New_Node;
//				aa=(uint32_t )Router_Node;
//			  Router_Node->Router_next=NULL;
//			  Router_Node->Node_addr=Addr;
//			  Router_Node->HeartBeatTime=HAL_GetTick();
//			  Router_Node->HeartBeatflag=HEARTBEAR_FIRST;
//				aa++;
//			}
//			  return TRUE;
//		 }
	return FALSE;

}
/*查找对应地址是否在路由表中*/
uint8_t Router_List_Addr_Find(uint8_t  Addr )
{
    Router_list *Router_Node;
	Router_Node=&RTU_Device.Router_Node_Head;
	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(Router_Node->Node_addr==Addr)
		 return TRUE;
		
			
	}

	return FALSE;

}
/*Data is distributed to the child nodes*/
void DataDistributionToChilds(void)
{
    Router_list *Router_Node;
	USHORT          usCRC16;
	Router_Node=RTU_Device.Router_Node_Head.Router_next;
    uint8_t *buffer=(RTU_Device.Rx_Buffer.ucRTUBuf);
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		 
        buffer[MB_SER_PDU_ADDR_OFF]=Router_Node->Node_addr;
        usCRC16 = usMBCRC16( ( UCHAR * ) buffer, RTU_Device.Rx_Buffer.size-2 );
        buffer[RTU_Device.Rx_Buffer.size-2] = ( UCHAR )( usCRC16 & 0xFF );
        buffer[RTU_Device.Rx_Buffer.size-1] = ( UCHAR )( usCRC16 >> 8 );

        HAL_UART_Transmit(RTU_Device.WirelessUsartHandle,buffer,RTU_Device.Rx_Buffer.size,250);
			
	}

	 
}
/*删除对应地址的路由表*/
uint8_t Router_List_Addr_Delete(uint8_t  Addr )
{
    Router_list *Router_Node,*Node=NULL;
	Router_Node=&RTU_Device.Router_Node_Head;
	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
	if(Router_Node->Router_next!=NULL)
		if(Router_Node->Router_next->Node_addr==Addr)
		{
			Node=Router_Node->Router_next;
		 	Router_Node->Router_next=Router_Node->Router_next->Router_next;
			free(Node);
			return TRUE;
		}
	 
			
	}

	return FALSE;

}
/*收到心跳确认，更新刷新时间*/
uint8_t Router_List_HeartBeat_Flash(uint8_t  Addr )
{
    Router_list *Router_Node=NULL;
	Router_Node=&RTU_Device.Router_Node_Head;
	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(Router_Node->Node_addr==Addr)
		{
			Router_Node->HeartBeatTime=HAL_GetTick();
			Router_Node->HeartBeatflag=HEARTBEAR_FIRST;
		
		
		 return TRUE;
		}
			
	}

	return FALSE;

}
/*检查时间是否心跳时间到，发送心跳包，有一次心跳失败重试机制，重试超时删除节点*/
uint8_t HeartBeat_Poll(void)
{
    Router_list *Router_Node=NULL;
	Router_Node=&RTU_Device.Router_Node_Head;
	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(HAL_GetTick()>Router_Node->HeartBeatTime&&Router_Node->Node_addr!=0)
		{
				if(HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIME)//心跳时间到了
				{
					
					if(Router_Node->HeartBeatflag==HEARTBEAR_FIRST)/*发送心跳包*/
					{
						 WireLess_NET_DATA[0]=ROUTER_HEART_BEAT_FRAME;
						 WireLess_NET_DATA[1]=RTU_Device.Dev_config_data.Switch_Adrr;
						 WireLess_NET_DATA[2]=Router_Node->Node_addr;

						 WirelessSend( WireLess_NET_DATA, 3 );
						 Router_Node->HeartBeatflag=HEARTBEAR_AGAIN;
						 return TRUE;
					}
					/*超时重发*/
					else if((HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIMEAGAIN)&&Router_Node->HeartBeatflag==HEARTBEAR_AGAIN)
					{
						 WireLess_NET_DATA[0]=ROUTER_HEART_BEAT_FRAME;
						 WireLess_NET_DATA[1]=RTU_Device.Dev_config_data.Switch_Adrr;
						 WireLess_NET_DATA[2]=Router_Node->Node_addr;

						 WirelessSend( WireLess_NET_DATA, 3 );
						 Router_Node->HeartBeatflag=HEARTBEAR_OUT;
						 return TRUE;
					
					}
					/*重试超时删除节点*/
				else if((HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIMEOUT)&&Router_Node->HeartBeatflag==HEARTBEAR_OUT)
					{
							Router_List_Addr_Delete(Router_Node->Node_addr);
					   
					}
						
				}	
			}
    	else 
		{
		  Router_Node->HeartBeatTime=HAL_GetTick();/*当滴答计时器值重置时，重新赋值*/
		}		
	
	}

	return FALSE;


}
/* Exported functions --------------------------------------------------------*/
WPANStateType WPAN_Startup(void)
{
    uint32_t tickstart=0;
    uint8_t  ucMBAddress= RTU_Device.Dev_config_data.Switch_Adrr;
    static uint8_t Retry_Couter=0;
    switch(WPANState)
		{
		  case WPAN_READY:
			     WPANState=WPAN_GatewayJoing;/*开机尝试网关加入*/
			     HAL_Delay(ucMBAddress%10*100+1);//
			     break;
						 
		 case WPAN_GatewayJoing:/*网关加入流程，发送网关请求帧，等待网关同意*/
			      WireLess_NET_DATA[0]=GATEWAY_JOIN_REQUEST_FRAME;
			      WireLess_NET_DATA[1]=ucMBAddress;
			      WireLess_NET_DATA[2]=~ucMBAddress;
			      //vMBPortSerialEnable(  FALSE ,TRUE );
			     // HAL_IWDG_Refresh(&hiwdg);
			    //  HAL_Delay(500*(rand()%RANDOMNUM));
			      WirelessSend( WireLess_NET_DATA, 3 );
                  tickstart = HAL_GetTick();
                   while((HAL_GetTick() - tickstart) < Join_Ack_Delay*(Retry_Couter+1))
                    {
                    		//拷贝入网帧todo
                    		 /*收到网关确认帧，设置路由级数，进入入网成功*/
                        if(WireLess_NET_DATA[0]==GATEWAY_JOIN_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress&&!(WireLess_NET_DATA[2]&ucMBAddress))
                    	{
            				WPANState=WPAN_Succede;
            				RTU_Device.Dev_config_data.Router_Level=Router_Primary;
            				Retry_Couter=0;
            				break;
                        }
                    }
                    
				 if(WPANState==WPAN_Succede)
					 break;
				 else if(++Retry_Couter==Retry_Couter_TO_GAY_NUM)/*尝试加入网关次数到了，进入路由入网状态*/
				 {
				    //HAL_Delay(100*(rand()%RANDOMNUM));
				   WPANState=WPAN_RouterJoing;
				   Retry_Couter=0;
				 }
				 else 
					 break;
			case  WPAN_RouterJoing:/*路由入网，发送路由入网帧，等待路由节点确认*/
				  WireLess_NET_DATA[0]=ROUTER_JOIN_REQUEST_FRAME;
			      WireLess_NET_DATA[1]=ucMBAddress;
			      WireLess_NET_DATA[2]=~ucMBAddress;

			      WirelessSend( WireLess_NET_DATA, 3 );
			      tickstart = HAL_GetTick();
				while((HAL_GetTick() - tickstart) < Join_Ack_Delay)
				 {

					 if(WireLess_NET_DATA[0]==ROUTER_JOIN_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)/*如果收到路由返回就发送入网确认帧*/
					 {

							RTU_Device.Host_Node.Host_Addr=WireLess_NET_DATA[2];/*保存父节点地址*/
							WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
							WireLess_NET_DATA[1]=RTU_Device.Host_Node.Host_Addr;
							WireLess_NET_DATA[2]=ucMBAddress;

							//HAL_Delay(500*(rand()%RANDOMNUM));
							WirelessSend( WireLess_NET_DATA, 3 );
							WPANState=WPAN_RouterACK;/*进入父节点确认状态*/
							Retry_Couter=0;
							break;
					 }
				  }
				 if(WPANState==WPAN_RouterACK)
				   break;
				 else if(++Retry_Couter==Retry_Couter_TO_ROUTER_NUM)/*尝试次数*/
				 {
					 WPANState=WPAN_READY;
					 Retry_Couter=0;
				//	 HAL_Delay(500*(rand()%RANDOMNUM));
				 }

						
				     
      case WPAN_Succede:
		   WireLess_NET_DATA[0]=0;/*清除帧头*/
		if(RTU_Device.Dev_config_data.Router_Level==Router_Primary)/*路由节点支持路由功能*/
			{
			   if(RTU_Device.ChildNodeNum)/*有从节点*/
				 {
					 HeartBeat_Poll();/*发送心跳包*/
				   if(WireLess_NET_DATA[0]==ROUTER_HEART_BEAT_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)/*收到心跳包返回，刷新对应节点心跳时间*/
					{
					  Router_List_HeartBeat_Flash(WireLess_NET_DATA[2] );
					}
				  }
				 /*收到路由入网请求，发送响应帧，子节点有个数限制*/
					if(RTU_Device.Dev_config_data.Router_Level==Router_Primary&&WireLess_NET_DATA[0]==ROUTER_JOIN_REQUEST_FRAME&&!(WireLess_NET_DATA[1]&WireLess_NET_DATA[2])&&RTU_Device.ChildNodeNum<MAX_ROUTER_NUM)
					 {
						 WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
						// WireLess_NET_DATA[1]=ucMBAddress;
						 WireLess_NET_DATA[2]=ucMBAddress;

						 //HAL_Delay(300*(rand()%RANDOMNUM));
						 WirelessSend( WireLess_NET_DATA, 3 );
						 break ;
					 }

					 /*收到子节点入网确认帧，加入路由表，发送入网确认帧*/
					 if(RTU_Device.Dev_config_data.Router_Level==Router_Primary&&WireLess_NET_DATA[0]==ROUTER_JOIN_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)
					 {

						 if( Router_List_Addr_Add(WireLess_NET_DATA[2]))
						 {
								RTU_Device.ChildNodeNum++;/*子节点数量加一*/
								WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
								// WireLess_NET_DATA[1]=ucMBAddress;
								//WireLess_NET_DATA[2]=ucMBAddress;

								//HAL_Delay(300*(rand()%RANDOMNUM));
								WirelessSend( WireLess_NET_DATA, 3 );

						 }
					 }
				  }
					else if( RTU_Device.Dev_config_data.Router_Level==Router_secondary)/*二级路由*/
					{

						/*收到心跳包返回确认帧，更新时间*/
							if(WireLess_NET_DATA[0]==ROUTER_HEART_BEAT_FRAME&&WireLess_NET_DATA[1]== RTU_Device.Host_Node.Host_Addr&&WireLess_NET_DATA[2]==ucMBAddress)
							{
								 WireLess_NET_DATA[0]=ROUTER_HEART_BEAT_ACK_FRAME;
							   WireLess_NET_DATA[2]=ucMBAddress;
							   WireLess_NET_DATA[1]=RTU_Device.Host_Node.Host_Addr;
							   WirelessSend( WireLess_NET_DATA, 3 );
								 RTU_Device.Host_Node.HeartBeatTime=HAL_GetTick();
							
							}
							if(HAL_GetTick()>RTU_Device.Host_Node.HeartBeatTime)
							{
									/*没有收到心跳包，心跳超时，重新发送路由入网确认帧*/
									if((HAL_GetTick() - RTU_Device.Host_Node.HeartBeatTime >ROUTER_HEARTBEATACK_TIMEOUT))
									{
											WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
											WireLess_NET_DATA[1]= RTU_Device.Host_Node.Host_Addr;
											WireLess_NET_DATA[2]=ucMBAddress;

											WirelessSend( WireLess_NET_DATA, 3 ); 
											WPANState=WPAN_RouterACK;								
									}
						  }
							else
								RTU_Device.Host_Node.HeartBeatTime=HAL_GetTick();
					}
  			   break;	
			case  WPAN_RouterACK:	
             tickstart = HAL_GetTick();
             while((HAL_GetTick() - tickstart) < Join_Ack_Delay)
             {
				 /*收到父节点入网确认帧，记录入网时间，进入入网成功状态*/
				 if(WireLess_NET_DATA[0]==ROUTER_JOIN_ACK_FRAME&&WireLess_NET_DATA[2]==ucMBAddress&&WireLess_NET_DATA[1]==RTU_Device.Host_Node.Host_Addr )
				 {
					  WPANState=WPAN_Succede;
					  RTU_Device.Host_Node.HeartBeatTime=HAL_GetTick();
					  RTU_Device.Dev_config_data.Router_Level=Router_secondary;
					  break;
				  }
			  }
				     if(WPANState==WPAN_Succede)
							 break;
				     else
					 {WPANState=WPAN_RouterJoing;}/*重新加入路由*/
					break;
		
		}
	return WPANState;

}
/**
  * @}
  */

  /**
	* @}
	*/

/*
 * wireless.c
 *
 *  Created on: 6 15, 2018
 *      Author: PT
 */

/* Includes ------------------------------------------------------------------*/


/* Private macros ------------------------------------------------------------*/


/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @}
  */

  /**
	* @}
	*/

