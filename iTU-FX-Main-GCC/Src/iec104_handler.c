// Header: IEC104 stack handle functions
// File Name: iec104_handler.c
// Author:LH
// Date: Aug 16 2017

/* Includes ------------------------------------------------------------------*/
/* ----------------------- Platform includes --------------------------------*/
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "cmsis_os.h"
#include "iwdg.h"
#include "gpio.h"
/* -----------------------  User includes --------------------------------*/
#include "iec60870_slave.h"
#include "lib60870_internal.h"
#include "user_config.h"
#include "env.h"
#include "can_network.h"
#include "iec104_handler.h"

/* Private macros ------------------------------------------------------------*/
#define SELECT_ON()         	HAL_GPIO_WritePin(SELECT_DO_GPIO_Port,SELECT_DO_Pin,GPIO_PIN_SET)
#define SELECT_OFF()		    HAL_GPIO_WritePin(SELECT_DO_GPIO_Port,SELECT_DO_Pin,GPIO_PIN_RESET)


#define INTERNAL_DISCRETE_START_ADDR    0x0001
#define INTERNAL_INPUT_START_ADDR       0x4001
#define INTERNAL_COIL_START_ADDR        0x6001
#define INTERNAL_HOLDING_START_ADDR     0x6201

#define EXTERNAL_DISCRETE_START_ADDR    0x1001
#define EXTERNAL_DEV_DISCRETE_CNT       100
#define EXTERNAL_INPUT_START_ADDR       0x4101
#define EXTERNAL_DEV_INPUT_CNT          100
#define EXTERNAL_COIL_START_ADDR        0x6101
#define EXTERNAL_DEV_COIL_CNT           100
#define EXTERNAL_HOLDING_START_ADDR     0x6301
#define EXTERNAL_DEV_HOLDING_CNT        100

#define COMMAND_WITHCP56TIMEA_TIMEOUT   (15000)

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
uint64_t TimeSelectOn = 0;
extern __IO uint64_t TickInMs;
extern xQueueHandle RawEventQueue;
extern __IO DiscreteInfo LogicalDiscreteInfo[INTERNAL_DISCRETE_COUNT];
osThreadId taskIEC104Handle;

/* Private function prototypes -----------------------------------------------*/
static bool clockSyncHandler (void* parameter, MasterConnection connection, ASDU asdu, CP56Time2a newTime);
static void prvvProcessExternalIn32FloatData(MasterConnection connection);
static void prvvProcessExternalIn8Data(MasterConnection connection);
static void prvvProcessInternalData(MasterConnection connection);
static bool interrogationHandler(void* parameter, MasterConnection connection, ASDU asdu, uint8_t qoi);
static bool asduHandler (void* parameter, MasterConnection connection, ASDU asdu);

static bool isValidIOA(int ioa);
static void processSingleCommand(ASDU asdu,InformationObject io);
static void processDoubleCommand(ASDU asdu,InformationObject io);
static void ExecuteOutput(int LogicalDiscreteNo,bool onoff);
//static bool Asdu_isValidCA(ASDU self);
static bool IsCommandExecutable(ASDU asdu);
static void prvvProcessExternalIn32FloatEvent(void);
static void prvvProcessExternalIn8Event(void);
static void prvvProcessInternalEvent(void);

static void handleSelectSwitchTimeout(void);

static void vTaskIEC104(void const * argument);
/* Private functions ---------------------------------------------------------*/
static bool
clockSyncHandler (void* parameter, MasterConnection connection, ASDU asdu, CP56Time2a newTime)
{
    DEBUG_PRINT("Process time sync command with time \n");
    // TODO: BROADCASTING ADDR
    if(ASDU_getCA(asdu) == env.IEC104Env.address){
        TickInMs = CP56Time2a_toMsTimestamp(newTime);
        RTC_TimeTypeDef sTime;
        sTime.SubSeconds = (CP56Time2a_getMillisecond(newTime)<<8) /1000;
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
    }
    else{
        DEBUG_PRINT("Received unknown common address of ASDU \n");
        ASDU_setCOT(asdu, UNKNOWN_COMMON_ADDRESS_OF_ASDU);
        MasterConnection_sendASDU(connection, asdu);
    }
    return true;
}

static void prvvProcessExternalIn32FloatData(MasterConnection connection)
{
    struct sCanDevice *dev = NULL;
    for(uint8_t i= 0;i<env.BasicEnv.CanDevCount;i++)
    {

        dev = (struct sCanDevice*)(CanNetwork->CanDevList)[i];
        if((dev != NULL)&&(dev->data->in32float != NULL))
        {
            ASDU  Asdu = ASDU_create(connectionParameters, M_ME_NC_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
            InformationObject io = NULL;
            for(uint8_t j =0;j<dev->in32floatCount;j++)
            {
                io = (InformationObject) MeasuredValueShort_create( (MeasuredValueShort)io,
                        EXTERNAL_INPUT_START_ADDR+i*EXTERNAL_DEV_INPUT_CNT+j,
                        dev->data->in32float[j],
                        IEC60870_QUALITY_GOOD);
                ASDU_addInformationObject(Asdu,io);
              
            }
            InformationObject_destroy(io);
            MasterConnection_sendASDU(connection, Asdu);
        }
        
    }
}

static void prvvProcessExternalIn8Data(MasterConnection connection)
{
    struct sCanDevice *dev = NULL;
    for(uint8_t i= 0;i<env.BasicEnv.CanDevCount;i++)
    {
        dev = (struct sCanDevice*)(CanNetwork->CanDevList)[i];
        if((dev != NULL)&&(dev->data->in8 != NULL))
        {
					if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
					{
            ASDU  Asdu = ASDU_create(connectionParameters, M_SP_NA_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
            InformationObject io = NULL;
            for(uint8_t j =0;j<dev->in8Count;j++)
            {
                io = (InformationObject) SinglePointInformation_create( (SinglePointInformation)io,
                        EXTERNAL_DISCRETE_START_ADDR+i*EXTERNAL_DEV_DISCRETE_CNT+j,
                        dev->data->in8[j]&1,
                        IEC60870_QUALITY_GOOD);
                ASDU_addInformationObject(Asdu,io);
            }
            InformationObject_destroy(io);
            MasterConnection_sendASDU(connection, Asdu);
					}
					
					if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
					{
            ASDU  Asdu = ASDU_create(connectionParameters, M_DP_NA_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
            InformationObject io = NULL;
            for(uint8_t j =0;j<dev->in8Count;j++)
            {
                io = (InformationObject) DoublePointInformation_create( (DoublePointInformation)io,
                        EXTERNAL_DISCRETE_START_ADDR+i*EXTERNAL_DEV_DISCRETE_CNT+j,
                        (DoublePointValue)dev->data->in8[j],
                        IEC60870_QUALITY_GOOD);
                ASDU_addInformationObject(Asdu,io);
            }
            InformationObject_destroy(io);
            MasterConnection_sendASDU(connection, Asdu);
					}					
        }
    }
}

static void prvvProcessInternalData(MasterConnection connection)
{
	ASDU Asdu;
	
	if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
	{
		Asdu = ASDU_create(connectionParameters, M_SP_NA_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
	}
	else if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
	{
		Asdu = ASDU_create(connectionParameters, M_DP_NA_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
	}
	
  InformationObject io = NULL;
		
	for(uint8_t i=0;i< env.BasicEnv.InternalDiscreteCNT;i++)
	{
		if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
		{
			io = (InformationObject) SinglePointInformation_create( (SinglePointInformation)io,
							INTERNAL_DISCRETE_START_ADDR+i,
							LogicalDiscreteInfo[i].val&1,
							IEC60870_QUALITY_GOOD);
		}
		else if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
		{
			io = (InformationObject) DoublePointInformation_create( (DoublePointInformation)io,
							INTERNAL_DISCRETE_START_ADDR+i,
							(DoublePointValue)LogicalDiscreteInfo[i].val,
							IEC60870_QUALITY_GOOD);			
		}
		
		ASDU_addInformationObject(Asdu,io);
	}
	
	InformationObject_destroy(io);
	MasterConnection_sendASDU(connection, Asdu);		
	
//	if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
//	{
//		ASDU Asdu = ASDU_create(connectionParameters, M_DP_NA_1, true, INTERROGATED_BY_STATION, 0, env.IEC104Env.address, false, false);
//    InformationObject io = NULL;
//    for(uint8_t i=0;i< env.BasicEnv.InternalDiscreteCNT;i++)
//    {
//        io = (InformationObject) DoublePointInformation_create( (DoublePointInformation)io,
//                INTERNAL_DISCRETE_START_ADDR+i,
//                (DoublePointValue)LogicalDiscreteInfo[i].val,
//                IEC60870_QUALITY_GOOD);
//        ASDU_addInformationObject(Asdu,io);
//    }
//    InformationObject_destroy(io);
//    MasterConnection_sendASDU(connection, Asdu);
//	}
}

static bool
interrogationHandler(void* parameter, MasterConnection connection, ASDU asdu, uint8_t qoi)
{
    DEBUG_PRINT("Received interrogation for group %i\n", qoi);
    // TODO: QOI processing
    if(ASDU_getCA(asdu) == env.IEC104Env.address){
        MasterConnection_sendACT_CON(connection, asdu, false);
			
				if(env.BasicEnv.CanDevCount > 0)
				{
					prvvProcessExternalIn32FloatData(connection);
				}

        prvvProcessInternalData(connection);
				
				if(env.BasicEnv.CanDevCount > 0)
				{
					prvvProcessExternalIn8Data(connection);
				}
				
        MasterConnection_sendACT_TERM(connection, asdu);
    }
    else
    {
        DEBUG_PRINT("Received unknown common address of ASDU \n");
        ASDU_setCOT(asdu, UNKNOWN_COMMON_ADDRESS_OF_ASDU);
        MasterConnection_sendASDU(connection, asdu);
    }
    return true;
}

static bool isValidIOA(int ioa)
{
    if((ioa < INTERNAL_COIL_START_ADDR) || ioa > (INTERNAL_COIL_START_ADDR + env.BasicEnv.InternalCoilCNT))
        return false;
    return true;
}

static void ExecuteOutput(int LogicalDiscreteNo,bool onoff)
{
	struct sLogicalCoilIndividual *obj = &env.LogicalCoilIndividual[LogicalDiscreteNo];
    if(onoff){
        if(COMBINATIONAL_MODE_SP==obj->CombinationalMode){
            ExecuteInternalCoilOutputON(obj->combination.sp);
//            osDelay(obj->PulseWidth);
//            ExecuteInternalCoilOutputOFF(obj->combination.sp);
        }
        if(COMBINATIONAL_MODE_DP==obj->CombinationalMode){
            ExecuteInternalCoilOutputON(obj->combination.dp.on);
            osDelay(obj->PulseWidth);
            ExecuteInternalCoilOutputOFF(obj->combination.dp.on);
        }
        if(COMBINATIONAL_MODE_DD==obj->CombinationalMode){
            ExecuteInternalCoilOutputON(obj->combination.dd.on1);
            ExecuteInternalCoilOutputON(obj->combination.dd.on2);
            osDelay(obj->PulseWidth);
            ExecuteInternalCoilOutputOFF(obj->combination.dd.on1);
            ExecuteInternalCoilOutputOFF(obj->combination.dd.on2);
        }
    }
    else{
        if(COMBINATIONAL_MODE_SP==obj->CombinationalMode){
//            ExecuteInternalCoilOutputON(obj->combination.sp);
//            osDelay(obj->PulseWidth);
            ExecuteInternalCoilOutputOFF(obj->combination.sp);
        }
        if(COMBINATIONAL_MODE_DP==obj->CombinationalMode){
            ExecuteInternalCoilOutputON(obj->combination.dp.off);
            osDelay(obj->PulseWidth);
            ExecuteInternalCoilOutputOFF(obj->combination.dp.off);
        }
        if(COMBINATIONAL_MODE_DD==obj->CombinationalMode){
            ExecuteInternalCoilOutputON(obj->combination.dd.off1);
            ExecuteInternalCoilOutputON(obj->combination.dd.off2);
            osDelay(obj->PulseWidth);
            ExecuteInternalCoilOutputOFF(obj->combination.dd.off1);
            ExecuteInternalCoilOutputOFF(obj->combination.dd.off2);
        }
    }
}

static void processSingleCommand(ASDU asdu,InformationObject io)
{
    int ioa = InformationObject_getObjectAddress(io);
    SingleCommand sc = (SingleCommand) io;
    if(SingleCommand_isSelect(sc))
		{
			if( COIL_MODE_DIRECT == env.BasicEnv.CoilOutputMode) 
			{
			   env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected =1;
         TimeSelectOn = HAL_GetTick();
         ASDU_setCOT(asdu, ACTIVATION_CON);				
			}
      else
			{
				if((0 == env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected)
                &&(GPIO_PIN_SET == HAL_GPIO_ReadPin(SELECT_FB_GPIO_Port,SELECT_FB_Pin))){
            SELECT_ON();
            env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected =1;
            TimeSelectOn = HAL_GetTick();
            ASDU_setCOT(asdu, ACTIVATION_CON);
        }
        else {
            ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
        }
			}
    }
    else
		{
				if(env.BasicEnv.CoilOutputMode == COIL_MODE_SBO)
				{
						if(0 == env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected)
						{
								ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
						}
						else 
						{
							ExecuteOutput(ioa-INTERNAL_COIL_START_ADDR,SingleCommand_getState(sc));
							env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected = 0;
							SELECT_OFF();
							ASDU_setCOT(asdu, ACTIVATION_CON);
						}
				}
				if(env.BasicEnv.CoilOutputMode == COIL_MODE_DIRECT)
				{
//						SELECT_ON();
//						TimeSelectOn = HAL_GetTick();
						ExecuteOutput(ioa-INTERNAL_COIL_START_ADDR,SingleCommand_getState(sc));
//						SELECT_OFF();
						ASDU_setCOT(asdu, ACTIVATION_CON);
				}
    }
}

static void processDoubleCommand(ASDU asdu,InformationObject io)
{
    int ioa = InformationObject_getObjectAddress(io);
    DoubleCommand dc = (DoubleCommand) io;
    if(DoubleCommand_isSelect(dc))
		{
			if( COIL_MODE_DIRECT == env.BasicEnv.CoilOutputMode)
			{
				env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected =1;
        TimeSelectOn = HAL_GetTick();
        ASDU_setCOT(asdu, ACTIVATION_CON);
			}
			else 
			{
				if((0 == env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected)
                &&(GPIO_PIN_SET == HAL_GPIO_ReadPin(SELECT_FB_GPIO_Port,SELECT_FB_Pin))){
            SELECT_ON();
            env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected =1;
            TimeSelectOn = HAL_GetTick();
            ASDU_setCOT(asdu, ACTIVATION_CON);
        }
        else {
            ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
        }
			}
    }
    else{
				if(env.BasicEnv.CoilOutputMode == COIL_MODE_SBO)
				{
					  if(0 == env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected)
						{
								ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
						}
						else 
						{
								int val = DoubleCommand_getState(dc);
								if((val == IEC60870_DOUBLE_POINT_INTERMEDIATE)||(val == IEC60870_DOUBLE_POINT_INDETERMINATE))
								{
										ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
								}
								else
								{
										ExecuteOutput(ioa-INTERNAL_COIL_START_ADDR,(val==IEC60870_DOUBLE_POINT_ON));
										env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected = 0;
										SELECT_OFF();
										ASDU_setCOT(asdu, ACTIVATION_CON);
								}
						}
				}
				if(env.BasicEnv.CoilOutputMode == COIL_MODE_DIRECT)
				{
						int val = DoubleCommand_getState(dc);
						if((val == IEC60870_DOUBLE_POINT_INTERMEDIATE)||(val == IEC60870_DOUBLE_POINT_INDETERMINATE))
						{
								ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
						}
						else
						{
//								SELECT_ON();
//								TimeSelectOn = HAL_GetTick();
								ExecuteOutput(ioa-INTERNAL_COIL_START_ADDR,(val==IEC60870_DOUBLE_POINT_ON));
//								env.LogicalCoilIndividual[ioa-INTERNAL_COIL_START_ADDR].Selected = 0;
//								SELECT_OFF();
								ASDU_setCOT(asdu, ACTIVATION_CON);
						}
				}
    }
}
static bool
asduHandler (void* parameter, MasterConnection connection, ASDU asdu)
{
    if(ASDU_getTypeID(asdu) == C_SC_NA_1) {
        DEBUG_PRINT("received single command.\n");
        if(!IsCommandExecutable(asdu)){
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }
        if(DEACTIVATION == ASDU_getCOT(asdu)){
						if(env.BasicEnv.CoilOutputMode == COIL_MODE_SBO){
							SELECT_OFF();
						}
            uint8_t i;
            for(i=0;i<env.BasicEnv.InternalCoilCNT;i++){
                env.LogicalCoilIndividual[i].Selected = 0;
            }
            ASDU_setCOT(asdu, DEACTIVATION_CON);
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }
        InformationObject io = ASDU_getElement(asdu, 0);
        processSingleCommand(asdu,io);
        InformationObject_destroy(io);
        MasterConnection_sendASDU(connection, asdu);
        return true;
    }


    if(ASDU_getTypeID(asdu) == C_DC_NA_1){
        DEBUG_PRINT("received double command.\n");
        if(!IsCommandExecutable(asdu)){
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }
        if(DEACTIVATION == ASDU_getCOT(asdu)){
						if(env.BasicEnv.CoilOutputMode == COIL_MODE_SBO)
							SELECT_OFF();
            uint8_t i;
            for(i=0;i<env.BasicEnv.InternalCoilCNT;i++){
                env.LogicalCoilIndividual[i].Selected = 0;
            }
            ASDU_setCOT(asdu, DEACTIVATION_CON);
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }
        InformationObject io = ASDU_getElement(asdu, 0);
        processDoubleCommand(asdu,io);
        InformationObject_destroy(io);
        MasterConnection_sendASDU(connection, asdu);
        return true;
    }

    if(ASDU_getTypeID(asdu) == C_SC_TA_1){
        DEBUG_PRINT("received single command with CP56Time2a.\n");
        if(!IsCommandExecutable(asdu)){
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }
        InformationObject io = ASDU_getElement(asdu, 0);

        SingleCommandWithCP56Time2a sct = (SingleCommandWithCP56Time2a) io;
        CP56Time2a SingleCmdTime =SingleCommandWithCP56Time2a_getTimestamp(sct);
        if(TickInMs >  (CP56Time2a_toMsTimestamp(SingleCmdTime) + COMMAND_WITHCP56TIMEA_TIMEOUT)){
            DEBUG_PRINT("wrong command time stamp!\n");
            ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
            InformationObject_destroy(io);
            return true;
        }
        processSingleCommand(asdu,io);
        InformationObject_destroy(io);
        MasterConnection_sendASDU(connection, asdu);
        return true;
    }

    if(ASDU_getTypeID(asdu) == C_DC_TA_1){
        DEBUG_PRINT("received double command with CP56Time2a.\n");
        if(!IsCommandExecutable(asdu)){
            MasterConnection_sendASDU(connection, asdu);
            return true;
        }

        InformationObject io = ASDU_getElement(asdu, 0);

        DoubleCommandWithCP56Time2a dct = (DoubleCommandWithCP56Time2a) io;
        CP56Time2a DoubleCmdTime = DoubleCommandWithCP56Time2a_getTimestamp(dct);
        if(TickInMs >  (CP56Time2a_toMsTimestamp(DoubleCmdTime) + COMMAND_WITHCP56TIMEA_TIMEOUT)){
            DEBUG_PRINT("wrong command time stamp!\n");
            ASDU_setCOT(asdu, ACTIVATION_TERMINATION);
            InformationObject_destroy(io);
            return true;
        }
        processDoubleCommand(asdu,io);
        InformationObject_destroy(io);
        MasterConnection_sendASDU(connection, asdu);
        return true;
    }
    
    return false;
}

static bool IsCommandExecutable(ASDU asdu)
{
    //check CA
    if(ASDU_getCA(asdu) != env.IEC104Env.address){
        DEBUG_PRINT("wrong common address!\n");
        ASDU_setCOT(asdu, UNKNOWN_COMMON_ADDRESS_OF_ASDU);
        return false;
    }
    //check COT
    CauseOfTransmission cot = ASDU_getCOT(asdu);
    if((cot != ACTIVATION) &&(cot != DEACTIVATION)){
        DEBUG_PRINT("unknown cause of transmission!\n");
        ASDU_setCOT(asdu, UNKNOWN_CAUSE_OF_TRANSMISSION);
        return false;
    }
    //check IOA
    InformationObject io = ASDU_getElement(asdu, 0);
    int ioa = InformationObject_getObjectAddress(io);
    if(!isValidIOA(ioa)) {
        DEBUG_PRINT("unknown information object address!\n");
        ASDU_setCOT(asdu, UNKNOWN_INFORMATION_OBJECT_ADDRESS);
        InformationObject_destroy(io);
        return false;
    }
    InformationObject_destroy(io);
    return true;
}

static void prvvProcessExternalIn32FloatEvent(void)
{
    UBaseType_t uxNumberOfItems = uxQueueMessagesWaiting(CanNetwork->In32FloatEventQueue);
    if(uxNumberOfItems)
    {
        struct sIn32FloatEvent item;

//            while(uxNumberOfItems--)
//            {
//                xQueueReceive( CanNetwork->MeasuredValueShortEventQueue, &item, 0 );
//                ASDU Asdu = ASDU_create(connectionParameters, M_ME_NC_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
//                InformationObject io = (InformationObject) MeasuredValueShort_create(NULL,
//                        EXTERNAL_INPUT_START_ADDR+(item.address-1)*EXTERNAL_DEV_INPUT_CNT+item.order,
//                        item.val,
//                        IEC60870_QUALITY_GOOD);
//                ASDU_addInformationObject(Asdu, io);
//                InformationObject_destroy(io);
//                Slave_enqueueASDU(slave, Asdu);
//            }

        ASDU Asdu = ASDU_create(connectionParameters, M_ME_NC_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
        InformationObject io = NULL;
			
				for(uint8_t i = 0;i<uxNumberOfItems;i++)//todo: max asdu lenth == 256.
        {
            xQueueReceive( CanNetwork->In32FloatEventQueue, &item, 0 );
            io = (InformationObject) MeasuredValueShort_create((MeasuredValueShort)io,
                    EXTERNAL_INPUT_START_ADDR+(item.address-1)*EXTERNAL_DEV_INPUT_CNT+(item.order-1),
                    item.val,
                    IEC60870_QUALITY_GOOD);
            ASDU_addInformationObject(Asdu, io);
        }
        InformationObject_destroy(io);
        Slave_enqueueASDU(slave, Asdu);
    }
}

static void prvvProcessExternalIn8Event(void)
{
    UBaseType_t uxNumberOfItems = uxQueueMessagesWaiting(CanNetwork->In8EventQueue);
    if(uxNumberOfItems)
    {
      struct sIn8Event item;
			if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
			{
        ASDU Asdu = ASDU_create(connectionParameters, M_SP_NA_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
        InformationObject io = NULL;
        for(uint8_t i = 0;i<uxNumberOfItems;i++)
        {
            xQueueReceive( CanNetwork->In32FloatEventQueue, &item, 0 );
            io = (InformationObject) SinglePointInformation_create((SinglePointInformation)io,
                    EXTERNAL_DISCRETE_START_ADDR+(item.address-1)*EXTERNAL_DEV_DISCRETE_CNT+item.order,
                    item.val&1,
                    IEC60870_QUALITY_GOOD);
            ASDU_addInformationObject(Asdu, io);
        }
        InformationObject_destroy(io);
        Slave_enqueueASDU(slave, Asdu);				
			}
			if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
			{
        ASDU Asdu = ASDU_create(connectionParameters, M_DP_NA_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
        InformationObject io = NULL;
        for(uint8_t i = 0;i<uxNumberOfItems;i++)
        {
            xQueueReceive( CanNetwork->In32FloatEventQueue, &item, 0 );
            io = (InformationObject) DoublePointInformation_create((DoublePointInformation)io,
                    EXTERNAL_DISCRETE_START_ADDR+(item.address-1)*EXTERNAL_DEV_DISCRETE_CNT+item.order,
                    (DoublePointValue)item.val,
                    IEC60870_QUALITY_GOOD);
            ASDU_addInformationObject(Asdu, io);
        }
        InformationObject_destroy(io);
        Slave_enqueueASDU(slave, Asdu);				
			}
    }
}

static void prvvProcessInternalEvent(void)
{
    UBaseType_t uxNumberOfItems = uxQueueMessagesWaiting(RawEventQueue); //Return the number of items that are currently held in Q
    if(uxNumberOfItems){
        DiscreteInfo item;
        struct sCP56Time2a timestamp;
        while(uxNumberOfItems--){
            xQueueReceive( RawEventQueue, &item, 0 );

            if(T104Slave_getOpenConnections(slave)){
                if(1 == env.LogicalDiscreteIndividual[item.flag].GenEvent)
								{
									if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
									{
                    ASDU Asdu = ASDU_create(connectionParameters, M_DP_NA_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
                    InformationObject io = (InformationObject) DoublePointInformation_create(NULL,
                        INTERNAL_DISCRETE_START_ADDR + item.flag,
                        (DoublePointValue)item.val,
                        IEC60870_QUALITY_GOOD);
                    ASDU_addInformationObject(Asdu, io);
                    InformationObject_destroy(io);
                    Slave_enqueueASDU(slave, Asdu);										
									}
									else if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
									{
                    ASDU Asdu = ASDU_create(connectionParameters, M_SP_NA_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
                    InformationObject io = (InformationObject) SinglePointInformation_create(NULL,
                        INTERNAL_DISCRETE_START_ADDR + item.flag,
                        item.val&1,
                        IEC60870_QUALITY_GOOD);
                    ASDU_addInformationObject(Asdu, io);
                    InformationObject_destroy(io);
                    Slave_enqueueASDU(slave, Asdu);										
									}

                }
            }

            if(1 == env.LogicalDiscreteIndividual[item.flag].GenSOE)
						{
                CP56Time2a_createFromMsTimestamp(&timestamp, item.timestamp);
								if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_SP)
								{
									ASDU Asdu = ASDU_create(connectionParameters, M_SP_TB_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
									InformationObject io = (InformationObject) SinglePointWithCP56Time2a_create( NULL,
												INTERNAL_DISCRETE_START_ADDR + item.flag,
												item.val&1,
												IEC60870_QUALITY_GOOD,
												&timestamp);
									ASDU_addInformationObject(Asdu, io);
									InformationObject_destroy(io);
									Slave_enqueueASDU(slave, Asdu);									
								}
								else if( env.LogicalDiscreteGlobal.ObjectMode == IEC104_OI_MODE_DP)
								{
									ASDU Asdu = ASDU_create(connectionParameters, M_DP_TB_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
									InformationObject io = (InformationObject) DoublePointWithCP56Time2a_create( NULL,
												INTERNAL_DISCRETE_START_ADDR + item.flag,
												(DoublePointValue)item.val,
												IEC60870_QUALITY_GOOD,
												&timestamp);
									ASDU_addInformationObject(Asdu, io);
									InformationObject_destroy(io);
									Slave_enqueueASDU(slave, Asdu);									
								}

            }
        }

//        InformationObject dp = NULL;
//        ASDU dpAsdu = ASDU_create(connectionParameters, M_DP_NA_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
//
//        InformationObject soe = NULL;
//        ASDU soeAsdu = ASDU_create(connectionParameters, M_DP_TB_1, false, SPONTANEOUS, 0, env.IEC104Env.address, false, false);
//
//        struct sCP56Time2a timestamp;
//        for(uint8_t i = 0;i<uxNumberOfItems;i++)
//        {
//            xQueueReceive(RawEventQueue, &item, 0 );
//
//            if(T104Slave_getOpenConnections(slave))
//            {
//                if(1 == env.LogicalDiscreteIndividual[item.flag].GenEvent)
//                {
//                    dp = (InformationObject) DoublePointInformation_create((DoublePointInformation)dp,
//                        INTERNAL_DISCRETE_START_ADDR + item.flag,
//                        (DoublePointValue)item.val,
//                        IEC60870_QUALITY_GOOD);
//                    ASDU_addInformationObject(dpAsdu, dp);
//                }
//            }
//
//            if(1 == env.LogicalDiscreteIndividual[item.flag].GenSOE)
//            {
//                CP56Time2a_createFromMsTimestamp(&timestamp, item.timestamp);
//                soe = (InformationObject) DoublePointWithCP56Time2a_create((DoublePointWithCP56Time2a)soe,
//                      INTERNAL_DISCRETE_START_ADDR + item.flag,
//                      (DoublePointValue)item.val,
//                      IEC60870_QUALITY_GOOD,
//                      &timestamp);
//                ASDU_addInformationObject(soeAsdu, soe);
//            }
//        }
//
//        InformationObject_destroy(dp);
//        Slave_enqueueASDU(slave, dpAsdu);
//
//        InformationObject_destroy(soe);
//        Slave_enqueueASDU(slave, soeAsdu);
    }
}

static void handleSelectSwitchTimeout(void)
{
    if( COIL_MODE_SBO == env.BasicEnv.CoilOutputMode) {
			if((GPIO_PIN_RESET == HAL_GPIO_ReadPin(SELECT_FB_GPIO_Port,SELECT_FB_Pin))
							&&((TimeSelectOn + env.IEC104Env.SBO_Deadband*1000) < HAL_GetTick())){
					HAL_GPIO_WritePin(SELECT_DO_GPIO_Port,SELECT_DO_Pin,GPIO_PIN_RESET);
					uint8_t i;
					for(i=0;i<env.BasicEnv.InternalCoilCNT;i++){
							env.LogicalCoilIndividual[i].Selected = 0;
					}
			}
		}
}

static void vTaskIEC104(void const * argument)
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
        printf("Starting server failed!\n");
    }
    
    while(1){  
				handleSelectSwitchTimeout();
        prvvProcessInternalEvent();
				if(env.BasicEnv.CanDevCount > 0)
				{
						prvvProcessExternalIn8Event();
						prvvProcessExternalIn32FloatEvent();
				}
        
    }
}

/* Exported functions --------------------------------------------------------*/
void IEC104_init(void)
{
    if(1 == env.BasicEnv.iec104_enalbe)
    {
        osThreadDef(IEC104, vTaskIEC104, osPriorityNormal, 0, 1024);
        taskIEC104Handle = osThreadCreate(osThread(IEC104), NULL);
    }
}
