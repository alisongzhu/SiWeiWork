// Header:
// File Name: DM8606C.C
// Author:LH
// Date: Jul 26 2017

#include <stdint.h>
#include "DM8606C.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

extern ETH_HandleTypeDef heth;

#if DM8606C_SUPERVISE
uint32_t P0_Regs[32] = { 0 };
uint32_t P1_Regs[32] = { 0 };
uint32_t P2_Regs[32] = { 0 };
uint32_t P3_Regs[32] = { 0 };
uint32_t P4_Regs[32] = { 0 };
uint32_t P5_Regs[32] = { 0 };

uint32_t Chip_Regs[32] = { 0 };

uint32_t System_Regs[32] = { 0 };

uint32_t Switch_Regs[32] = { 0 };
#endif

//static uint32_t vid=0xffffffff,pid=0xffffffff;
//static uint32_t regvalue=0xffffffff;

/* Private function prototypes -----------------------------------------------*/
static uint8_t
DM_CSUM(uint16_t Data, uint16_t RegAddr, uint8_t OpCode);

static void
P0_Init(void);
static void
P1_Init(void);
static void
P2_Init(void);
static void
P3_Init(void);
static void
P4_Init(void);

#if DM8606C_SUPERVISE
static void
P0_Supervisory(void);
static void
P1_Supervisory(void);
static void
P2_Supervisory(void);
static void
P3_Supervisory(void);
static void
P4_Supervisory(void);
static void
P5_Supervisory(void);
static void
Chip_Supervisory(void);
static void
System_Supervisory(void);
static void
Switch_Superbisory(void);
#endif

/* Private functions ---------------------------------------------------------*/
static uint8_t DM_CSUM(uint16_t Data, uint16_t RegAddr, uint8_t OpCode) {
  uint8_t result = 0, data_high8, data_low8;

  data_high8 = Data >> 8;
  data_low8 = Data & 0xff;

  result = data_low8 ^ data_high8 ^ (RegAddr & 0xff);

  uint8_t tmp1 = result ^ (RegAddr >> 8);
  uint8_t tmp2 = result ^ (OpCode << 2);

  result = (result & 0xf0) | (tmp1 & 3) | (tmp2 * 0x0c);
  return result;
}

static void P0_Init(void) {
  uint32_t reg = 0;
  heth.Init.PhyAddress = P0_PHY_BASE;

  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
  reg |= 0x1U << 11;
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);
  //DM_CSUM(reg,P0_PHY_BASE<<5|PHY_BMCR,1);
}

static void P1_Init(void) {
  uint32_t reg = 0;
  heth.Init.PhyAddress = P1_PHY_BASE;

  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R, &reg);
  reg |= 0x01 << 14;
  reg &= ~(0x1U << 10);
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R, &reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U << 4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);

//  uint32_t bmcr = 0;
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &bmcr);
//  bmcr |= (0x1U << 9);
//  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, bmcr);
}

static void P2_Init(void) {
  uint32_t reg = 0;
  heth.Init.PhyAddress = P2_PHY_BASE;

  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R, &reg);
  reg |= 0x01 << 14;
  reg &= ~(0x1U << 10);
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R, &reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U << 4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);
}

static void P3_Init(void) {
  uint32_t reg = 0;
  heth.Init.PhyAddress = P3_PHY_BASE;

  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);
//  uint32_t sc1r = 0;
//  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
//  reg |= 0x01<< 14;
//  reg &= ~(0x1U << 10);
//  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R, &reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U << 4;
  reg |= 1 << 9;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);

  HAL_ETH_ReadPHYRegister(&heth, PHY_PSCR, &reg);
//  reg &= ~(0x1U << 5);
  reg |= 0x1U << 9;
  HAL_ETH_WritePHYRegister(&heth, PHY_PSCR, reg);

}

static void P4_Init(void) {
  uint32_t reg = 0;
  heth.Init.PhyAddress = P4_PHY_BASE;

  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
  reg |= 0x1U << 11;
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);
}

#if DM8606C_SUPERVISE
static void P0_Supervisory(void) {
  heth.Init.PhyAddress = P0_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P0_Regs + i)) {
      P0_Regs[i] = 0;
    }
  }
}

static void P1_Supervisory(void) {
  heth.Init.PhyAddress = P1_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P1_Regs + i)) {
      P1_Regs[i] = 0;
    }
  }
}

static void P2_Supervisory(void) {
  heth.Init.PhyAddress = P2_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P2_Regs + i)) {
      P2_Regs[i] = 0;
    }
  }
}

static void P3_Supervisory(void) {
  heth.Init.PhyAddress = P3_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P3_Regs + i)) {
      P3_Regs[i] = 0;
    }
  }
}

static void P4_Supervisory(void) {
  heth.Init.PhyAddress = P4_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P4_Regs + i)) {
      P4_Regs[i] = 0;
    }
  }
}

static void P5_Supervisory(void) {
  heth.Init.PhyAddress = P5_EXTERNAL_PHY_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P5_Regs + i)) {
      P5_Regs[i] = 0;
    }
  }
}

static void Chip_Supervisory(void) {
  heth.Init.PhyAddress = CHIP_CSR_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, Chip_Regs + i)) {
      Chip_Regs[i] = 0;
    }
  }
}

static void System_Supervisory(void) {
  heth.Init.PhyAddress = SYSTEM_BASE;
  for (uint16_t i = 0x18; i < 0x1e; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, System_Regs + i)) {
      System_Regs[i] = 0;
    }
  }
//	HAL_ETH_ReadPHYRegister(&heth, i, System_Regs + i);
}

static void Switch_Superbisory(void) {
  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
  for (uint16_t i = 0; i < 32; i++) {
    if (HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, Switch_Regs + i)) {
      Switch_Regs[i] = 0;
    }
  }
}
#endif

/* Exported functions --------------------------------------------------------*/
void DM8606C_Init() {
  uint32_t tmp = 0;

  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
//Power down all analog PHY	
  HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 1 << 2);

  //NO initial of register in software rest
//	HAL_ETH_ReadPHYRegister(&heth, SWITCH_CR, &tmp);
//	tmp |= 0x1U <<4;
////	tmp |= 1<<3;
//	HAL_ETH_WritePHYRegister(&heth, SWITCH_CR, tmp);

  P0_Init();
  P1_Init();
  P2_Init();
  P3_Init();
  P4_Init();

  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
//	HAL_ETH_ReadPHYRegister(&heth, SWITCH_MCR, &tmp);
//	HAL_ETH_WritePHYRegister(&heth, SWITCH_MCR, 5);
  HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 0);

//	heth.Init.PhyAddress = SYSTEM_BASE;
//	HAL_ETH_WritePHYRegister(&heth, SYS_SBCR, 1);
}

void DM8606C_Watchdog(void) {
  HAL_StatusTypeDef reval;
  uint32_t reg = 0;
  heth.Init.PhyAddress = P1_PHY_BASE;

  do {
    reval = HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R, &reg);
  } while (reval != HAL_OK);

  if (0x4014 != reg) {
    HAL_GPIO_WritePin(DM8606C_RESET_GPIO_Port, DM8606C_RESET_Pin,
                      GPIO_PIN_RESET);
    osDelay(2);
    HAL_GPIO_WritePin(DM8606C_RESET_GPIO_Port, DM8606C_RESET_Pin, GPIO_PIN_SET);
    osDelay(1);
    DM8606C_Init();
  }
}

#if DM8606C_SUPERVISE
void DM8606C_Supervisory(void) {
  P0_Supervisory();
  P1_Supervisory();
  P2_Supervisory();
  P3_Supervisory();
  P4_Supervisory();
  P5_Supervisory();
  Chip_Supervisory();
  System_Supervisory();
  Switch_Superbisory();
  if (P1_Regs[0] != 0x2100) {

  }
}
#endif
