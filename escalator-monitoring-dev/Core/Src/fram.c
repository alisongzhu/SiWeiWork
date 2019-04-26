/*
 * fram.c
 *
 *  Created on: 2018��11��26��
 *      Author: pt
 */



/* Includes ------------------------------------------------------------------*/
#include <errno.h>

#include "fram.h"
#include "gpio.h"
#include "cmsis_os.h"
#if defined(FRAM_USE_IIC)
//#include "iic.h"
#elif defined(FRAM_USE_SPI)
#include "spi.h"

#else
#error "Must define mode FRAM_USE_IIC or FRAM_USE_SPI in fram.h"
#endif

/* Private macros ------------------------------------------------------------*/
#if defined(FRAM_USE_IIC)
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
uint8_t fram_write(uint16_t addr, uint8_t* data,uint16_t num_bytes)
{
	uint8_t Addr = 0, count = 0;

	Addr = (uint8_t)addr / I2C_PageSize;

	count = (uint8_t)addr % I2C_PageSize;

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

	while(num_bytes--)
	{
    iic_Send_Byte(* data);
    iic_Wait_ACK();
    data++;
	}
	iic_Stop();

	HAL_Delay(10);
	return TRUE;
}

uint8_t fram_read(uint16_t addr, uint8_t* data, uint16_t num_bytes)
{
	uint8_t Addr = 0, count = 0;

	Addr = (uint8_t) addr / I2C_PageSize;

	count = (uint8_t) addr % I2C_PageSize;

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
	while(num_bytes)
	{
		*data = iic_Rev_Byte();
		if(num_bytes == 1)iic_NACK();
		else iic_ACK();
		data++;
		num_bytes--;
	}
	iic_Stop();
	return TRUE;
}

void fram_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin =I2C_SDA_Pin| I2C_CLK_Pin ;
	GPIO_InitStruct.Mode =  GPIO_MODE_OUTPUT_OD   ;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	iic_SDA_H;
	iic_SCL_H;
}

#elif defined(FRAM_USE_SPI)

#define MB85RSXXV_MANUFACTURER_ID_CMD 0x9f
#define MB85RSXXV_WRITE_ENABLE_CMD 0x06
#define MB85RSXXV_READ_CMD 0x03
#define MB85RSXXV_WRITE_CMD 0x02
#define MAX_USER_DATA_LENGTH 1024

static int mb85rsxxv_access(uint8_t cmd, uint16_t addr, void *data, size_t len)
{
  uint8_t access[3] = {0};
  access[0] = cmd;
  taskENTER_CRITICAL();
  LL_GPIO_ResetOutputPin(FLASH_SPI3_CS_GPIO_Port,FLASH_SPI3_CS_Pin);

  if (cmd == MB85RSXXV_WRITE_CMD || cmd == MB85RSXXV_READ_CMD) {
    access[1] = (addr >> 8) & 0xFF;
    access[2] = addr & 0xFF;
    HAL_SPI_Transmit(&hspi3,access,3,0xff);
    if(cmd == MB85RSXXV_WRITE_CMD){
      HAL_SPI_Transmit(&hspi3,data,len,0xff);
    }
    else
    HAL_SPI_Receive(&hspi3,data,len,0xff);
  }else {
    HAL_SPI_Transmit(&hspi3,access,1,0xff);
    if(cmd == MB85RSXXV_MANUFACTURER_ID_CMD){
      HAL_SPI_Receive(&hspi3,data,len,0xff);
    }
  }

  LL_GPIO_SetOutputPin(FLASH_SPI3_CS_GPIO_Port,FLASH_SPI3_CS_Pin);
  taskEXIT_CRITICAL();
  return 0;
}

static int mb85rsxxv_read_id(void)
{
  uint8_t id[4];
  int err;
  err = mb85rsxxv_access(MB85RSXXV_MANUFACTURER_ID_CMD, 0, &id, 4);
  if (err) {
//    printk("Error during ID read\n");
    return -EIO;
  }

  if (id[0] != 0x04) {
    return -EIO;
  }

  if (id[1] != 0x7f) {
    return -EIO;
  }

  if (id[2] != 0x01) {
    return -EIO;
  }

  if (id[3] != 0x01) {
    return -EIO;
  }

  return 0;
}



int fram_write(uint16_t addr, uint8_t* data,uint16_t num_bytes)
{
  int err;

  /* disable write protect */
  err = mb85rsxxv_access(MB85RSXXV_WRITE_ENABLE_CMD, 0, NULL, 0);
  if (err) {
//    printk("unable to disable write protect\n");
    return -EIO;
  }

  /* write cmd */
  err = mb85rsxxv_access(MB85RSXXV_WRITE_CMD, addr, data, num_bytes);
  if (err) {
//    printk("Error during SPI write\n");
    return -EIO;
  }

  return 0;
}


int fram_read(uint16_t addr, uint8_t* data, uint16_t num_bytes)
{
  int err;

  /* read cmd */
  err = mb85rsxxv_access(MB85RSXXV_READ_CMD, addr, data, num_bytes);
  if (err) {
//    printk("Error during SPI read\n");
    return -EIO;
  }

  return 0;
}

void fram_init(void)
{
  int err;

  err = mb85rsxxv_read_id();
  if (err) {
//    printk("Could not verify FRAM ID\n");
    return;
  }
}

#endif
/**
  * @}
  */




