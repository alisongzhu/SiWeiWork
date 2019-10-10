/*
 *  Copyright 2016 MZ Automation GmbH
 *
 *  This file is part of lib60870-C
 *
 *  lib60870-C is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lib60870-C is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lib60870-C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  See COPYING file for the complete license text.
 */

#include "hal_thread.h"
#include "cmsis_os.h"
#include "lib_memory.h"
//#include "freertos.h"
//#include "task.h"
#include "lib60870_internal.h"

#define IEC104_SERVER_THREAD_STACK_SIZE  512
#define IEC104_AUTOMATIC_THREAD_STACK_SIZE  512
#define IEC104_THREAD_PRIORITY    osPriorityNormal //osPriorityNormal
struct sThread
{
//  osThreadDef_t          osThread;
  os_pthread pthread;      ///< start address of thread function
//  osPriority             tpriority;    ///< initial thread priority
//  uint32_t               instances;    ///< maximum number of instances of that thread function
//  uint32_t               stacksize;    ///< stack size requirements in bytes; 0 is default stack size
  void *parameter;
  osThreadId handle;
  bool autodestroy;
};

Semaphore
Semaphore_create (int initialValue)
{
//	Semaphore semaphore_def = GLOBAL_MALLOC(sizeof(Semaphore));
  return (Semaphore) osSemaphoreCreate (NULL, initialValue);
}

/* Wait until semaphore value is more than zero. Then decrease the semaphore value. */
void
Semaphore_wait (Semaphore self)
{
  osSemaphoreWait ((osSemaphoreId) self, osWaitForever);
}

void
Semaphore_post (Semaphore self)
{
  osSemaphoreRelease ((osSemaphoreId) self);
}

void
Semaphore_destroy (Semaphore self)
{
  osSemaphoreDelete ((osSemaphoreId) self);
}

Thread
Thread_create (ThreadExecutionFunction function, void* parameter,
	       bool autodestroy)
{
  Thread thread = (Thread) GLOBAL_MALLOC(sizeof(struct sThread));

  if (thread != NULL)
    {
      thread->pthread = (os_pthread) function;
      thread->parameter = parameter;
      thread->autodestroy = autodestroy;
    }

  return thread;

}

static void
destroyAutomaticThread (void* parameter)
{
  Thread thread = (Thread) parameter;

  thread->pthread (thread->parameter);

  Thread_destroy (thread);

}

void
Thread_start (Thread thread)
{
  if (thread->autodestroy == true)
    {
      osThreadDef(thread_autodestroy, (os_pthread )destroyAutomaticThread,
		  IEC104_THREAD_PRIORITY, 0,
		  IEC104_AUTOMATIC_THREAD_STACK_SIZE);
      thread->handle = osThreadCreate (osThread(thread_autodestroy), thread);
    }
  else
    {
      osThreadDef(iec104_Svr, (os_pthread )thread->pthread,
		  IEC104_THREAD_PRIORITY, 0, IEC104_SERVER_THREAD_STACK_SIZE);
      thread->handle = osThreadCreate (osThread(iec104_Svr), thread->parameter);
    }
}

void
Thread_destroy (Thread thread)
{
  GLOBAL_FREEMEM(thread);
  osThreadTerminate (NULL);
}

void
Thread_sleep (int millies)
{
  osDelay (millies);
}
