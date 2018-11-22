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
uint8_t  WireLess_NET_DATA[3] ;/*��������*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void WirelessSend(uint8_t * data,uint8_t size)
{
	HAL_UART_Transmit_DMA(RTU_Device.WirelessUsartHandle, data, size);
	HAL_UART_Transmit_DMA(RTU_Device.BusUsartHandle, data, size);
}

/* USER CODE BEGIN 4 */
/*��Ӵӻ���ַ��·�ɱ�*/
uint8_t Router_List_Addr_Add(uint8_t  Addr )
{
    Router_list *Router_Node;//,*New_Node;
	Router_Node=&RTU_Device.Router_Node_Head;

	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(Router_Node->Node_addr==Addr)
		 return TRUE;
		if(Router_Node->Router_next==NULL)/*����β������*/
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
/*���Ҷ�Ӧ��ַ�Ƿ���·�ɱ���*/
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
/*ɾ����Ӧ��ַ��·�ɱ�*/
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
/*�յ�����ȷ�ϣ�����ˢ��ʱ��*/
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
/*���ʱ���Ƿ�����ʱ�䵽����������������һ������ʧ�����Ի��ƣ����Գ�ʱɾ���ڵ�*/
uint8_t HeartBeat_Poll(void)
{
    Router_list *Router_Node=NULL;
	Router_Node=&RTU_Device.Router_Node_Head;
	
	for(;Router_Node!=NULL;Router_Node=Router_Node->Router_next)
	
	{
		if(HAL_GetTick()>Router_Node->HeartBeatTime&&Router_Node->Node_addr!=0)
		{
				if(HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIME)//����ʱ�䵽��
				{
					
					if(Router_Node->HeartBeatflag==HEARTBEAR_FIRST)/*����������*/
					{
						 WireLess_NET_DATA[0]=ROUTER_HEART_BEAT_FRAME;
						 WireLess_NET_DATA[1]=RTU_Device.Dev_config_data.Switch_Adrr;
						 WireLess_NET_DATA[2]=Router_Node->Node_addr;

						 WirelessSend( WireLess_NET_DATA, 3 );
						 Router_Node->HeartBeatflag=HEARTBEAR_AGAIN;
						 return TRUE;
					}
					/*��ʱ�ط�*/
					else if((HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIMEAGAIN)&&Router_Node->HeartBeatflag==HEARTBEAR_AGAIN)
					{
						 WireLess_NET_DATA[0]=ROUTER_HEART_BEAT_FRAME;
						 WireLess_NET_DATA[1]=RTU_Device.Dev_config_data.Switch_Adrr;
						 WireLess_NET_DATA[2]=Router_Node->Node_addr;

						 WirelessSend( WireLess_NET_DATA, 3 );
						 Router_Node->HeartBeatflag=HEARTBEAR_OUT;
						 return TRUE;
					
					}
					/*���Գ�ʱɾ���ڵ�*/
				else if((HAL_GetTick()-Router_Node->HeartBeatTime>ROUTER_HEARTBEAT_TIMEOUT)&&Router_Node->HeartBeatflag==HEARTBEAR_OUT)
					{
							Router_List_Addr_Delete(Router_Node->Node_addr);
					   
					}
						
				}	
			}
    	else 
		{
		  Router_Node->HeartBeatTime=HAL_GetTick();/*���δ��ʱ��ֵ����ʱ�����¸�ֵ*/
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
			     WPANState=WPAN_GatewayJoing;/*�����������ؼ���*/
			     HAL_Delay(ucMBAddress%10*100+1);//
			     break;
						 
		 case WPAN_GatewayJoing:/*���ؼ������̣�������������֡���ȴ�����ͬ��*/
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
                    		//��������֡todo
                    		 /*�յ�����ȷ��֡������·�ɼ��������������ɹ�*/
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
				 else if(++Retry_Couter==Retry_Couter_TO_GAY_NUM)/*���Լ������ش������ˣ�����·������״̬*/
				 {
				    //HAL_Delay(100*(rand()%RANDOMNUM));
				   WPANState=WPAN_RouterJoing;
				   Retry_Couter=0;
				 }
				 else 
					 break;
			case  WPAN_RouterJoing:/*·������������·������֡���ȴ�·�ɽڵ�ȷ��*/
				  WireLess_NET_DATA[0]=ROUTER_JOIN_REQUEST_FRAME;
			      WireLess_NET_DATA[1]=ucMBAddress;
			      WireLess_NET_DATA[2]=~ucMBAddress;

			      WirelessSend( WireLess_NET_DATA, 3 );
			      tickstart = HAL_GetTick();
				while((HAL_GetTick() - tickstart) < Join_Ack_Delay)
				 {

					 if(WireLess_NET_DATA[0]==ROUTER_JOIN_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)/*����յ�·�ɷ��ؾͷ�������ȷ��֡*/
					 {

							RTU_Device.Host_Node.Host_Addr=WireLess_NET_DATA[2];/*���游�ڵ��ַ*/
							WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
							WireLess_NET_DATA[1]=RTU_Device.Host_Node.Host_Addr;
							WireLess_NET_DATA[2]=ucMBAddress;

							//HAL_Delay(500*(rand()%RANDOMNUM));
							WirelessSend( WireLess_NET_DATA, 3 );
							WPANState=WPAN_RouterACK;/*���븸�ڵ�ȷ��״̬*/
							Retry_Couter=0;
							break;
					 }
				  }
				 if(WPANState==WPAN_RouterACK)
				   break;
				 else if(++Retry_Couter==Retry_Couter_TO_ROUTER_NUM)/*���Դ���*/
				 {
					 WPANState=WPAN_READY;
					 Retry_Couter=0;
				//	 HAL_Delay(500*(rand()%RANDOMNUM));
				 }

						
				     
      case WPAN_Succede:
		   WireLess_NET_DATA[0]=0;/*���֡ͷ*/
		if(RTU_Device.Dev_config_data.Router_Level==Router_Primary)/*·�ɽڵ�֧��·�ɹ���*/
			{
			   if(RTU_Device.ChildNodeNum)/*�дӽڵ�*/
				 {
					 HeartBeat_Poll();/*����������*/
				   if(WireLess_NET_DATA[0]==ROUTER_HEART_BEAT_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)/*�յ����������أ�ˢ�¶�Ӧ�ڵ�����ʱ��*/
					{
					  Router_List_HeartBeat_Flash(WireLess_NET_DATA[2] );
					}
				  }
				 /*�յ�·���������󣬷�����Ӧ֡���ӽڵ��и�������*/
					if(RTU_Device.Dev_config_data.Router_Level==Router_Primary&&WireLess_NET_DATA[0]==ROUTER_JOIN_REQUEST_FRAME&&!(WireLess_NET_DATA[1]&WireLess_NET_DATA[2])&&RTU_Device.ChildNodeNum<MAX_ROUTER_NUM)
					 {
						 WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
						// WireLess_NET_DATA[1]=ucMBAddress;
						 WireLess_NET_DATA[2]=ucMBAddress;

						 //HAL_Delay(300*(rand()%RANDOMNUM));
						 WirelessSend( WireLess_NET_DATA, 3 );
						 break ;
					 }

					 /*�յ��ӽڵ�����ȷ��֡������·�ɱ���������ȷ��֡*/
					 if(RTU_Device.Dev_config_data.Router_Level==Router_Primary&&WireLess_NET_DATA[0]==ROUTER_JOIN_ACK_FRAME&&WireLess_NET_DATA[1]==ucMBAddress)
					 {

						 if( Router_List_Addr_Add(WireLess_NET_DATA[2]))
						 {
								RTU_Device.ChildNodeNum++;/*�ӽڵ�������һ*/
								WireLess_NET_DATA[0]=ROUTER_JOIN_ACK_FRAME;
								// WireLess_NET_DATA[1]=ucMBAddress;
								//WireLess_NET_DATA[2]=ucMBAddress;

								//HAL_Delay(300*(rand()%RANDOMNUM));
								WirelessSend( WireLess_NET_DATA, 3 );

						 }
					 }
				  }
					else if( RTU_Device.Dev_config_data.Router_Level==Router_secondary)/*����·��*/
					{

						/*�յ�����������ȷ��֡������ʱ��*/
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
									/*û���յ���������������ʱ�����·���·������ȷ��֡*/
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
				 /*�յ����ڵ�����ȷ��֡����¼����ʱ�䣬���������ɹ�״̬*/
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
					 {WPANState=WPAN_RouterJoing;}/*���¼���·��*/
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

