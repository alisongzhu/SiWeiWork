/**
 ******************************************************************************
 * File Name          : USART.c
 * Description        : This file provides code for the configuration
 *                      of the USART instances.
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether 
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2017 STMicroelectronics International N.V. 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other 
 *    contributors to this software may be used to endorse or promote products 
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this 
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under 
 *    this license is void and will automatically terminate your rights under 
 *    this license. 
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "dma.h"
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;
/* USER CODE END 0 */

UART_HandleTypeDef huart5;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

/* UART5 init function */
void
MX_UART5_Init (void)
{

  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init (&huart5) != HAL_OK)
    {
      _Error_Handler (__FILE__, __LINE__);
    }

}
/* USART1 init function */

void
MX_USART1_UART_Init (void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init (&huart1) != HAL_OK)
    {
      _Error_Handler (__FILE__, __LINE__);
    }

}
/* USART6 init function */

void
MX_USART6_UART_Init (void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init (&huart6) != HAL_OK)
    {
      _Error_Handler (__FILE__, __LINE__);
    }

}

void
HAL_UART_MspInit (UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if (uartHandle->Instance == UART5)
    {
      /* USER CODE BEGIN UART5_MspInit 0 */

      /* USER CODE END UART5_MspInit 0 */
      /* UART5 clock enable */
      __HAL_RCC_UART5_CLK_ENABLE()
      ;

      /**UART5 GPIO Configuration    
       PC12     ------> UART5_TX
       PD2     ------> UART5_RX 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_12;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
      HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_2;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
      HAL_GPIO_Init (GPIOD, &GPIO_InitStruct);

      /* USER CODE BEGIN UART5_MspInit 1 */

      /* USER CODE END UART5_MspInit 1 */
    }
  else if (uartHandle->Instance == USART1)
    {
      /* USER CODE BEGIN USART1_MspInit 0 */

      /* USER CODE END USART1_MspInit 0 */
      /* USART1 clock enable */
      __HAL_RCC_USART1_CLK_ENABLE()
      ;

      /**USART1 GPIO Configuration    
       PA9     ------> USART1_TX
       PA10     ------> USART1_RX 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
      HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

      /* USART1 interrupt Init */
      HAL_NVIC_SetPriority (USART1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ (USART1_IRQn);
      /* USER CODE BEGIN USART1_MspInit 1 */

      /* USER CODE END USART1_MspInit 1 */
    }
  else if (uartHandle->Instance == USART6)
    {
      /* USER CODE BEGIN USART6_MspInit 0 */

      /* USER CODE END USART6_MspInit 0 */
      /* USART6 clock enable */
      __HAL_RCC_USART6_CLK_ENABLE()
      ;

      /**USART6 GPIO Configuration    
       PC6     ------> USART6_TX
       PC7     ------> USART6_RX 
       */
      GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_PULLUP;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
      GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
      HAL_GPIO_Init (GPIOC, &GPIO_InitStruct);

      /* USART6 interrupt Init */
      HAL_NVIC_SetPriority (USART6_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ (USART6_IRQn);
      /* USER CODE BEGIN USART6_MspInit 1 */

      /* USART6 DMA Init */
      /* USART6_RX Init */
      hdma_usart6_rx.Instance = DMA2_Stream1;
      hdma_usart6_rx.Init.Channel = DMA_CHANNEL_5;
      hdma_usart6_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_usart6_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_usart6_rx.Init.MemInc = DMA_MINC_ENABLE;
      hdma_usart6_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_usart6_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      hdma_usart6_rx.Init.Mode = DMA_NORMAL;
      hdma_usart6_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
      hdma_usart6_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      if (HAL_DMA_Init (&hdma_usart6_rx) != HAL_OK)
	{
	  _Error_Handler (__FILE__, __LINE__);
	}

      __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart6_rx);

//    /* USART6_TX Init */
//    hdma_usart6_tx.Instance = DMA2_Stream6;
//    hdma_usart6_tx.Init.Channel = DMA_CHANNEL_5;
//    hdma_usart6_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
//    hdma_usart6_tx.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_usart6_tx.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_usart6_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    hdma_usart6_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    hdma_usart6_tx.Init.Mode = DMA_NORMAL;
//    hdma_usart6_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
//    hdma_usart6_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    if (HAL_DMA_Init(&hdma_usart6_tx) != HAL_OK)
//    {
//      _Error_Handler(__FILE__, __LINE__);
//    }

//    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart6_tx);
      /* USER CODE END USART6_MspInit 1 */
    }
}

void
HAL_UART_MspDeInit (UART_HandleTypeDef* uartHandle)
{

  if (uartHandle->Instance == UART5)
    {
      /* USER CODE BEGIN UART5_MspDeInit 0 */

      /* USER CODE END UART5_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_UART5_CLK_DISABLE();

      /**UART5 GPIO Configuration    
       PC12     ------> UART5_TX
       PD2     ------> UART5_RX 
       */
      HAL_GPIO_DeInit (GPIOC, GPIO_PIN_12);

      HAL_GPIO_DeInit (GPIOD, GPIO_PIN_2);

      /* USER CODE BEGIN UART5_MspDeInit 1 */

      /* USER CODE END UART5_MspDeInit 1 */
    }
  else if (uartHandle->Instance == USART1)
    {
      /* USER CODE BEGIN USART1_MspDeInit 0 */

      /* USER CODE END USART1_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_USART1_CLK_DISABLE();

      /**USART1 GPIO Configuration    
       PA9     ------> USART1_TX
       PA10     ------> USART1_RX 
       */
      HAL_GPIO_DeInit (GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

      /* USART1 interrupt Deinit */
      HAL_NVIC_DisableIRQ (USART1_IRQn);
      /* USER CODE BEGIN USART1_MspDeInit 1 */

      /* USER CODE END USART1_MspDeInit 1 */
    }
  else if (uartHandle->Instance == USART6)
    {
      /* USER CODE BEGIN USART6_MspDeInit 0 */
//    /* USART6 DMA DeInit */
      HAL_DMA_DeInit (uartHandle->hdmarx);
//    HAL_DMA_DeInit(uartHandle->hdmatx);
      /* USER CODE END USART6_MspDeInit 0 */
      /* Peripheral clock disable */
      __HAL_RCC_USART6_CLK_DISABLE();

      /**USART6 GPIO Configuration    
       PC6     ------> USART6_TX
       PC7     ------> USART6_RX 
       */
      HAL_GPIO_DeInit (GPIOC, GPIO_PIN_6 | GPIO_PIN_7);

      /* USART6 interrupt Deinit */
      HAL_NVIC_DisableIRQ (USART6_IRQn);
      /* USER CODE BEGIN USART6_MspDeInit 1 */

      /* USER CODE END USART6_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
BOOL
BSP_USART1_UART_Init (ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = ulBaudRate;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  if (MB_PAR_NONE == eParity)
    huart1.Init.Parity = UART_PARITY_NONE;
  else if (MB_PAR_EVEN == eParity)
    huart1.Init.Parity = UART_PARITY_EVEN;
  else if (MB_PAR_ODD == eParity)
    huart1.Init.Parity = UART_PARITY_ODD;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init (&huart1) != HAL_OK)
    {
      return FALSE;
    }

  return TRUE;
}

BOOL
BSP_USART6_UART_Init (ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
  huart6.Instance = USART6;
  huart6.Init.BaudRate = ulBaudRate;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  if (MB_PAR_NONE == eParity)
    huart6.Init.Parity = UART_PARITY_NONE;
  else if (MB_PAR_EVEN == eParity)
    huart6.Init.Parity = UART_PARITY_EVEN;
  else if (MB_PAR_ODD == eParity)
    huart6.Init.Parity = UART_PARITY_ODD;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init (&huart6) != HAL_OK)
    {
      return FALSE;
    }

  return TRUE;
}

/* USER CODE END 1 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
