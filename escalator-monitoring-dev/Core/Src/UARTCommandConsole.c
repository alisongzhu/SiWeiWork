/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * NOTE:  This file uses a third party USB CDC driver.
 */

/* Standard includes. */
#include "string.h"
#include "stdio.h"

#include "UARTCommandConsole.h"
#include "semphr.h"
/* Example includes. */
#include "FreeRTOS_CLI.h"
#include "rtc.h"
/* Demo application includes. */
#include "serial.h"
#include "flashdef.h"
#define TELNET
#ifdef TELNET
/* ----------------------- lwIP includes ------------------------------------*/
#include "lwip/api.h"
#include "lwip/tcp.h"
#define TELNET_PORT 23
#endif
/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE 50

/* Dimentions a buffer to be used by the UART driver, if the UART driver uses a
buffer at all. */
#define cmdQUEUE_LENGTH 25

/* DEL acts as a backspace. */
#define cmdASCII_DEL (0x7F)

/* The maximum time to wait for the mutex that guards the UART to become
available. */
#define cmdMAX_MUTEX_WAIT pdMS_TO_TICKS(300)

#ifndef configCLI_BAUD_RATE
#define configCLI_BAUD_RATE 115200
#endif

/*-----------------------------------------------------------*/

/*
 * The task that implements the command console processing.
 */
static void prvUARTCommandConsoleTask(void *pvParameters);
#ifdef TELNET
static void prvNetCommandConsoleTask(void *pvParameters);
static err_t NetCLIHandle(struct netconn *conn);
#endif

/*-----------------------------------------------------------*/

/* Const messages output by the command console. */
static const char *const pcWelcomeMessage = "FreeRTOS command server.\r\nType Help to view a list of registered commands.\r\n\r\n>";
static const char *const pcEndOfOutputMessage = "\r\n[Press ENTER to execute the previous command again]\r\n>";
static const char *const pcNewLine = "\r\n";
//const char FirmwareVersion[]  __attribute__((section(".envdata"))) ="sys_escalator 1.0" ;
__IO const struct DevLogdef  devinfo __attribute__((section(".envdata")))={
		.flag = pdTRUE,
		.version= 1,
		.BuildDate= __DATE__	,
		.BuildTime = __TIME__
};
/* Used to guard access to the UART in case messages are sent to the UART from
more than one task. */
static SemaphoreHandle_t xTxMutex = NULL;

/* The handle to the UART port, which is not used by all ports. */
static xComPortHandle xPort = 0;

/*-----------------------------------------------------------*/

void vUARTCommandConsoleStart(uint16_t usStackSize, UBaseType_t uxPriority)
{
	/* Create the semaphore used to access the UART Tx. */
	xTxMutex = xSemaphoreCreateMutex();
	configASSERT(xTxMutex);

	/* Create that task that handles the console itself. */
	xTaskCreate(prvUARTCommandConsoleTask, /* The task that implements the command console. */
				"CLI",					   /* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
				usStackSize,			   /* The size of the stack allocated to the task. */
				NULL,					   /* The parameter is not used, so NULL is passed. */
				uxPriority,				   /* The priority allocated to the task. */
				NULL);					   /* A handle is not required, so just pass NULL. */
										   /* Create that task that handles the console itself. */
#ifdef TELNET
	xTaskCreate(prvNetCommandConsoleTask, /* The task that implements the command console. */
				"TELNET",				  /* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
				usStackSize,			  /* The size of the stack allocated to the task. */
				NULL,					  /* The parameter is not used, so NULL is passed. */
				uxPriority,				  /* The priority allocated to the task. */
				NULL);					  /* A handle is not required, so just pass NULL. */
#endif
}
/*-----------------------------------------------------------*/

static void prvNetCommandConsoleTask(void *pvParameters)
{
	struct netconn *NetCLI, *NewNetCLI;
	err_t err;
	char *pcOutputString;
	NetCLI = netconn_new(NETCONN_TCP);
	uint32_t LinkTime;
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();
	if (NetCLI != NULL)
	{
		//		err = pdFALSE;
		netconn_bind(NetCLI, NULL, TELNET_PORT);
		netconn_listen(NetCLI);
		while (1)
		{
			err = netconn_accept(NetCLI, &NewNetCLI);
			if (err == ERR_OK)
			{
				snprintf(pcOutputString,configCOMMAND_INT_MAX_OUTPUT_SIZE,"version：%d\r\nfirmware Date:%s %s\r\n",devinfo.version,devinfo.BuildDate,devinfo.BuildTime);
				netconn_write(NewNetCLI, pcOutputString, strlen(pcOutputString), NETCONN_COPY);
				Show_RTC_Calendar(pcOutputString,configCOMMAND_INT_MAX_OUTPUT_SIZE);
				netconn_write(NewNetCLI, pcOutputString, strlen(pcOutputString), NETCONN_COPY);
				LinkTime = HAL_GetTick();
				while (1)
				{
					err = netconn_err(NewNetCLI);
					if (err != ERR_OK)
					{
						netconn_close(NewNetCLI);
						netconn_delete(NewNetCLI);
						break;
					}
					err = NetCLIHandle(NewNetCLI);
					if(err != ERR_OK)
					{
						netconn_close(NewNetCLI);
						netconn_delete(NewNetCLI);
					}

				}
			}
		}
		netconn_close(NetCLI);
		netconn_delete(NetCLI);
	}
	while (1)
		;
}

static err_t NetCLIHandle(struct netconn *conn)
{
	err_t err;
	struct netbuf *inbuf;
	static char cInputString[cmdMAX_INPUT_SIZE], cLastInputString[cmdMAX_INPUT_SIZE];
	char *buf;
	uint16_t Inbuflen;
	uint8_t i;
	BaseType_t xReturned;
	char *pcOutputString;
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();
	memset(cInputString, 0x00, cmdMAX_INPUT_SIZE);
	err = netconn_recv(conn, &inbuf);
	if (err == ERR_OK)
	{

		netbuf_data(inbuf, (void **)&buf, &Inbuflen);
		if (Inbuflen)
		{
			for (i = 0; i < Inbuflen; i++)
			{
				if (buf[i] == '\r' || buf[i] == '\n')
				{
					cInputString[i] = '\0';
				}
				else
				{
					cInputString[i] = buf[i];
				}
			}
			if (strlen(cInputString))
			{
				strcpy(cLastInputString, cInputString);
			}
			else
			{
				strcpy(cInputString, cLastInputString);
			}
			do
			{
				/* Get the next output string from the command interpreter. */
				xReturned = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

				/* Write the generated string to the UART. */
				// vSerialPutString( xPort, ( signed char * ) pcOutputString, ( unsigned short ) strlen( pcOutputString ) );
				netconn_write(conn, pcOutputString, strlen(pcOutputString), NETCONN_COPY);

			} while (xReturned != pdFALSE);
		}
		netbuf_delete(inbuf);
	}

	return err;
}
static void prvUARTCommandConsoleTask(void *pvParameters)
{
	signed char cRxedChar;
	uint8_t ucInputIndex = 0;
	char *pcOutputString;
	static char cInputString[cmdMAX_INPUT_SIZE], cLastInputString[cmdMAX_INPUT_SIZE];
	BaseType_t xReturned;
	xComPortHandle xPort;

	(void)pvParameters;

	/* Obtain the address of the output buffer.  Note there is no mutual
	exclusion on this buffer as it is assumed only one command console interface
	will be used at any one time. */
	pcOutputString = FreeRTOS_CLIGetOutputBuffer();

	/* Initialise the UART. */
	xPort = xSerialPortInitMinimal(configCLI_BAUD_RATE, cmdQUEUE_LENGTH);

	/* Send the welcome message. */
	vSerialPutString(xPort, (signed char *)pcWelcomeMessage, (unsigned short)strlen(pcWelcomeMessage));
	snprintf(pcOutputString,configCOMMAND_INT_MAX_OUTPUT_SIZE,"version：%d\r\n  Date:%s %s\r\n",devinfo.version,devinfo.BuildDate,devinfo.BuildTime);
	vSerialPutString(xPort, (signed char *)pcOutputString, (unsigned short)strlen(pcOutputString));
	CmdLine_Input("read_ip");
	for (;;)
	{
		/* Wait for the next character.  The while loop is used in case
		INCLUDE_vTaskSuspend is not set to 1 - in which case portMAX_DELAY will
		be a genuine block time rather than an infinite block time. */
		while (xSerialGetChar(xPort, &cRxedChar, portMAX_DELAY) != pdPASS)
			;

		/* Ensure exclusive access to the UART Tx. */
		if (xSemaphoreTake(xTxMutex, cmdMAX_MUTEX_WAIT) == pdPASS)
		{
			/* Echo the character back. */
			xSerialPutChar(xPort, cRxedChar, portMAX_DELAY);

			/* Was it the end of the line? */
			if (cRxedChar == '\n' || cRxedChar == '\r')
			{
				/* Just to space the output from the input. */
				vSerialPutString(xPort, (signed char *)pcNewLine, (unsigned short)strlen(pcNewLine));

				/* See if the command is empty, indicating that the last command
				is to be executed again. */
				if (ucInputIndex == 0)
				{
					/* Copy the last command back into the input string. */
					strcpy(cInputString, cLastInputString);
				}

				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns
				pdFALSE	(indicating there is no more output) as it might
				generate more than one string. */
				do
				{
					/* Get the next output string from the command interpreter. */
					xReturned = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

					/* Write the generated string to the UART. */
					vSerialPutString(xPort, (signed char *)pcOutputString, (unsigned short)strlen(pcOutputString));

				} while (xReturned != pdFALSE);

				/* All the strings generated by the input command have been
				sent.  Clear the input string ready to receive the next command.
				Remember the command that was just processed first in case it is
				to be processed again. */
				strcpy(cLastInputString, cInputString);
				ucInputIndex = 0;
				memset(cInputString, 0x00, cmdMAX_INPUT_SIZE);

				vSerialPutString(xPort, (signed char *)pcEndOfOutputMessage, (unsigned short)strlen(pcEndOfOutputMessage));
			}
			else
			{
				if (cRxedChar == '\r')
				{
					/* Ignore the character. */
				}
				else if ((cRxedChar == '\b') || (cRxedChar == cmdASCII_DEL))
				{
					/* Backspace was pressed.  Erase the last character in the
					string - if any. */
					if (ucInputIndex > 0)
					{
						ucInputIndex--;
						cInputString[ucInputIndex] = '\0';
					}
				}
				else
				{
					/* A character was entered.  Add it to the string entered so
					far.  When a \n is entered the complete	string will be
					passed to the command interpreter. */
					if ((cRxedChar >= ' ') && (cRxedChar <= '~'))
					{
						if (ucInputIndex < cmdMAX_INPUT_SIZE)
						{
							cInputString[ucInputIndex] = cRxedChar;
							ucInputIndex++;
						}
					}
				}
			}

			/* Must ensure to give the mutex back. */
			xSemaphoreGive(xTxMutex);
		}
	}
}
/*-----------------------------------------------------------*/

void vOutputString(const char *const pcMessage)
{
	if (xSemaphoreTake(xTxMutex, cmdMAX_MUTEX_WAIT) == pdPASS)
	{
		vSerialPutString(xPort, (signed char *)pcMessage, (unsigned short)strlen(pcMessage));
		xSemaphoreGive(xTxMutex);
	}
}

/*-----------------------------------------------------------*/
