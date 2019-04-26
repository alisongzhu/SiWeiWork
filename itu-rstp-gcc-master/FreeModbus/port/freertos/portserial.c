/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "usart.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static uint8_t slave_rx;
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if(  xRxEnable) {
//		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE)  ;
    //����485���ͽ��տ��ƹܽ�Ϊ����ģʽ
        HAL_GPIO_WritePin(RS485_USART1_T_R_GPIO_Port,RS485_USART1_T_R_Pin, GPIO_PIN_RESET);
        HAL_UART_Receive_IT(&huart1,&slave_rx,1);
    }
	else  {
//		__HAL_UART_DISABLE_IT(&huart1,UART_IT_RXNE);
		//����485���ͽ��տ��ƹܽ�Ϊ����ģʽ
        HAL_GPIO_WritePin(RS485_USART1_T_R_GPIO_Port,RS485_USART1_T_R_Pin, GPIO_PIN_SET); 
	}
  
	if ( xTxEnable)	 
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_TXE)  ;
    else    
		__HAL_UART_DISABLE_IT(&huart1,UART_IT_TXE);
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{     
    return BSP_USART1_UART_Init(ulBaudRate,ucDataBits,eParity);
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    
    if(  HAL_UART_Transmit_IT(&huart1,(uint8_t *)&ucByte,1) != HAL_OK) 
        return FALSE;
    else 
        return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = slave_rx;
    if ( HAL_UART_Receive_IT(&huart1,&slave_rx,1) != HAL_OK ) 
        return FALSE;
    else 
        return TRUE;    
    
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */


/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */

