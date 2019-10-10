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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "tim.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"

#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;

/* ----------------------- static functions ---------------------------------*/
void
prvvMasterTIMERExpiredISR (void);

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBMasterPortTimersInit (USHORT usTimeOut50us)
{
  /* backup T35 ticks */
//    usT35TimeOut50us = usTimeOut50us;
//
//    TIM_MasterConfigTypeDef sMasterConfig;
//
//    htim7.Instance = TIM7;
//    htim7.Init.Prescaler = T35_PSC;
//    htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
//    htim7.Init.Period = usTimeOut50us;
//    if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
//    {
//      _Error_Handler(__FILE__, __LINE__);
//    }
//
//    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
//    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
//    if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
//    {
//      _Error_Handler(__FILE__, __LINE__);
//    }
  return TRUE;
}

void
vMBMasterPortTimersT35Enable ()
{
  /* Set current timer mode, don't change it.*/
  vMBMasterSetCurTimerMode (MB_TMODE_T35);

  __HAL_TIM_SET_AUTORELOAD(&htim7, usT35TimeOut50us);

  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
  __HAL_TIM_SetCounter(&htim7, 0);

  HAL_TIM_Base_Start_IT (&htim7);
}

void
vMBMasterPortTimersConvertDelayEnable ()
{
  /* Set current timer mode, don't change it.*/
  vMBMasterSetCurTimerMode (MB_TMODE_CONVERT_DELAY);

  __HAL_TIM_SET_AUTORELOAD(&htim7, MB_MASTER_DELAY_MS_CONVERT*20);

  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
  __HAL_TIM_SetCounter(&htim7, 0);

  HAL_TIM_Base_Start_IT (&htim7);
}

void
vMBMasterPortTimersRespondTimeoutEnable ()
{
  /* Set current timer mode, don't change it.*/
  vMBMasterSetCurTimerMode (MB_TMODE_RESPOND_TIMEOUT);

  __HAL_TIM_SET_AUTORELOAD(&htim7, MB_MASTER_TIMEOUT_MS_RESPOND*20);

  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
  __HAL_TIM_SetCounter(&htim7, 0);

  HAL_TIM_Base_Start_IT (&htim7);
}

void
vMBMasterPortTimersDisable ()
{
  HAL_TIM_Base_Stop_IT (&htim7);

  __HAL_TIM_SetCounter(&htim7,0);
  __HAL_TIM_CLEAR_IT(&htim7, TIM_IT_UPDATE);
}

void
prvvMasterTIMERExpiredISR (void)
{
  (void) pxMBMasterPortCBTimerExpired ();
}

#endif
