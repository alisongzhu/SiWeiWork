/*
 * FreeModbus Libary: ARM7/AT91SAM7X Port
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
 * File: $Id: portevent.c,v 1.1 2007/09/12 10:15:56 wolti Exp $
 */
/* ----------------------- System includes ----------------------------------*/
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
//#include "FreeRTOS.h"
//#include "task.h"
//#include "queue.h"
#include "cmsis_os.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- Type definitions ---------------------------------*/
//static xQueueHandle xQueueHdl;
static osMessageQId MsgModbus;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    BOOL            bStatus = FALSE;
    osMessageQDef(MsgModbus, 1, eMBEventType);      
    MsgModbus = osMessageCreate(osMessageQ(MsgModbus),NULL );
    if( NULL != MsgModbus)
    {
        bStatus = TRUE;
    }
    return bStatus;
}

void
vMBPortEventClose( void )
{
    if( NULL != MsgModbus )
    {
        osMessageDelete( MsgModbus );
        MsgModbus = NULL;
    }
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    BOOL            bStatus = FALSE;
    uint32_t  tmp ;
    tmp = ( uint32_t )eEvent;
    if(tmp == 1)
    {
        printf("fuck!\n");
    }
    if( osOK ==osMessagePut(MsgModbus,tmp,osWaitForever /*portTICK_RATE_MS * 50*/)) // maybe the timeout should be osWaitForever
    {
        bStatus = TRUE;
    } 
    return bStatus;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    BOOL            xEventHappened = FALSE;
    osEvent evt;
    evt = osMessageGet(MsgModbus,osWaitForever);
    if(osEventMessage == evt.status)
    {       
        *eEvent = (eMBEventType)evt.value.v; 
        xEventHappened = TRUE;
    }
    return xEventHappened;
}

