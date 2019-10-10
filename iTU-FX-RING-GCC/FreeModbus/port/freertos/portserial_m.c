/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
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
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */

#include "port.h"
#include "usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
//ALIGN(RT_ALIGN_SIZE)
///* software simulation serial transmit IRQ handler thread stack */
//static rt_uint8_t serial_soft_trans_irq_stack[512];
///* software simulation serial transmit IRQ handler thread */
//static struct rt_thread thread_serial_soft_trans_irq;
///* serial event */
//static struct rt_event event_serial;
///* modbus master serial device */
//static rt_serial_t *serial;
/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size);
//static void serial_soft_trans_irq(void* parameter);
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBMasterPortSerialInit (UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits,
			 eMBParity eParity)
{
  return BSP_USART6_UART_Init (ulBaudRate, ucDataBits, eParity);
}

void
vMBMasterPortSerialEnable (BOOL xRxEnable, BOOL xTxEnable)
{
  if (xRxEnable)
    {
      __HAL_UART_ENABLE_IT(&huart6,UART_IT_RXNE);
      //����485���ͽ��տ��ƹܽ�Ϊ����ģʽ
      HAL_GPIO_WritePin (RS485_USART6_T_R_GPIO_Port, RS485_USART6_T_R_Pin,
			 GPIO_PIN_RESET);
    }
  else
    {
      __HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);
      //����485���ͽ��տ��ƹܽ�Ϊ����ģʽ
      HAL_GPIO_WritePin (RS485_USART6_T_R_GPIO_Port, RS485_USART6_T_R_Pin,
			 GPIO_PIN_SET);
    }

  if (xTxEnable)
    __HAL_UART_ENABLE_IT(&huart6,UART_IT_TXE);
  else
    __HAL_UART_DISABLE_IT(&huart6, UART_IT_TXE);
}

void
vMBMasterPortClose (void)
{

}

BOOL
xMBMasterPortSerialPutByte (CHAR ucByte)
{
  HAL_GPIO_WritePin (RS485_USART6_T_R_GPIO_Port, RS485_USART6_T_R_Pin,
		     GPIO_PIN_SET);
  if (HAL_UART_Transmit (&huart6, (uint8_t *) &ucByte, 1, 0xff) != HAL_OK)
    return FALSE;
  else
    return TRUE;
}

BOOL
xMBMasterPortSerialGetByte (CHAR * pucByte)
{
  HAL_GPIO_WritePin (RS485_USART6_T_R_GPIO_Port, RS485_USART6_T_R_Pin,
		     GPIO_PIN_RESET);

  if (HAL_UART_Receive (&huart6, (uint8_t *) pucByte, 1, 0xff) != HAL_OK)
    return FALSE;
  else
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
//void prvvUARTTxReadyISR(void)
//{
//    pxMBMasterFrameCBTransmitterEmpty();
//}
/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
//void prvvUARTRxISR(void)
//{
//    pxMBMasterFrameCBByteReceived();
//}
/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
//static void serial_soft_trans_irq(void* parameter) {
//    rt_uint32_t recved_event;
//    while (1)
//    {
//        /* waiting for serial transmit start */
//        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START, RT_EVENT_FLAG_OR,
//                RT_WAITING_FOREVER, &recved_event);
//        /* execute modbus callback */
//        prvvUARTTxReadyISR();
//    }
//}
///**
// * This function is serial receive callback function
// *
// * @param dev the device of serial
// * @param size the data size that receive
// *
// * @return return RT_EOK
// */
//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size) {
//    prvvUARTRxISR();
//    return RT_EOK;
//}
#endif
