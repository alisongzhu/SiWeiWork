#include <stdio.h>
#include "usart.h"


int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1,0x00ff);
  return ch;
}
