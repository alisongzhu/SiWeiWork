// Header:
// File Name: DM8606C.C
// Author:LH
// Date: Jul 26 2017


#include <stdint.h>

#include "dm8606c_priv.h"
#include "dm8606c.h"
#include "stm32f4xx_hal.h"

/* Private function prototypes -----------------------------------------------*/
static uint8_t DM_CSUM(uint16_t Data, uint16_t RegAddr, uint8_t OpCode);

static void P0_Init(void);
static void P1_Init(void);
static void P2_Init(void);
static void P3_Init(void);
static void P4_Init(void);
static void P5_Init(void);


#if DM8606C_SUPERVISE
uint32_t P0_PHY_Regs[32];
uint32_t P1_PHY_Regs[32];
uint32_t P2_PHY_Regs[32];
uint32_t P3_PHY_Regs[32];
uint32_t P4_PHY_Regs[32];
uint32_t P5_PHY_Regs[32];

uint32_t P0_Switch_Regs[32];
uint32_t P1_Switch_Regs[32];
uint32_t P2_Switch_Regs[32];
uint32_t P3_Switch_Regs[32];
uint32_t P4_Switch_Regs[32];
uint32_t P5_Switch_Regs[32];

uint32_t Chip_Regs[32];

uint32_t System_Regs[32];

uint32_t Switch_Regs[32];

static void P0_PHY_Supervisory(void);
static void P1_PHY_Supervisory(void);
static void P2_PHY_Supervisory(void);
static void P3_PHY_Supervisory(void);
static void P4_PHY_Supervisory(void);
static void P5_PHY_Supervisory(void);

static void P0_Switch_Supervisory(void);
static void P1_Switch_Supervisory(void);
static void P2_Switch_Supervisory(void);
static void P3_Switch_Supervisory(void);
static void P4_Switch_Supervisory(void);
static void P5_Switch_Supervisory(void);

static void Chip_Supervisory(void);
static void System_Supervisory(void);
static void Switch_Superbisory(void);
#endif

extern ETH_HandleTypeDef heth;

/* Private functions ---------------------------------------------------------*/
static uint8_t DM_CSUM(uint16_t Data, uint16_t RegAddr, uint8_t OpCode)
{
  uint8_t result = 0,data_high8,data_low8;

  data_high8 = Data>>8;
  data_low8 = Data & 0xff;

  result = data_low8^data_high8^(RegAddr&0xff);

  uint8_t tmp1 = result^(RegAddr>>8);
  uint8_t tmp2 = result^(OpCode<<2);

  result = (result&0xf0) |(tmp1&3)|(tmp2*0x0c);
  return result;
}

static void P0_Init(void)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = P0_PHY_BASE;

  //set port duplex mode = full,speed selection = 100M,auto-negotisation = disable.
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
  reg |= 0x01<< 14; //bypass scrambler/descrambler in fx mode.
  reg &= ~(0x1U << 10);//100base fx mode.
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);

  //manual force MDIX in FX mode.
  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R,&reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U <<4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);

  // Unplug clear address table.
  heth.Init.PhyAddress = P0_PORT_BASE;
  HAL_ETH_WritePHYRegister(&heth, PORT_BC0R, 1<<14);

  //broadcast storm threshold limited to 30%
  HAL_ETH_WritePHYRegister(&heth, PORT_BWCR, 0x0020);
}

static void P1_Init(void)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = P1_PHY_BASE;
  
  //set port duplex mode = full,speed selection = 100M,auto-negotisation = disable.
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
  reg |= 0x01<< 14; //bypass scrambler/descrambler in fx mode.
  reg &= ~(0x1U << 10);//100base fx mode.
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);
  
  //manual force MDIX in FX mode.
  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R,&reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U <<4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);
  
  // Unplug clear address table.
  heth.Init.PhyAddress = P1_PORT_BASE;
  HAL_ETH_WritePHYRegister(&heth, PORT_BC0R, 1<<14);

  //broadcast storm threshold limited to 30%
  HAL_ETH_WritePHYRegister(&heth, PORT_BWCR, 0x0020);
}

static void P2_Init(void)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = P2_PHY_BASE;

//  // power down this PHY.
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
//  reg |= 0x1U << 11 ;
//  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);

  // Unplug clear address table.
  heth.Init.PhyAddress = P2_PORT_BASE;
  HAL_ETH_WritePHYRegister(&heth, PORT_BC0R, 1<<14);

  //broadcast storm threshold limited to 16K packets/Sec
  HAL_ETH_WritePHYRegister(&heth, PORT_BWCR, 0x0020);
}

static void P3_Init(void)
{
  uint32_t reg = 0;
//  heth.Init.PhyAddress = P3_PHY_BASE;
//
//  // power down this PHY.
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
//  reg |= 0x1U << 11 ;
//  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);

  // Unplug clear address table.
  heth.Init.PhyAddress = P3_PORT_BASE;
  HAL_ETH_WritePHYRegister(&heth, PORT_BC0R, 1<<14);

  //broadcast storm threshold limited to 16K packets/Sec
  HAL_ETH_WritePHYRegister(&heth, PORT_BWCR, 0x0020);
}

static void P4_Init(void)
{
  uint32_t reg = 0;
//  heth.Init.PhyAddress = P4_PHY_BASE;
//
//  // power down this PHY.
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
//  reg |= 0x1U << 11 ;
//  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);

  // Unplug clear address table.
  heth.Init.PhyAddress = P4_PORT_BASE;
  HAL_ETH_WritePHYRegister(&heth, PORT_BC0R, 1<<14);

  //broadcast storm threshold limited to 30%
  HAL_ETH_WritePHYRegister(&heth, PORT_BWCR, 0x0020);
}

static void P5_Init(void)
{

}

#if DM8606C_SUPERVISE
static void P0_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P0_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P0_PHY_Regs + i))
    {
      P0_PHY_Regs[i] = 0xffffffff;
    }
  }
}

static void P1_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P1_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P1_PHY_Regs + i))
    {
      P1_PHY_Regs[i] =0xffffffff;
    }
  }
}

static void P2_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P2_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P2_PHY_Regs + i))
    {
      P2_PHY_Regs[i] = 0xffffffff;
    }
  }
}

static void P3_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P3_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P3_PHY_Regs + i))
    {
      P3_PHY_Regs[i] = 0xffffffff;
    }
  }
}

static void P4_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P4_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P4_PHY_Regs + i))
    {
      P4_PHY_Regs[i] = 0xffffffff;
    }
  }
}

static void P5_PHY_Supervisory(void)
{
  heth.Init.PhyAddress = P5_EXTERNAL_PHY_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P5_PHY_Regs + i))
    {
      P5_PHY_Regs[i] = 0xffffffff;
    }
  }
}

static void P0_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P0_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P0_Switch_Regs + i))
    {
      P0_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void P1_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P1_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P1_Switch_Regs + i))
    {
      P1_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void P2_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P2_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P2_Switch_Regs + i))
    {
      P2_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void P3_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P3_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P3_Switch_Regs + i))
    {
      P3_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void P4_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P4_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P4_Switch_Regs + i))
    {
      P4_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void P5_Switch_Supervisory(void)
{
  heth.Init.PhyAddress = P5_PORT_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, P5_Switch_Regs + i))
    {
      P5_Switch_Regs[i] = 0xffffffff;
    }
  }
}

static void Chip_Supervisory(void)
{
  heth.Init.PhyAddress = CHIP_CSR_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, Chip_Regs + i))
    {
      Chip_Regs[i] = 0;
    }
  }
}

static void System_Supervisory(void)
{
  heth.Init.PhyAddress = SYSTEM_BASE;
  for(uint16_t i=0x18;i<0x1e;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, System_Regs + i))
    {
      System_Regs[i] = 0;
    }
  }        
//  HAL_ETH_ReadPHYRegister(&heth, i, System_Regs + i);
}

static void Switch_Superbisory(void)
{
  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
  for(uint16_t i=0;i<32;i++)
  {
    if(HAL_OK != HAL_ETH_ReadPHYRegister(&heth, i, Switch_Regs + i))
    {
      Switch_Regs[i] = 0;
    }
  }
}
#endif

/* Exported functions --------------------------------------------------------*/
void DM8606C_Init()
{
  uint32_t val =0;

  //NO initial of register in software rest.
//  HAL_ETH_ReadPHYRegister(&heth, SWITCH_CR, &val);
  val = 0x1U <<4;
  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
  HAL_ETH_WritePHYRegister(&heth, SWITCH_CR, val);

  HAL_ETH_WritePHYRegister(&heth, 0X15, 3);/*AGE_TIME = 64SEC +- 32SEC*/

  P0_Init();
  P1_Init();
  P2_Init();
  P3_Init();
  P4_Init();
//  P5_Init();

#if LWIP_RSTP
    // Enable special tagging for RX and TX.
    // See the CPU port& Mirror Control Register, page 40 of the DM8606C datasheet.
    heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
    HAL_ETH_WritePHYRegister(&heth, SWITCH_MCR, 0x00ED);
    //Enable STP/RSTP
    // See the STP Index Enable Register, page 52 of the DM8606C datasheet.
    heth.Init.PhyAddress = 0x14;
    HAL_ETH_WritePHYRegister(&heth, 0x12, 1);
#endif


  //SMI bus error check enable.
//  heth.Init.PhyAddress = SYSTEM_BASE;
//  HAL_ETH_WritePHYRegister(&heth, SYS_SBCR, 1);
}

void DM8606C_Set_PHY_Down(void)
{
  //power on all analog PHY
  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
  HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 4u);
}

void DM8606C_Set_PHY_Up(void)
{
  //power on all analog PHY
  heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
  HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 2u);
}

#if DM8606C_SUPERVISE
void DM8606C_Supervisory(void)
{
//  P0_PHY_Supervisory();
//  P1_PHY_Supervisory();
//  P2_PHY_Supervisory();
//  P3_PHY_Supervisory();
//  P4_PHY_Supervisory();
//  P5_PHY_Supervisory();

//  P0_Switch_Supervisory();
  P1_Switch_Supervisory();
  P2_Switch_Supervisory();
  P3_Switch_Supervisory();
//  P4_Switch_Supervisory();
//  P5_Switch_Supervisory();

//  Chip_Supervisory();
//  System_Supervisory();
//  Switch_Superbisory();
}
#endif

