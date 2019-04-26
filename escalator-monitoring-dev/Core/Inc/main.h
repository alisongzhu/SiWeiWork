/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_crc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

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
#define LED_SPECIAL_Pin LL_GPIO_PIN_5
#define LED_SPECIAL_GPIO_Port GPIOE
#define DM8606B_RESET_Pin LL_GPIO_PIN_0
#define DM8606B_RESET_GPIO_Port GPIOC
#define ETH_INTR_Pin LL_GPIO_PIN_6
#define ETH_INTR_GPIO_Port GPIOA
#define RN8302B_SPI2_CS_Pin LL_GPIO_PIN_15
#define RN8302B_SPI2_CS_GPIO_Port GPIOE
#define RN8302B_SPI2_SCK_Pin LL_GPIO_PIN_10
#define RN8302B_SPI2_SCK_GPIO_Port GPIOB
#define RN8302B_SPI2_MOSI_Pin LL_GPIO_PIN_14
#define RN8302B_SPI2_MOSI_GPIO_Port GPIOB
#define RN8302B_SPI2_MOSIB15_Pin LL_GPIO_PIN_15
#define RN8302B_SPI2_MOSIB15_GPIO_Port GPIOB
#define HC165_SS_Pin LL_GPIO_PIN_9
#define HC165_SS_GPIO_Port GPIOD
#define HC165_CLK_Pin LL_GPIO_PIN_10
#define HC165_CLK_GPIO_Port GPIOD
#define HC165_SERIAL_INPUT2_Pin LL_GPIO_PIN_11
#define HC165_SERIAL_INPUT2_GPIO_Port GPIOD
#define HC165_SERIAL_INPUT1_Pin LL_GPIO_PIN_12
#define HC165_SERIAL_INPUT1_GPIO_Port GPIOD
#define HC595_OUTPUT_Pin LL_GPIO_PIN_13
#define HC595_OUTPUT_GPIO_Port GPIOD
#define HC595_STORAGE_REG_CLK_Pin LL_GPIO_PIN_14
#define HC595_STORAGE_REG_CLK_GPIO_Port GPIOD
#define HC595_SHIFT_REG_CLK_Pin LL_GPIO_PIN_15
#define HC595_SHIFT_REG_CLK_GPIO_Port GPIOD
#define HC595_OVERRIDING_CLEAR_Pin LL_GPIO_PIN_6
#define HC595_OVERRIDING_CLEAR_GPIO_Port GPIOC
#define DS18B20_Pin LL_GPIO_PIN_15
#define DS18B20_GPIO_Port GPIOA
#define FLASH_SPI3_CS_Pin LL_GPIO_PIN_6
#define FLASH_SPI3_CS_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
