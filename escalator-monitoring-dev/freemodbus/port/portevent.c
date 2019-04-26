/*
 * FreeModbus Libary: lwIP Port
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
 * File: $Id$
 */

/* ----------------------- lwIP ---------------------------------------------*/
#include "lwip/api.h"
#include "lwip/sys.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_POLL_CYCLETIME       100     /* Poll cycle time is 100ms */

#define MB_MBOX_SIZE               ( 16 )
/* ----------------------- Static variables ---------------------------------*/
static sys_mbox_t xMailBox = {
    .mbx = NULL,
};
static eMBEventType eMailBoxEvent;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    eMailBoxEvent = EV_READY;

    return ERR_OK == sys_mbox_new(&xMailBox, MB_MBOX_SIZE ) ? TRUE : FALSE;
}

void
vMBPortEventClose( void )
{
    if( sys_mbox_valid(&xMailBox))
    {
        sys_mbox_free(&xMailBox );
    }
}

BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    eMailBoxEvent = eEvent;
    sys_mbox_post( &xMailBox, &eMailBoxEvent );
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    void           *peMailBoxEvent;
    BOOL            xEventHappend = FALSE;
    u32_t           uiTimeSpent;

    uiTimeSpent = sys_arch_mbox_fetch( &xMailBox, &peMailBoxEvent, MB_POLL_CYCLETIME );
    if( uiTimeSpent != SYS_ARCH_TIMEOUT )
    {
        *eEvent = *( eMBEventType * ) peMailBoxEvent;
        eMailBoxEvent = EV_READY;
        xEventHappend = TRUE;
    }
    return xEventHappend;
}
