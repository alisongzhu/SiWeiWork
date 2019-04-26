/*
 * analog_input.c
 *
 *  Created on: 2019年3月31日
 *      Author: pt
 */

/* Includes ------------------------------------------------------------------*/
#include "stdlib.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

#include "cmsis_os.h"
#include "main.h"
#include "tim.h"
#include "FreeRTOS_CLI.h"
#include "bsp_nvs_config.h"
#include "nvs/nvs.h"
#include "flash.h"

#include "user_regs.h"
#include "analog_input.h"
/* Private macros ------------------------------------------------------------*/
#define INPUT_SCAN_TIME (1000 - 1)
#define DEFUALT_MIN_COUNTER_NUM (9817)
#define DEFUALT_MAX_COUNTER_NUM (73181)
#define VOLTAGE_MEASURING_RANGE (30.0)
#define CALI_LIMIT              (0.2f)
/* Private types -------------------------------------------------------------*/
typedef struct AnalogParadef
{
	uint8_t flag[REG_INPUT_ANALOG_LIST_SIZE];
	uint32_t MinCounter[REG_INPUT_ANALOG_LIST_SIZE];
	uint32_t MaxCounter[REG_INPUT_ANALOG_LIST_SIZE];
} AnalogParadef;
/* Private constants ---------------------------------------------------------*/
static BaseType_t prvRead_Voltage(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvCalibration_Voltage(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

static const CLI_Command_Definition_t xReadVoltageCommand =
		{
				"vol",
				"\r\nvol:\r\nReads the voltage of the specified circuit(e.g.:volt 1-32/all) \r\n",
				prvRead_Voltage,
				1};
static const CLI_Command_Definition_t xCalibrationVoltageCommand =
		{
				"c_vol",
				"\r\nc_vol:\r\nCalibrate the voltage of the specified circuit(e.g.:c_vol 1-32/all min/max) \r\n",
				prvCalibration_Voltage,
				2};
/* Private variables ---------------------------------------------------------*/
static uint8_t analog_channel_map[8] = {2, 1, 0, 3, 4, 6, 7, 5};
static uint32_t *analogs = NULL;
static QueueHandle_t xQueueCounter = NULL;
static SemaphoreHandle_t xSemaphoreAnalog = NULL;
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void hc595_output(uint8_t data);
static void Select_one_input(uint8_t InputNum);
static void ReadAnalogInputCounter(uint8_t InputNum, uint32_t *counter);
static void ReadAnalogInputVoltage(uint8_t InputNum, uint32_t *Voltage);
/* Private functions ---------------------------------------------------------*/
static void hc595_output(uint8_t data)
{
	//  taskENTER_CRITICAL();
	//  LL_GPIO_ResetOutputPin(HC595_OVERRIDING_CLEAR_GPIO_Port,HC595_OVERRIDING_CLEAR_Pin);
	LL_GPIO_ResetOutputPin(HC595_STORAGE_REG_CLK_GPIO_Port, HC595_STORAGE_REG_CLK_Pin);
	LL_GPIO_ResetOutputPin(HC595_SHIFT_REG_CLK_GPIO_Port, HC595_SHIFT_REG_CLK_Pin);

	//  LL_GPIO_SetOutputPin(HC595_OVERRIDING_CLEAR_GPIO_Port,HC595_OVERRIDING_CLEAR_Pin);

	for (uint8_t i = 0; i < 8; i++)
	{
		if (data & (1 << (7 - i)))
		{
			LL_GPIO_SetOutputPin(HC595_OUTPUT_GPIO_Port, HC595_OUTPUT_Pin);
		}
		else
		{
			LL_GPIO_ResetOutputPin(HC595_OUTPUT_GPIO_Port, HC595_OUTPUT_Pin);
		}
		HAL_Delay(1);
		LL_GPIO_SetOutputPin(HC595_SHIFT_REG_CLK_GPIO_Port, HC595_SHIFT_REG_CLK_Pin);
		HAL_Delay(1);
		LL_GPIO_ResetOutputPin(HC595_SHIFT_REG_CLK_GPIO_Port, HC595_SHIFT_REG_CLK_Pin);
	}
	LL_GPIO_SetOutputPin(HC595_STORAGE_REG_CLK_GPIO_Port, HC595_STORAGE_REG_CLK_Pin);

	//  taskEXIT_CRITICAL();
}

static void Select_one_input(uint8_t InputNum)
{
	uint8_t chip_no = 0;
	uint8_t channel = 0;
	uint8_t hc595_data = 0;
	chip_no = InputNum / 8;
	channel = analog_channel_map[InputNum % 8];
	hc595_data = (channel | 0xf8) & (~(1 << (6 - chip_no)));
	hc595_output(hc595_data);
}

static void ReadAnalogInputCounter(uint8_t InputNum, uint32_t *counter)
{
	xSemaphoreTake(xSemaphoreAnalog, portMAX_DELAY);
	Select_one_input(InputNum);
	LL_TIM_SetCounter(TIM2, 0);
	LL_TIM_SetCounter(TIM6, 0);
	xQueueReceive(xQueueCounter, counter, 0);
	taskENTER_CRITICAL();
	LL_TIM_EnableCounter(TIM2);
	LL_TIM_EnableCounter(TIM6);
	taskEXIT_CRITICAL();
	//	xTaskNotifyWait(0 ,0xffffffff, counter, portMAX_DELAY);
	xQueueReceive(xQueueCounter, counter, portMAX_DELAY);
	xSemaphoreGive(xSemaphoreAnalog);
}

static void ReadAnalogInputVoltage(uint8_t InputNum, uint32_t *Voltage)
{
	int rc;
	AnalogParadef InputPara;
	uint32_t counter = 0, MinCounter, MaxCounter;
	float Vdata;

	rc = nvs_read(&fs, ANALOG_PRAR_ID, &InputPara, sizeof(AnalogParadef));
	if (rc < 0 || InputPara.flag[InputNum] != pdTRUE)
	{
		MinCounter = DEFUALT_MIN_COUNTER_NUM;
		MaxCounter = DEFUALT_MAX_COUNTER_NUM;
	}
	else
	{
		MinCounter = InputPara.MinCounter[InputNum];
		MaxCounter = InputPara.MaxCounter[InputNum];
	}
	ReadAnalogInputCounter(InputNum, &counter);

	if (counter < MinCounter)
	{
		counter = MinCounter;
	}
	else if (counter > MaxCounter)
	{
		counter = MaxCounter;
	}
	Vdata = ((counter - MinCounter) * VOLTAGE_MEASURING_RANGE) / (MaxCounter - MinCounter);
	*Voltage = (uint32_t)(Vdata * 100);
}
static BaseType_t prvRead_Voltage(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t xStringLength1 = 0;
	BaseType_t xReturn = pdFALSE;
	static uint8_t Input = 0;
	char *pcParameter;
	uint32_t Voltage,counter;
	xTaskNotifyGive( analog_scan_handler );/* Suspend the main poll */
	pcParameter = (char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString, (UBaseType_t)1, (BaseType_t *)&xStringLength1);
	if (memcmp(pcParameter, "all", 3) == 0)
	{
		ReadAnalogInputVoltage(Input, &Voltage);
		ReadAnalogInputCounter(Input, &counter);
		xReturn = pdTRUE;
	}
	else
	{
		Input = (uint8_t)strtol((const char *)pcParameter, NULL, 10);
		ReadAnalogInputVoltage(Input, &Voltage);
		ReadAnalogInputCounter(Input, &counter);
		xReturn = pdFALSE;
	}
	snprintf(pcWriteBuffer, xWriteBufferLen, "\rVoltage %d =%dmV  %d\r\n ", Input, (int)(Voltage)*10,counter);
	if (++Input >= REG_INPUT_ANALOG_LIST_SIZE)
	{
		xReturn = pdFALSE;
	}
	if (xReturn == pdFALSE)
		Input = 0;
	return xReturn;
}

static BaseType_t prvCalibration_Voltage(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	uint32_t xStringLength1 = 0;
	BaseType_t xReturn = pdFALSE;
	static uint8_t InputNum = 0;
	char *pcParameter;
	uint32_t counter,MinLimit,MaxLimit;
	AnalogParadef InputPara;
	memset(&InputPara,sizeof(AnalogParadef),pdFALSE);
	xTaskNotifyGive( analog_scan_handler );/* Suspend the main poll */
	pcParameter = (char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString, (UBaseType_t)1, (BaseType_t *)&xStringLength1);
	if (memcmp(pcParameter, "all", 3) == 0)
	{
	 xReturn = pdTRUE;
	}
	else
	{
		InputNum = (uint8_t)strtol((const char *)pcParameter, NULL, 10);
		xReturn = pdFALSE;
	}
	ReadAnalogInputCounter(InputNum, &counter);
	pcParameter = (char *)FreeRTOS_CLIGetParameter((const char *)pcCommandString, (UBaseType_t)2, (BaseType_t *)&xStringLength1);
	nvs_read(&fs, ANALOG_PRAR_ID, &InputPara, sizeof(AnalogParadef));
	MinLimit = abs(counter - DEFUALT_MIN_COUNTER_NUM);
	MaxLimit = abs(counter - DEFUALT_MAX_COUNTER_NUM);
	if (memcmp(pcParameter, "min", 3) == 0 && MinLimit < CALI_LIMIT*DEFUALT_MIN_COUNTER_NUM)
	{
		InputPara.MinCounter[InputNum] = counter;

	}
	else if (memcmp(pcParameter, "max", 3) == 0 && MaxLimit < CALI_LIMIT*DEFUALT_MAX_COUNTER_NUM )
	{
		InputPara.MaxCounter[InputNum] = counter;

	}

	MinLimit = abs(InputPara.MinCounter[InputNum] - DEFUALT_MIN_COUNTER_NUM);
	MaxLimit = abs(InputPara.MaxCounter[InputNum] - DEFUALT_MAX_COUNTER_NUM);
	if(MaxLimit < CALI_LIMIT*DEFUALT_MAX_COUNTER_NUM && MinLimit < CALI_LIMIT*DEFUALT_MIN_COUNTER_NUM)
	{
		InputPara.flag[InputNum] = pdTRUE;
	}
	else
	{
		InputPara.flag[InputNum] = pdFALSE;
	}
	snprintf(pcWriteBuffer, xWriteBufferLen, "\r %d: min=%d max=%d  flag =%d\r\n ", InputNum, InputPara.MinCounter[InputNum], InputPara.MaxCounter[InputNum],InputPara.flag[InputNum]);
	nvs_write(&fs, ANALOG_PRAR_ID, &InputPara, sizeof(AnalogParadef));
	if (++InputNum >= REG_INPUT_ANALOG_LIST_SIZE)
	{
		xReturn = pdFALSE;
	}
	if (xReturn == pdFALSE)
		InputNum = 0;
	return xReturn;
}

/* Exported functions --------------------------------------------------------*/
void AnalogInputInit(void)
{
	xQueueCounter = xQueueCreate(1, sizeof(uint32_t));
	xSemaphoreAnalog = xSemaphoreCreateMutex();
	MX_TIM2_Init();
	MX_TIM6_Init();
	LL_TIM_SetAutoReload(TIM6, INPUT_SCAN_TIME);
	LL_TIM_EnableIT_UPDATE(TIM6);
	FreeRTOS_CLIRegisterCommand(&xReadVoltageCommand);
	FreeRTOS_CLIRegisterCommand(&xCalibrationVoltageCommand);
	xSemaphoreGive(xSemaphoreAnalog);
}

void AnalogScan(uint32_t *Rdata)
{
	uint i;
	BaseType_t xEvent;
	analogs = Rdata;
	for (i = 0; i < REG_INPUT_ANALOG_LIST_SIZE; i++)
	{
		ReadAnalogInputVoltage(i, &analogs[i]);
	}
	xEvent=ulTaskNotifyTake( pdTRUE, 0);
	if(xEvent)
	{
		vTaskDelay(5000);/* sleep */
	}
	vTaskDelay(10);
}

void TIM6_DAC_IRQHandler(void)
{
	uint32_t counter;
	counter = LL_TIM_GetCounter(TIM2);

	LL_TIM_ClearFlag_UPDATE(TIM6);

	LL_TIM_DisableCounter(TIM6);
	LL_TIM_DisableCounter(TIM2);

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	//  xTaskNotifyFromISR( analog_scan_handler, counter,eSetValueWithoutOverwrite , &xHigherPriorityTaskWoken );
	xQueueSendFromISR(xQueueCounter, &counter, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
