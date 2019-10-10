#include <stdio.h>
#include "usart.h"
/**
  * 函数功能: 重定向c库函数printf到USART5
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1,0xffff);
  return ch;
}
