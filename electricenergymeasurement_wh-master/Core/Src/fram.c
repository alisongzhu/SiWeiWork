/*
 * fram.c
 *
 *  Created on: 2018��11��26��
 *      Author: pt
 */



/* Includes ------------------------------------------------------------------*/
#include "fram.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "gpio.h"
/* Private macros ------------------------------------------------------------*/
#define I2C_SDA_Pin GPIO_PIN_9
#define I2C_SDA_GPIO_Port GPIOB
#define I2C_CLK_Pin GPIO_PIN_8
#define I2C_CLK_GPIO_Port GPIOB
#define iic_SDA_H HAL_GPIO_WritePin(I2C_SDA_GPIO_Port,I2C_SDA_Pin,GPIO_PIN_SET)
#define iic_SDA_L HAL_GPIO_WritePin(I2C_SDA_GPIO_Port,I2C_SDA_Pin,GPIO_PIN_RESET)
#define iic_SCL_H HAL_GPIO_WritePin(I2C_CLK_GPIO_Port,I2C_CLK_Pin,GPIO_PIN_SET)
#define iic_SCL_L HAL_GPIO_WritePin(I2C_CLK_GPIO_Port,I2C_CLK_Pin,GPIO_PIN_RESET)

#define I2C_Speed 100000
#define I2C1_SLAVE_ADDRESS7 0xA0
#define I2C_PageSize 256


#define FALSE 0
#define TRUE  1
/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t FRAM_ADDRESS;
/* Global variables ----------------------------------------------------------*/
static void delay_us(uint16_t us);
/* Private function prototypes -----------------------------------------------*/
static void iicSDA_OUT(void);
static void iicSDA_IN(void);
/* Private functions ---------------------------------------------------------*/
void delay_us(uint16_t us)
{
  __IO uint32_t Delay = us * (SystemCoreClock / 72U / 1000000U);
  do
  {
    __NOP();
  }
  while (Delay --);
}
static void iicSDA_OUT(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.Pin = I2C_SDA_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);
//	iic_SDA_H;
//	iic_SCL_H;
}

static void iicSDA_IN(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	GPIO_InitStruct.Pin = I2C_SDA_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	HAL_GPIO_Init(I2C_SDA_GPIO_Port, &GPIO_InitStruct);

}
static uint8_t iic_Start(void)
{
	iicSDA_OUT();
	iic_SDA_H;
	iic_SCL_H;
	if(!HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port,I2C_SDA_Pin))
		return FALSE;
	delay_us(5);
	iic_SDA_L;
	if(HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port,I2C_SDA_Pin))
		return FALSE;
	delay_us(5);
	iic_SCL_L;
	return TRUE;
}

 static void iic_Stop(void)
{
	iicSDA_OUT();
	iic_SCL_L;
	iic_SDA_L;
	iic_SCL_H;
	delay_us(5);
	iic_SDA_H;
	delay_us(5);

}
 static void iic_ACK(void)
{
	iicSDA_OUT();
	iic_SCL_L;
	delay_us(2);
	iic_SDA_L;
	delay_us(2);
	iic_SCL_H;
	delay_us(5);
	iic_SCL_L;
	delay_us(2);

}
static void iic_NACK(void)
{
	iicSDA_OUT();
	iic_SCL_L;
	iic_SDA_H;
	delay_us(2);
	iic_SCL_H;
	delay_us(5);
	iic_SCL_L;
	delay_us(1);
}
static uint8_t iic_Wait_ACK(void)
{
	uint8_t i=0;
	iicSDA_IN();
	iic_SCL_L;
	delay_us(2);
	iic_SDA_H;
	delay_us(2);
	iic_SCL_H;
	delay_us(1);
	while(HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port,I2C_SDA_Pin))
	{
		i++;
		if(i>250)
		{
			 iic_Stop();
			 return 1;
		}
	}
	iic_SCL_L;
	return 0;
}
static void iic_Send_Byte(uint8_t data)
{
	uint8_t i;
	iicSDA_OUT();
	iic_SCL_L;
	delay_us(2);
	for(i=0;i<8;i++)
	{
		if(data&0x80)
			iic_SDA_H;
		else
			iic_SDA_L;
		data<<=1;
		iic_SCL_H;
		delay_us(2);
		iic_SCL_L;
		delay_us(2);

	}
}

static uint8_t iic_Rev_Byte(void)
{
 uint8_t data=0,i;
 iicSDA_IN();
 iic_SDA_H;
 for(i=0;i<8;i++)
 {
	 data<<=1;
	 iic_SCL_L;
	 delay_us(2);
	 iic_SCL_H;
	 delay_us(2);
	 if(HAL_GPIO_ReadPin(I2C_SDA_GPIO_Port,I2C_SDA_Pin))
		 data|=0x01;

	//delay_us(1);
 }
 iic_SCL_L;
return data;
}
static void iic_Unlock()
{
	uint8_t i;
	for(i=0;i<9;i++)
	{
		iic_SCL_L;
	    delay_us(2);
		iic_SCL_H;
		delay_us(2);
	}
}
/* Exported functions --------------------------------------------------------*/
uint8_t iic_FRAM_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t Addr = 0, count = 0;

	Addr = (uint8_t)WriteAddr / I2C_PageSize;

	count = (uint8_t)WriteAddr % I2C_PageSize;

	Addr = Addr << 1;

	Addr = Addr & 0x0F;

	FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr;

	if (!iic_Start())
	{
		iic_Unlock();
		return FALSE;
	}
	iic_Send_Byte(FRAM_ADDRESS);

	if (iic_Wait_ACK())
	{
		iic_Unlock();
		iic_Stop();
		return FALSE;
	}
	iic_Send_Byte(count);
	iic_Wait_ACK();

	while(NumByteToWrite--)
	{
	iic_Send_Byte(* pBuffer);
	iic_Wait_ACK();
	pBuffer++;
	}
	iic_Stop();

	HAL_Delay(10);
	return TRUE;
}

uint8_t iic_FRAM_BufferRead(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToRead)
{
	uint8_t Addr = 0, count = 0;

	Addr = (uint8_t) WriteAddr / I2C_PageSize;

	count = (uint8_t) WriteAddr % I2C_PageSize;

	Addr = (uint8_t)Addr << 1;

	Addr = Addr & 0x0F;

	FRAM_ADDRESS = I2C1_SLAVE_ADDRESS7 | Addr;

	if (!iic_Start())
		{
			iic_Unlock();
			return FALSE;
		}

	iic_Send_Byte(FRAM_ADDRESS);

	if (iic_Wait_ACK())
	{
		iic_Unlock();
		iic_Stop();
		return FALSE;
	}

	iic_Send_Byte(count);
	iic_Wait_ACK();
	iic_Start();
	iic_Send_Byte(FRAM_ADDRESS | 0x01);
	iic_Wait_ACK();
	while(NumByteToRead)
	{
		*pBuffer = iic_Rev_Byte();
		if(NumByteToRead == 1)iic_NACK();
		else iic_ACK();
		pBuffer++;
		NumByteToRead--;
	}
	iic_Stop();
	return TRUE;
}

void FRAM_iicInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin =I2C_SDA_Pin| I2C_CLK_Pin ;
	GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_OD   ;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	iic_SDA_H;
	iic_SCL_H;
	iic_Unlock();
}
/**
  * @}
  */




