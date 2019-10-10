/*
 * rs485_network.c
 *
 *  Created on: 2017年12月6日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "rs485_network.h"
#include "env.h"
#include "dtu.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
//static TaskHandle_t Com1Handle;
//static TaskHandle_t Com2Handle;
/* Global variables ----------------------------------------------------------*/
struct sRS485Network RS485Network[RS485_PORT_COUNT];

/* Private function prototypes -----------------------------------------------*/
static void
RS485Port1_init (void);
static void
RS485Port2_init (void);
static void
vTaskModbusSlave (void const * argument);
static void
vTaskModbusMaster (void const * argument);

/* Private functions ---------------------------------------------------------*/
static void
RS485Port1_init (void)
{
  BaseType_t status;
  uint8_t opMode = env.ComEnv[0].opMode;
  eMBMode mode;
  switch (opMode)
    {
    case COM_OPMODE_NONE:
//			printf("Configure COM1 closed!\r\n");
      break;
#if MB_SLAVE_ASCII_ENABLED > 0
      case COM_OPMODE_SLAVE_ASC:
      mode = MB_ASCII;
      status = xTaskCreate((TaskFunction_t)vTaskModbusSlave,"MsASC",100,&mode,3,&(RS485Network[0].handler));
      break;
#endif
#if MB_SLAVE_RTU_ENABLED > 0
    case COM_OPMODE_SLAVE_RTU:
      mode = MB_RTU;
      status = xTaskCreate ((TaskFunction_t) vTaskModbusSlave, "MsRTU", 100,
			    &mode, 3, &(RS485Network[0].handler));
      break;
#endif
#if MB_SLAVE_TCP_ENABLED > 0
      case COM_OPMODE_SLAVE_TCP:
      mode = MB_TCP;
      status = xTaskCreate((TaskFunction_t)vTaskModbusSlave,"MsTCP",100,&mode,3,&(RS485Network[0].handler));
      break;
#endif
    default:
//			printf("Not supported operation mode!\r\n");
      break;
    }
  if (status != pdPASS)
    {

    }
}

static void
RS485Port2_init (void)
{
  BaseType_t status;
  uint8_t opMode = env.ComEnv[1].opMode;
  eMBMode mode;
  switch (opMode)
    {
    case COM_OPMODE_NONE:
//			printf("Configure COM2 closed!\r\n");
      break;
    case COM_OPMODE_DTU:
      DTU_init ();
//			status = xTaskCreate((TaskFunction_t)vTaskDTU,"DTU",100,NULL,3,&(RS485Network[1].handler));
      break;
#if MB_MASTER_ASCII_ENABLED > 0
      case COM_OPMODE_MASTER_ASC:
      mode = MB_ASCII;
      status = xTaskCreate((TaskFunction_t)vTaskModbusMaster,"MmASC",100,&mode,3,&(RS485Network[1].handler));
      break;
#endif
#if MB_MASTER_RTU_ENABLED > 0
    case COM_OPMODE_MASTER_RTU:
      mode = MB_RTU;
      status = xTaskCreate ((TaskFunction_t) vTaskModbusMaster, "MmRTU", 100,
			    &mode, 3, &(RS485Network[1].handler));
      break;
#endif
#if MB_MASTER_TCP_ENABLED > 0
      case COM_OPMODE_MASTER_TCP:
      mode = MB_TCP;
      break;
#endif
    default:
//			printf("Not supported operation mode!\r\n");
      break;
    }
  if (status != pdPASS)
    {

    }
}

static void
vTaskModbusSlave (void const * argument)
{
  RS485Network[0].eStatus = MB_ENOERR;
  eMBMode* mode = (eMBMode*) argument;

  RS485Network[0].eStatus = eMBInit (*mode, env.ComEnv[0].address, 1,
				     env.ComEnv[0].baudrate,
				     (eMBParity) env.ComEnv[0].eParity);
  RS485Network[0].eStatus = eMBEnable ();
  while (1)
    {
      RS485Network[0].eStatus = eMBPoll ();
    }
}

void
vTaskModbusMaster (void const * argument)
{
  RS485Network[1].eStatus = MB_ENOERR;
  eMBMode* mode = (eMBMode*) argument;

  RS485Network[1].eStatus = eMBMasterInit (*mode, 2, env.ComEnv[1].baudrate,
					   (eMBParity) env.ComEnv[1].eParity);

  RS485Network[1].eStatus = eMBMasterEnable ();
  while (1)
    {
      RS485Network[1].eStatus = eMBMasterPoll ();
    }
}

/* Exported functions --------------------------------------------------------*/
void
RS485Network_init (void)
{
  RS485Port1_init ();
  RS485Port2_init ();
}
