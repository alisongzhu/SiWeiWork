/*
 * iec104_via_rs485_handler.c
 *
 *  Created on: 2018年12月14日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
    
/* Exported functions --------------------------------------------------------*/

/* ----------------------- Platform includes --------------------------------*/
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "gpio.h"
/* -----------------------  User includes --------------------------------*/
#include "iec60870_slave.h"
#include "lib60870_internal.h"

#include <rn8302b.h>
#include "iec104_via_rs485_handler.h"

/* Private macros ------------------------------------------------------------*/
#define INTERNAL_DISCRETE_START_ADDR    0x0001
#define INTERNAL_INPUT_START_ADDR       0x4501
#define INTERNAL_COIL_START_ADDR        0x6001
#define INTERNAL_HOLDING_START_ADDR     0x6201

#define COMMAND_WITHCP56TIMEA_TIMEOUT   (15000)

#define DEFAULT_CA                     (1)
#define DEFAULT_IO_COUNT               (20)
/* Private types -------------------------------------------------------------*/
typedef enum {
    TYPE_IOA_UNKNOWN,
    TYPE_IOA_INTERNAL,
    TYPE_IOA_EXTERNAL
}IOA_TYPE;

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static ConnectionParameters connectionParameters;
static Slave slave;


/* Global variables ----------------------------------------------------------*/

extern __IO uint64_t TickInMs;

osThreadId taskIEC104Handle;

/* Private function prototypes -----------------------------------------------*/
static bool clockSyncHandler (void* parameter, MasterConnection connection, ASDU asdu, CP56Time2a newTime);
static bool interrogationHandler(void* parameter, MasterConnection connection, ASDU asdu, uint8_t qoi);
static bool asduHandler (void* parameter, MasterConnection connection, ASDU asdu);

//static void vTaskIEC104(void const * argument);
/* Private functions ---------------------------------------------------------*/
static bool
clockSyncHandler (void* parameter, MasterConnection connection, ASDU asdu, CP56Time2a newTime)
{
    DEBUG_PRINT("Process time sync command with time \n");
    // TODO: BROADCASTING ADDR
//    if(ASDU_getCA(asdu) == DEFAULT_CA){
        TickInMs = CP56Time2a_toMsTimestamp(newTime);
        RTC_TimeTypeDef sTime;
//        sTime.SubSeconds = (CP56Time2a_getMillisecond(newTime)<<8) /1000;
        sTime.Seconds = CP56Time2a_getSecond(newTime);
        sTime.Minutes = CP56Time2a_getMinute(newTime);
        sTime.Hours   = CP56Time2a_getHour(newTime);
        HAL_RTC_SetTime(&hrtc, &sTime ,RTC_FORMAT_BIN);

        RTC_DateTypeDef sDate;
        sDate.WeekDay = CP56Time2a_getDayOfWeek(newTime);
        sDate.Date = CP56Time2a_getDayOfMonth(newTime);
        sDate.Month = CP56Time2a_getMonth(newTime);
        sDate.Year = CP56Time2a_getYear(newTime) ;
        HAL_RTC_SetDate(&hrtc, &sDate ,RTC_FORMAT_BIN );

        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
    //  DEBUG_PRINT("Current date and time: %02d year %02d Month %02d weekday %d ", sDate.Year, sDate.Month, sDate.Date,sDate.WeekDay);
    //  DEBUG_PRINT("%02d:%02d:%02d\r\n", sTime.Hours, sTime.Minutes, sTime.Seconds);

        ASDU_setCOT(asdu, ACTIVATION_CON);
        MasterConnection_sendASDU(connection, asdu);
//    }
//    else{
//        DEBUG_PRINT("Received unknown common address of ASDU \n");
//        ASDU_setCOT(asdu, UNKNOWN_COMMON_ADDRESS_OF_ASDU);
//        MasterConnection_sendASDU(connection, asdu);
//    }
    return true;
}

static bool
interrogationHandler(void* parameter, MasterConnection connection, ASDU asdu, uint8_t qoi)
{
  DEBUG_PRINT("Received interrogation for group %i\n", qoi);
  // TODO: QOI processing
//  if(ASDU_getCA(asdu) == DEFAULT_CA){
    MasterConnection_sendACT_CON(connection, asdu, false);

    ASDU  Asdu = ASDU_create(connectionParameters, M_ME_NC_1, true, INTERROGATED_BY_STATION,
                             0, DEFAULT_CA, false, false);

    InformationObject io = NULL;
    for(uint8_t j =0;j<READ_LIST_SIZE;j++)
    {
      io = (InformationObject) MeasuredValueShort_create( (MeasuredValueShort)io,
              INTERNAL_INPUT_START_ADDR+j,
              RN8302B_DataTable_Front[j],
              IEC60870_QUALITY_GOOD);
      ASDU_addInformationObject(Asdu,io);
    }

    InformationObject_destroy(io);

    MasterConnection_sendASDU(connection, Asdu);

    MasterConnection_sendACT_TERM(connection, asdu);
//  }
//  else
//  {
//      DEBUG_PRINT("Received unknown common address of ASDU \n");
//      ASDU_setCOT(asdu, UNKNOWN_COMMON_ADDRESS_OF_ASDU);
//      MasterConnection_sendASDU(connection, asdu);
//  }
  return true;
}

static bool
asduHandler (void* parameter, MasterConnection connection, ASDU asdu)
{
    return false;
}

/* Exported functions --------------------------------------------------------*/
void vTaskIEC104(void const * argument)
{
    /* create a new slave/server instance with default connection parameters and
     * default message queue size */
    slave = T104Slave_create(NULL, 0, 0);

    T104Slave_setLocalAddress(slave, "0.0.0.0");

    /* get the connection parameters - we need them to create correct ASDUs */
    connectionParameters = Slave_getConnectionParameters(slave);

    /* set the callback handler for the clock synchronization command */
    Slave_setClockSyncHandler(slave, clockSyncHandler, NULL);

    /* set the callback handler for the interrogation command */
    Slave_setInterrogationHandler(slave, interrogationHandler, NULL);

    /* set handler for other message types */
    Slave_setASDUHandler(slave, asduHandler, NULL);

//    vTaskDelay(1000/portTICK_PERIOD_MS);

    Slave_start(slave);

    if (Slave_isRunning(slave) == false) {
//        printf("Starting server failed!\n");
    }

    while(1){
      Thread_sleep(1000);
    }
}
