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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define LED_SPEC_Pin GPIO_PIN_5
#define LED_SPEC_GPIO_Port GPIOE
#define DM8606C_RESET_Pin GPIO_PIN_0
#define DM8606C_RESET_GPIO_Port GPIOC
#define DM8606C_INTR_Pin GPIO_PIN_6
#define DM8606C_INTR_GPIO_Port GPIOA
#define DM8606C_INTR_EXTI_IRQn EXTI9_5_IRQn
#define SELECT_DO_Pin GPIO_PIN_9
#define SELECT_DO_GPIO_Port GPIOE
#define SELECT_FB_Pin GPIO_PIN_10
#define SELECT_FB_GPIO_Port GPIOE
#define DO1_Pin GPIO_PIN_11
#define DO1_GPIO_Port GPIOE
#define DO2_Pin GPIO_PIN_12
#define DO2_GPIO_Port GPIOE
#define DO3_Pin GPIO_PIN_13
#define DO3_GPIO_Port GPIOE
#define DO4_Pin GPIO_PIN_14
#define DO4_GPIO_Port GPIOE
#define DO5_Pin GPIO_PIN_15
#define DO5_GPIO_Port GPIOE
#define DO6_Pin GPIO_PIN_8
#define DO6_GPIO_Port GPIOD
#define DO7_Pin GPIO_PIN_9
#define DO7_GPIO_Port GPIOD
#define DO8_Pin GPIO_PIN_10
#define DO8_GPIO_Port GPIOD
#define DO9_Pin GPIO_PIN_11
#define DO9_GPIO_Port GPIOD
#define DO10_Pin GPIO_PIN_12
#define DO10_GPIO_Port GPIOD
#define DO11_Pin GPIO_PIN_13
#define DO11_GPIO_Port GPIOD
#define DO12_Pin GPIO_PIN_14
#define DO12_GPIO_Port GPIOD
#define RS485_USART6_T_R_Pin GPIO_PIN_15
#define RS485_USART6_T_R_GPIO_Port GPIOD
#define RS485_USART1_T_R_Pin GPIO_PIN_8
#define RS485_USART1_T_R_GPIO_Port GPIOA
#define HC165_QH2_Pin GPIO_PIN_4
#define HC165_QH2_GPIO_Port GPIOD
#define HC165_QH1_Pin GPIO_PIN_5
#define HC165_QH1_GPIO_Port GPIOD
#define HC165_CLK_Pin GPIO_PIN_6
#define HC165_CLK_GPIO_Port GPIOD
#define HC165_SS_Pin GPIO_PIN_7
#define HC165_SS_GPIO_Port GPIOD
#define FLASH_SPI3_CS_Pin GPIO_PIN_6
#define FLASH_SPI3_CS_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
 #define USE_FULL_ASSERT    1U 

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
