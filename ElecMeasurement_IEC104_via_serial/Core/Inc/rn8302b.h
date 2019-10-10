/*
 * rb8302b.h
 *
 *  Created on: 2018年12月10日
 *      Author: alien
 */

#ifndef CORE_INC_RN8302B_H_
#define CORE_INC_RN8302B_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

//#include <cmsis_os.h>
#include <spi.h>
/* Macros --------------------------------------------------------------------*/
#define READ_LIST_SIZE    (20)

/* Types ---------------------------------------------------------------------*/
//typedef int (*rn8302b_cs)(bool enable );

//struct rn8302b_priv{
//  SPI_HandleTypeDef *hspi;
//  rn8302b_cs  cs;
//};

typedef union {
  uint8_t  ucTempBuf[8];
  uint32_t lTemp32;
  uint16_t wTemp16;
  uint8_t  ucTemp8;
}sRN8302StruDataComm_TypeDef;

typedef struct {
  uint16_t LostVoltage;
  uint16_t pad;
  uint32_t IRmsConst;
  uint32_t VRmsConst;
  uint32_t PRmsConst;
}sRN8302RatioPara_TypeDef;

typedef struct {
  uint8_t PHSU[3];
  uint8_t pad;
  uint16_t  VGain[3];
  uint16_t  IGain[4];
} sRN8302FirmParaFile_TypeDef;

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/
extern float RN8302B_DataTable_Front[READ_LIST_SIZE];


/* Function Prototypes -------------------------------------------------------*/
void RN8302B_init(sRN8302FirmParaFile_TypeDef *RN8302FirmPara );
void rn8302b_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE_INC_RN8302B_H_ */
