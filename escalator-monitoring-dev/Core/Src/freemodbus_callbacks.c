/*
 * freemodbus_callbacks.c
 *
 *  Created on: 2019年1月10日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include "mb.h"
#include "mbutils.h"
#include "rn8302b.h"
#include "user_regs.h"

#include <stdint.h>
/* Private macros ------------------------------------------------------------*/


/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint16_t   usRegInputBuf[REG_INPUT_NREGS];
uint8_t    ucDiscreteBuf[REG_DISCRETE_LIST_SIZE];

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
    
/* Exported functions --------------------------------------------------------*/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - REG_INPUT_START );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );

            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  if( ( usAddress >= REG_DISCRETE_START )
      && ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_NREGS ) )
  {
      iRegIndex = ( int )( usAddress - REG_DISCRETE_START);
      while( usNDiscrete > 0 )
      {
        *pucRegBuffer++ =
         xMBUtilGetBits(ucDiscreteBuf, iRegIndex,(unsigned char)(usNDiscrete > 8 ? 8 : usNDiscrete));
        if(usNDiscrete > 8)
        {
          usNDiscrete -= 8;
          iRegIndex += 8;
        }
        else
        {
          break;
        }

      }
  }
  else
  {
      eStatus = MB_ENOREG;
  }

  return eStatus;
}
