#include <stdio.h>
#include "usart.h"
/**
 * ��������: �ض���c�⺯��printf��USART5
 * �������: ��
 * �� �� ֵ: ��
 * ˵    ������
 */
int
fputc (int ch, FILE *f)
{
  HAL_UART_Transmit (&huart5, (uint8_t *) &ch, 1, 0xffff);
  return ch;
}
