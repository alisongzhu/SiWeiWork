/*
 * uset_CLI_handle.c
 *
 *  Created on: 2019年3月29日
 *      Author: pt
 */
 
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include"bsp_nvs_config.h"
#include "nvs/nvs.h"
#include "flash.h"
#include "rtc.h"
#include "FreeRTOS_CLI.h"
#include "string.h"
/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static BaseType_t prvSet_IP( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRead_IP( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvRestDev( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSetTime( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvReadTime( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvTest( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvfactory_reset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvSet_Network( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );

/* Private constants ---------------------------------------------------------*/
static const CLI_Command_Definition_t xSetNETCommand =
{
  "set_network",
  "\r\nset_network:\r\n Set ip is address, subnet mask, default gateway(set_ip 192 168 1 111 255 255 255 0 192 168 1 1)\r\n",
  prvSet_Network,
  12
};
static const CLI_Command_Definition_t xSetIPCommand =
{
  "set_ip",
  "\r\nset_ip:\r\n Set ip is address, subnet mask, default gateway(set_ip 192 168 1 111 )\r\n",
  prvSet_IP,
  4
};
static const CLI_Command_Definition_t xReadIPCommand =
{
  "read_ip",
  "\r\nread_ip:\r\nRead IP is address, subnet mask, default gateway \r\n",
  prvRead_IP,
  0
};
static const CLI_Command_Definition_t xRestCommand =
{
  "reset",
  "\r\nreset:\r\n reset device\r\n",
  prvRestDev,
  0
};
static const CLI_Command_Definition_t xFactoryResetCommand =
{
  "factory_reset",
  "\r\nfactory_reset:\r\n factory reset\r\n",
  prvfactory_reset,
  0
};
static const CLI_Command_Definition_t xTestCommand =
{
  "test",
  "\r\ntest:\r\n test things\r\n",
  prvTest,
  0
};

static const CLI_Command_Definition_t xSetTimeCommand =
{
  "set_time",
  "\r\nset_time:\r\n set_time 19 1 1 2 14 23 30 (Year month day week hour minute second)\r\n",
  prvSetTime,
  7
};
static const CLI_Command_Definition_t xReadTimeCommand =
{
  "time",
  "\r\ntime:\r\n read time\r\n",
  prvReadTime,
  0
};
/* Private variables ---------------------------------------------------------*/


/* Global variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static BaseType_t prvfactory_reset( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	int rc;
	nvs_delete(&fs,NETWORK_IP_ADDRESS);
	nvs_delete(&fs,RN8302B_PARA_ID);
	nvs_delete(&fs,RN8302B_RATIO_PARA_ID);
	nvs_delete(&fs,RN8302B_DEADZONE_PARA_ID);
	nvs_delete(&fs,VOLTAGE_CALIBRATION_ID);
	nvs_delete(&fs,ANALOG_PRAR_ID);
	strncpy(pcWriteBuffer,"Clear configuration parameters and clear NVS\r\n",xWriteBufferLen);
	return pdFALSE;
}
static BaseType_t prvSet_IP( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  int rc;
  uint8_t i,*Pdata;
  Network_ParaDef dev_ip = { 0,0,0,0,255,255,255,0,192,168,1,1 };
  uint32_t xStringLength1 = 0;
  char * pcParameter;
  Pdata = &dev_ip;
  for(i = 0;i < 4 ;i++)
  {
		 pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i+1,(BaseType_t *)&xStringLength1);
		 *Pdata++=(uint8_t)strtol((const char *)pcParameter,NULL,10);
  }
	snprintf(pcWriteBuffer,xWriteBufferLen,"\r IP:%d %d %d %d\r\n "
			,dev_ip.IP_ADDRESS[0],dev_ip.IP_ADDRESS[1],dev_ip.IP_ADDRESS[2],dev_ip.IP_ADDRESS[3]);
	snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r mask:%d %d %d %d\r\n "
				,dev_ip.NETMASK_ADDRESS[0],dev_ip.NETMASK_ADDRESS[1],dev_ip.NETMASK_ADDRESS[2],dev_ip.NETMASK_ADDRESS[3]);
	snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r gateway:%d %d %d %d\r\n "
				,dev_ip.GATEWAY_ADDRESS[0],dev_ip.GATEWAY_ADDRESS[1],dev_ip.GATEWAY_ADDRESS[2],dev_ip.GATEWAY_ADDRESS[3]);
	rc = nvs_write(&fs, NETWORK_IP_ADDRESS, &dev_ip,sizeof(Network_ParaDef) );
return pdFALSE;
}
static BaseType_t prvSet_Network( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
  int rc;
  uint8_t i,*Pdata;
  Network_ParaDef dev_ip;
  uint32_t xStringLength1 = 0;
  char * pcParameter;
  Pdata = &dev_ip;
  for(i=0;i<sizeof(Network_ParaDef) ;i++)
  {
		 pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i+1,(BaseType_t *)&xStringLength1);
		 *Pdata++=(uint8_t)strtol((const char *)pcParameter,NULL,10);
  }
	snprintf(pcWriteBuffer,xWriteBufferLen,"\r IP:%d %d %d %d\r\n "
			,dev_ip.IP_ADDRESS[0],dev_ip.IP_ADDRESS[1],dev_ip.IP_ADDRESS[2],dev_ip.IP_ADDRESS[3]);
	snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r mask:%d %d %d %d\r\n "
				,dev_ip.NETMASK_ADDRESS[0],dev_ip.NETMASK_ADDRESS[1],dev_ip.NETMASK_ADDRESS[2],dev_ip.NETMASK_ADDRESS[3]);
	snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r gateway:%d %d %d %d\r\n "
				,dev_ip.GATEWAY_ADDRESS[0],dev_ip.GATEWAY_ADDRESS[1],dev_ip.GATEWAY_ADDRESS[2],dev_ip.GATEWAY_ADDRESS[3]);
	rc = nvs_write(&fs, NETWORK_IP_ADDRESS, &dev_ip,sizeof(Network_ParaDef) );
return pdFALSE;
}
static BaseType_t prvRead_IP( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	int rc;
	Network_ParaDef dev_ip;
	rc = nvs_read(&fs, NETWORK_IP_ADDRESS, &dev_ip,sizeof(Network_ParaDef) );
	if(rc > 0)
	{
		snprintf(pcWriteBuffer,xWriteBufferLen,"\r IP:%d %d %d %d\r\n "
				,dev_ip.IP_ADDRESS[0],dev_ip.IP_ADDRESS[1],dev_ip.IP_ADDRESS[2],dev_ip.IP_ADDRESS[3]);
		snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r mask:%d %d %d %d\r\n "
					,dev_ip.NETMASK_ADDRESS[0],dev_ip.NETMASK_ADDRESS[1],dev_ip.NETMASK_ADDRESS[2],dev_ip.NETMASK_ADDRESS[3]);
		snprintf(&pcWriteBuffer[strlen(pcWriteBuffer)-1],xWriteBufferLen,"\r gateway:%d %d %d %d\r\n "
					,dev_ip.GATEWAY_ADDRESS[0],dev_ip.GATEWAY_ADDRESS[1],dev_ip.GATEWAY_ADDRESS[2],dev_ip.GATEWAY_ADDRESS[3]);
	}
	else
	{
		strncpy(pcWriteBuffer,"ip:192.168.1.111 255.255.255.0 192.168.1.1\r\n",xWriteBufferLen);
	}
 return pdFALSE;
}
static BaseType_t prvRestDev( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
 HAL_NVIC_SystemReset();
 return pdFALSE;
}
static BaseType_t prvTest( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
 int rc;
 static uint16_t id =0;
 uint8_t Data[100]={0xA5,0xA5};
 while(1)
 {
 id++;
 rc = nvs_write(&fs, 1, &id,2 );

 }
 if(rc >= 0 )
 {
	 snprintf(pcWriteBuffer,xWriteBufferLen,"nvs= %d,sta= %d\r\n",id ,rc);
	 return pdTRUE;
 }
 else
 {
	strncpy(pcWriteBuffer,"error NVS\r\n",xWriteBufferLen);
 }
  if(id > 4000)
  {
	  id=0;
	  return pdTRUE;
  }
}
static BaseType_t prvSetTime( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
	uint8_t i,*Pdata;
 	uint32_t xStringLength1 = 0;
	char * pcParameter;
 	RTC_TimeTypeDef Time;
	RTC_DateTypeDef Date;
	Pdata = &Time; 
	for(i = 0 ; i< 3 ; i++)
	{
		pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)i+5,(BaseType_t *)&xStringLength1);
		*Pdata++=(uint8_t)strtol((const char *)pcParameter,NULL,16);
	}
	LL_RTC_DisableWriteProtection(RTC);
	Enter_RTC_InitMode();
	LL_RTC_TIME_Config(RTC,LL_RTC_TIME_FORMAT_AM_OR_24,Time.Hours,Time.Minutes,Time.Seconds);
	pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)1,(BaseType_t *)&xStringLength1);
	Date.Year=(uint8_t)strtol((const char *)pcParameter,NULL,16);
	pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)2,(BaseType_t *)&xStringLength1);
	Date.Month=(uint8_t)strtol((const char *)pcParameter,NULL,16);
	pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)3,(BaseType_t *)&xStringLength1);
	Date.Date=(uint8_t)strtol((const char *)pcParameter,NULL,16);
	pcParameter = (  char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString,(UBaseType_t)4,(BaseType_t *)&xStringLength1);
	Date.WeekDay=(uint8_t)strtol((const char *)pcParameter,NULL,16);

	LL_RTC_DATE_Config(RTC,Date.WeekDay,Date.Date,Date.Month,Date.Year);
	Exit_RTC_InitMode();
	LL_RTC_EnableWriteProtection(RTC);
	LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, RTC_BKP_DATE_TIME_UPDTATED);
	snprintf(pcWriteBuffer,xWriteBufferLen,"time:%X:%X:%X\r\n date:20%x %x %x week:%x \r\n",Time.Hours,Time.Minutes,Time.Seconds,Date.Year,Date.Month,Date.Date,Date.WeekDay);
	return pdFALSE;
}
static BaseType_t prvReadTime( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
// 	RTC_TimeTypeDef Time;
//	RTC_DateTypeDef Date;
//	Time.Hours = LL_RTC_TIME_GetHour(RTC);
//	Time.Minutes = LL_RTC_TIME_GetMinute(RTC);
//	Time.Seconds = LL_RTC_TIME_GetSecond(RTC);
//	Date.Date = LL_RTC_DATE_GetDay(RTC);
//	Date.Month = LL_RTC_DATE_GetMonth(RTC);
//	Date.Year = LL_RTC_DATE_GetYear(RTC);
//	Date.WeekDay = LL_RTC_DATE_GetWeekDay(RTC);
//	snprintf(pcWriteBuffer,xWriteBufferLen,"time:%X:%X:%X\r\n date:20%x %x %x week:%x \r\n",Time.Hours,Time.Minutes,Time.Seconds,Date.Year,Date.Month,Date.Date,Date.WeekDay);
	Show_RTC_Calendar(pcWriteBuffer,xWriteBufferLen);
	return pdFALSE;
}
/* Exported functions --------------------------------------------------------*/
void AddCommand_CLI(void)
{
	FreeRTOS_CLIRegisterCommand(&xSetIPCommand);
	FreeRTOS_CLIRegisterCommand(&xReadIPCommand);
	FreeRTOS_CLIRegisterCommand(&xSetNETCommand);
	FreeRTOS_CLIRegisterCommand(&xRestCommand);
//	FreeRTOS_CLIRegisterCommand(&xTestCommand);
	FreeRTOS_CLIRegisterCommand(&xReadTimeCommand);
	FreeRTOS_CLIRegisterCommand(&xSetTimeCommand);
	FreeRTOS_CLIRegisterCommand(&xFactoryResetCommand);

}
