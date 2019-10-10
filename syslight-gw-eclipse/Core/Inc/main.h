/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DS18B20_GPIO_INOUT_PA1_Pin GPIO_PIN_1
#define DS18B20_GPIO_INOUT_PA1_GPIO_Port GPIOA
#define RS485_UART2_TX_Pin GPIO_PIN_2
#define RS485_UART2_TX_GPIO_Port GPIOA
#define RS485_UART2_RX_Pin GPIO_PIN_3
#define RS485_UART2_RX_GPIO_Port GPIOA
#define W5500_RST_GPIO_OUT_PA4_Pin GPIO_PIN_4
#define W5500_RST_GPIO_OUT_PA4_GPIO_Port GPIOA
#define W5500_SPI1_SCK_Pin GPIO_PIN_5
#define W5500_SPI1_SCK_GPIO_Port GPIOA
#define W5500_SPI1_MISO_Pin GPIO_PIN_6
#define W5500_SPI1_MISO_GPIO_Port GPIOA
#define W5500_SPI1_MOSI_Pin GPIO_PIN_7
#define W5500_SPI1_MOSI_GPIO_Port GPIOA
#define W5500_INT_GPIO_INT_PB0_Pin GPIO_PIN_0
#define W5500_INT_GPIO_INT_PB0_GPIO_Port GPIOB
#define W5500_CS_GPIO_OUT_PB1_Pin GPIO_PIN_1
#define W5500_CS_GPIO_OUT_PB1_GPIO_Port GPIOB
#define PB721_UART3_TX_Pin GPIO_PIN_10
#define PB721_UART3_TX_GPIO_Port GPIOB
#define PB721_UART3_RX_Pin GPIO_PIN_11
#define PB721_UART3_RX_GPIO_Port GPIOB
#define PB721_BRK_GPIO_IN_PB15_Pin GPIO_PIN_15
#define PB721_BRK_GPIO_IN_PB15_GPIO_Port GPIOB
#define CH340C_UART1_TX_Pin GPIO_PIN_9
#define CH340C_UART1_TX_GPIO_Port GPIOA
#define CH340C_UART1_RX_Pin GPIO_PIN_10
#define CH340C_UART1_RX_GPIO_Port GPIOA
#define BUTTON2_GPIO_IN_PA11_Pin GPIO_PIN_11
#define BUTTON2_GPIO_IN_PA11_GPIO_Port GPIOA
#define BUTTON1_GPIO_IN_PA12_Pin GPIO_PIN_12
#define BUTTON1_GPIO_IN_PA12_GPIO_Port GPIOA
#define LED1_TIM2_CH1_GPIO_OUT_PA15_Pin GPIO_PIN_15
#define LED1_TIM2_CH1_GPIO_OUT_PA15_GPIO_Port GPIOA
#define LED2_TIM3_CH1_GPIO_OUT_PB4_Pin GPIO_PIN_4
#define LED2_TIM3_CH1_GPIO_OUT_PB4_GPIO_Port GPIOB
#define FRAM_I2C1_SCL_Pin GPIO_PIN_6
#define FRAM_I2C1_SCL_GPIO_Port GPIOB
#define FRAM_I2C1_SDA_Pin GPIO_PIN_7
#define FRAM_I2C1_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
