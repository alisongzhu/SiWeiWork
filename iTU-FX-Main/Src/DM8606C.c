// Header:
// File Name: DM8606C.C
// Author:LH
// Date: Jul 26 2017


#include <stdint.h>
#include "DM8606C.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

extern ETH_HandleTypeDef heth;

//static uint32_t vid=0xffffffff,pid=0xffffffff;
//static uint32_t regvalue=0xffffffff;

/* Private function prototypes -----------------------------------------------*/
static uint8_t DM_CSUM(uint16_t Data, uint16_t RegAddr, uint8_t OpCode);

static void P0_Init(void);
static void P1_Init(void);
static void P2_Init(void);
static void P3_Init(void);
static void P4_Init(void);
static void DM8606C_Switch_SetUp(void);
	
#if DM8606C_SUPERVISE
uint32_t P0_PHY_Regs[32] = {0};
uint32_t P1_PHY_Regs[32] = {0};
uint32_t P2_PHY_Regs[32] = {0};
uint32_t P3_PHY_Regs[32] = {0};
uint32_t P4_PHY_Regs[32] = {0};
uint32_t P5_PHY_Regs[32] = {0};

uint32_t P0_Switch_Regs[32] = {0};
uint32_t P1_Switch_Regs[32] = {0};
uint32_t P2_Switch_Regs[32] = {0};
uint32_t P3_Switch_Regs[32] = {0};
uint32_t P4_Switch_Regs[32] = {0};
uint32_t P5_Switch_Regs[32] = {0};

uint32_t Chip_Regs[32] = {0};

uint32_t System_Regs[32] = {0};

uint32_t Switch_Regs[32] = {0};

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

  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
  reg |= 0x1U << 11 ;
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);
	//DM_CSUM(reg,P0_PHY_BASE<<5|PHY_BMCR,1);
}

static void P1_Init(void)
{
	uint32_t reg = 0;
  heth.Init.PhyAddress = P1_PHY_BASE;
  
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
  reg |= 0x01<< 14;
  reg &= ~(0x1U << 10);
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);
  
  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R,&reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U <<4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg);
  
//  uint32_t bmcr = 0;
//  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &bmcr);
//  bmcr |= (0x1U << 9);
//  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, bmcr);
}

static void P2_Init(void)
{
  uint32_t reg = 0;
  heth.Init.PhyAddress = P2_PHY_BASE;
  
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);

  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
  reg |= 0x01<< 14;
  reg &= ~(0x1U << 10);
  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);
  
  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R,&reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U <<4;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg); 
}

static void P3_Init(void)
{
	uint32_t reg = 0;  
  heth.Init.PhyAddress = P3_PHY_BASE;
  
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, 0x2100U);
//  uint32_t sc1r = 0;
//  HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
//  reg |= 0x01<< 14;
//  reg &= ~(0x1U << 10);
//  HAL_ETH_WritePHYRegister(&heth, PHY_SC1R, reg);
  
  HAL_ETH_ReadPHYRegister(&heth, PHY_SC2R,&reg);
  reg &= ~(0x1U << 5);
  reg |= 0x1U <<4;
	reg |= 1<<9;
  HAL_ETH_WritePHYRegister(&heth, PHY_SC2R, reg); 
	
  HAL_ETH_ReadPHYRegister(&heth, PHY_PSCR,&reg);
//  reg &= ~(0x1U << 5);
  reg |= 0x1U <<9;
  HAL_ETH_WritePHYRegister(&heth, PHY_PSCR, reg); 
	
}

static void P4_Init(void)
{
	uint32_t reg = 0;
  heth.Init.PhyAddress = P4_PHY_BASE;

  HAL_ETH_ReadPHYRegister(&heth, PHY_BMCR, &reg);
  reg |= 0x1U << 11 ;
  HAL_ETH_WritePHYRegister(&heth, PHY_BMCR, reg);
}

static void DM8606C_Switch_SetUp(void)
{
	uint32_t reg = 0;
	// Set special tag Ether-Type 
	// See the special tag Ether-Type Register, page 41 of the DM8606C datasheet.
	heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
	HAL_ETH_WritePHYRegister(&heth, 0x14, 0x8100);
	
	// Enable special tagging for RX and TX.
	// See the CPU port& Mirror Control Register, page 40 of the DM8606C datasheet.
    heth.Init.PhyAddress = SWITCH_ENGINE_BASE;   
    HAL_ETH_ReadPHYRegister(&heth, SWITCH_MCR, &reg);
    reg |= (1<<7)|(1<<6) ; //enable special tag
    reg = (reg& ~7u)| 5u;  //Select CPU port number as port 5;
    HAL_ETH_WritePHYRegister(&heth, SWITCH_MCR, reg);
	
	//Enable STP/RSTP
	// See the STP Index Enable Register, page 52 of the DM8606C datasheet.
	heth.Init.PhyAddress = 0x14;
	HAL_ETH_WritePHYRegister(&heth, 0x12, 1);
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
//	HAL_ETH_ReadPHYRegister(&heth, i, System_Regs + i);
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
	uint32_t tmp =0;
	
	heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
//Power down all analog PHY	
	HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 1<<2);
	
	//NO initial of register in software rest
//	HAL_ETH_ReadPHYRegister(&heth, SWITCH_CR, &tmp);
//	tmp |= 0x1U <<4;
////	tmp |= 1<<3;
//	HAL_ETH_WritePHYRegister(&heth, SWITCH_CR, tmp);
	
//  P0_Init();
  P1_Init();
  P2_Init();
  P3_Init();
//  P4_Init();
	
	heth.Init.PhyAddress = SWITCH_ENGINE_BASE;
//	HAL_ETH_ReadPHYRegister(&heth, SWITCH_MCR, &tmp);
//	HAL_ETH_WritePHYRegister(&heth, SWITCH_MCR, 5);//set cpu port.
	HAL_ETH_WritePHYRegister(&heth, SWITCH_RR, 0);

	
//	heth.Init.PhyAddress = SYSTEM_BASE;
//	HAL_ETH_WritePHYRegister(&heth, SYS_SBCR, 1);

	DM8606C_Switch_SetUp();
}

void DM8606C_Watchdog(void)
{
	HAL_StatusTypeDef reval;
	uint32_t reg =0;
	heth.Init.PhyAddress = P1_PHY_BASE;
	
	do{
		reval = HAL_ETH_ReadPHYRegister(&heth, PHY_SC1R,&reg);
	}
	while(reval != HAL_OK);
	
	if(0x4014 != reg)
	{
    HAL_GPIO_WritePin(DM8606C_RESET_GPIO_Port, DM8606C_RESET_Pin, GPIO_PIN_RESET);
		osDelay(2);
		HAL_GPIO_WritePin(DM8606C_RESET_GPIO_Port, DM8606C_RESET_Pin, GPIO_PIN_SET);
		osDelay(1);
		DM8606C_Init();
	}
}

#if DM8606C_SUPERVISE
void DM8606C_Supervisory(void)
{
//	P0_PHY_Supervisory();
//  P1_PHY_Supervisory();
//  P2_PHY_Supervisory();
  P3_PHY_Supervisory();
//  P4_PHY_Supervisory();
//  P5_PHY_Supervisory();

//	P0_Switch_Supervisory();
  P1_Switch_Supervisory();
  P2_Switch_Supervisory();
  P3_Switch_Supervisory();
//  P4_Switch_Supervisory();
//  P5_Switch_Supervisory();
	
//  Chip_Supervisory();
//  System_Supervisory();
//	Switch_Superbisory();
}
#endif
