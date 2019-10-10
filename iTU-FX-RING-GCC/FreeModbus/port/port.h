/*
 * FreeModbus Libary: STM32F407 + Freertos Port
 * Copyright (C) 2017 LH <alien.whu@gmail.com>
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
 * File: $Id: port.h ,v 1.60 2017/08/07 11:07:05 LH add Master Functions $
 */

#ifndef _PORT_H
#define _PORT_H

#include "stm32f4xx_hal.h"
#include "mbconfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>
#include <assert.h>

/* ----------------------- Defines ------------------------------------------*/
#define	INLINE                      inline
#define STATIC                      static
    
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define ENTER_CRITICAL_SECTION( )   vMBPortEnterCritical()
#define EXIT_CRITICAL_SECTION( )    vMBPortExitCritical()

//#define assert( x )

typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifndef TRUE
#define TRUE                                    1
#endif

#ifndef FALSE
#define FALSE                                   0
#endif
/* ----------------------- Prototypes ---------------------------------------*/
//void            vMBPortSetWithinException( BOOL bInException );
//BOOL            bMBPortIsWithinException( void );

void            vMBPortEnterCritical( void );
void            vMBPortExitCritical( void );

#endif
