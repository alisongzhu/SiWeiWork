
/**
  ******************************************************************************
  * File Name          : flash.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
	#include "Flash.h"
	#include "string.h"
void save_config(Device_Config_DataTypeDef *Config_Data )
{
	uint16_t  *Config_data;
	uint8_t i;
	uint32_t Address = 0, PAGEError = 0;
	HAL_FLASH_Unlock();	
	FLASH_EraseInitTypeDef EraseInitStruct;
 

  /* Fill EraseInit structure*/
  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = FLASH_PROGRAM;
  EraseInitStruct.NbPages     = FLASH_ERASE_NBPAGES;  
  
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PAGEError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
    printf("Error occurred while page erase.");
  }
  
  Address = FLASH_PROGRAM;
  Config_data=(uint16_t*)Config_Data;
 
 
    for(i=0;i<sizeof(Device_Config_DataTypeDef);i=i+2)
		{
		  HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD , Address,*Config_data);
      Address = Address + 2;
	    Config_data=Config_data+1;
		
		}

  
  HAL_FLASH_Lock();
}
void load_configuration(Device_Config_DataTypeDef *config_Data)
{

  uint16_t* read_addr = ( uint16_t*)FLASH_PROGRAM;
  memcpy(config_Data,read_addr,sizeof(Device_Config_DataTypeDef));

}
