/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
#define GENERAL_TIM_PRESCALER (14400-1)
#define PULSE_INM_PERIOD 5000
#define PULSE_INM_PRESCALER (7200-1)

#define I2C_SDA_Pin GPIO_PIN_0
#define I2C_SDA_GPIO_Port GPIOA
#define I2C_CLK_Pin GPIO_PIN_1
#define I2C_CLK_GPIO_Port GPIOA
#define ULORA_TX_Pin GPIO_PIN_2
#define ULORA_TX_GPIO_Port GPIOA
#define ULORA_RX_Pin GPIO_PIN_3
#define ULORA_RX_GPIO_Port GPIOA
#define SCSN_M_Pin GPIO_PIN_4
#define SCSN_M_GPIO_Port GPIOA
#define SCLK_M_Pin GPIO_PIN_5
#define SCLK_M_GPIO_Port GPIOA
#define SDI_M_Pin GPIO_PIN_6
#define SDI_M_GPIO_Port GPIOA
#define SDO_M_Pin GPIO_PIN_7
#define SDO_M_GPIO_Port GPIOA
#define INTN_Pin GPIO_PIN_0
#define INTN_GPIO_Port GPIOB
#define RN8302_RST_Pin GPIO_PIN_1
#define RN8302_RST_GPIO_Port GPIOB
#define LED_STATE_Pin GPIO_PIN_2
#define LED_STATE_GPIO_Port GPIOB
#define UTX485_1_Pin GPIO_PIN_10
#define UTX485_1_GPIO_Port GPIOB
#define URX485_1_Pin GPIO_PIN_11
#define URX485_1_GPIO_Port GPIOB
#define E32_M0_Pin GPIO_PIN_12
#define E32_M0_GPIO_Port GPIOB
#define E32_M1_Pin GPIO_PIN_13
#define E32_M1_GPIO_Port GPIOB
#define E32_AUX_Pin GPIO_PIN_14
#define E32_AUX_GPIO_Port GPIOB
#define LORA_RST_Pin GPIO_PIN_15
#define LORA_RST_GPIO_Port GPIOB
#define DS18B20_DQ_Pin GPIO_PIN_8
#define DS18B20_DQ_GPIO_Port GPIOA
#define UTX485_2_Pin GPIO_PIN_9
#define UTX485_2_GPIO_Port GPIOA
#define URX485_2_Pin GPIO_PIN_10
#define URX485_2_GPIO_Port GPIOA
#define SWITCH3_ON_Pin GPIO_PIN_11
#define SWITCH3_ON_GPIO_Port GPIOA
#define SWITCH3_OFF_Pin GPIO_PIN_12
#define SWITCH3_OFF_GPIO_Port GPIOA
#define SWITCH1_ON_Pin GPIO_PIN_15
#define SWITCH1_ON_GPIO_Port GPIOA
#define SWITCH1_OFF_Pin GPIO_PIN_3
#define SWITCH1_OFF_GPIO_Port GPIOB
#define SWO_Pin GPIO_PIN_4
#define SWO_GPIO_Port GPIOB
#define HC165_S_L_Pin GPIO_PIN_5
#define HC165_S_L_GPIO_Port GPIOB
#define HC165_CLK_Pin GPIO_PIN_6
#define HC165_CLK_GPIO_Port GPIOB
#define HC165_DI_Pin GPIO_PIN_7
#define HC165_DI_GPIO_Port GPIOB
#define SWITCH2_ON_Pin GPIO_PIN_8
#define SWITCH2_ON_GPIO_Port GPIOB
#define SWITCH2_OFF_Pin GPIO_PIN_9
#define SWITCH2_OFF_GPIO_Port GPIOB

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

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
