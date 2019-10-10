#ifndef __DM8606C_H
#define __DM8606C_H

//#include "stm32f4xx_hal_eth.h"
#define DM8606C_SUPERVISE         1

//Port0~Port4 PHY Register (000H~0FFH)
/* For easy to understanding, The absolute address can be devided to 5-bit PHY Address plus 5-bit Register Address */
#define P0_PHY_BASE     ((uint16_t)0x02U)    //UNUSED
#define P1_PHY_BASE     ((uint16_t)0x03U)    //FX1
#define P2_PHY_BASE     ((uint16_t)0x04U)    //FX2
#define P3_PHY_BASE     ((uint16_t)0x05U)    //TX
#define P4_PHY_BASE     ((uint16_t)0x06U)    //UNUSED

/* There is no phy layer of port 5 in DM8606C */
#define P5_EXTERNAL_PHY_BASE     ((uint16_t)0x07U) // Reverse MII

#define PHY_BMCR         ((uint16_t)0x00U)    // Basic Mode Control Register (BMCR)
#define PHY_BMSR         ((uint16_t)0x01U)    // Basic Mode Status Register (BMSR)
#define PHYID1           ((uint16_t)0x02U)    // PHY ID Identifier Register #1
#define PHYID2           ((uint16_t)0x03U)    // PHY ID Identifier Register #2
#define PHY_ANAR         ((uint16_t)0x04U)    // Auto-Nego. Advertised Register (ANAR)
#define PHY_ANPAR        ((uint16_t)0x05U)    // Auto-Nego. Partner Ability Register (ANPAR)
#define PHY_ANER         ((uint16_t)0x06U)    // Auto-Nego. Expansion Register (ANER)

#define PHY_SC1R         ((uint16_t)0x10U)    // Specified Control 1 Register (SC1R)
#define PHY_SC2R         ((uint16_t)0x14U)    // Specified Control 2 Register (SC2R)
#define PHY_PSCR         ((uint16_t)0x1DU)    // Power Saving Control Register (PSCR)

//Switch Per-Port Registers (100H~1FFH)
#define P0_PORT_BASE     ((uint16_t)0x08U)    //UNUSED
#define P1_PORT_BASE     ((uint16_t)0x09U)    //FX1
#define P2_PORT_BASE     ((uint16_t)0x0AU)    //FX2
#define P3_PORT_BASE     ((uint16_t)0x0BU)    //TX
#define P4_PORT_BASE     ((uint16_t)0x0CU)    //UNUSED
#define P5_PORT_BASE     ((uint16_t)0x0DU)    //RevMII

#define PORT_SDR           ((uint16_t)0x10U)    // Per Port Status Data Register (P0_PPSDR)
#define PORT_BC0R          ((uint16_t)0x11U)    // Per Port Basic Control 0 Register (PPBC0R)
#define PORT_BC1R          ((uint16_t)0x12U)    // Per Port Basic Control 1 Register (PPBC1R)
#define PORT_BLKC0R        ((uint16_t)0x13U)    // Per Port Block Control 0 Register (P0_PPBLKC0R)
#define PORT_BLKC1R        ((uint16_t)0x14U)    // Per Port Block Control 1 Register (P0_PPBLKC1R)
#define PORT_BWCR          ((uint16_t)0x15U)    // Per Port BandWidth Control  Register (P0_PPBWCR)
#define PORT_VLAN_TIR      ((uint16_t)0x16U)    // Per Port VLAN Tag Information Register 
#define PORT_VLAN_CR       ((uint16_t)0x17U)    // Per Port Priority and VLAN Control Register
#define PORT_SEC_CR        ((uint16_t)0x18U)    // Per Port Security Control Register
#define PORT_ACR           ((uint16_t)0x19U)    // Per Port Advanced Control Register
#define PORT_MCR           ((uint16_t)0x1AU)    // Per Port Memory Control Register
#define PORT_DLR           ((uint16_t)0x1BU)    // Per Port Discard Limitation Register

// Switch Engine Registers (200H~2FFH)
#define SWITCH_ENGINE_BASE        ((uint16_t)0x10U)    // Switch Base Register Address
#define SWITCH_SR          ((uint16_t)0x10U)    // Switch Status Register
#define SWITCH_RR          ((uint16_t)0x11U)    // Switch Reset Register
#define SWITCH_CR          ((uint16_t)0x12U)    // Switch Control Register Register
#define SWITCH_MCR         ((uint16_t)0x13U)    // CPU Port & Mirror Control Register

// Chip Control and Status Registers
#define CHIP_CSR_BASE      ((uint16_t)0x18U)    

#define CHIP_VID           ((uint16_t)0x10U)    // Vendor ID
#define CHIP_PID           ((uint16_t)0x11U)    // Vendor ID
#define CHIP_CHIPR         ((uint16_t)0x12U)    // Chip Revision
#define CHIP_P4_MAC_CR     ((uint16_t)0x14U)    // Port 4 MAC Control Register
#define CHIP_P5_MAC_CR     ((uint16_t)0x15U)    // Port 5 MAC Control Register
#define CHIP_LED_CR        ((uint16_t)0x17U)    // LED Control Register
#define CHIP_ISR           ((uint16_t)0x18U)    // Interrupt Status Register
#define CHIP_IMCR          ((uint16_t)0x19U)    // Interrupt Mask Control Register
#define CHIP_EEPROM_CAR    ((uint16_t)0x1AU)    // EEPROM Control & Address Register
#define CHIP_EEPROM_DR     ((uint16_t)0x1BU)    // EEPROM Data Register
#define CHIP_MR1           ((uint16_t)0x1CU)    // Monitor Register 1
#define CHIP_MR2           ((uint16_t)0x1DU)    // Monitor Register 2
#define CHIP_MR3           ((uint16_t)0x1EU)    // Monitor Register 3

// System Registers
#define SYSTEM_BASE        ((uint16_t)0x19U) 

#define SYS_SCSR           ((uint16_t)0x18U)    // System Clock Select Registers        
#define SYS_SBECR          ((uint16_t)0x19U)    // Serial Bus Error Check Register  
#define SYS_SBCR           ((uint16_t)0x1AU)    // Serial Bus Control Register  
#define SYS_VPHY_CR        ((uint16_t)0x1DU)    // Virtual PHY Control Register 
#define SYS_PHY_CTR        ((uint16_t)0x1EU)    // PHY Control Test Register 



/* Initialization and de-initialization functions  ****************************/

/** @addtogroup DM8606C_Exported_Functions_Group1
  * @{
  */
void DM8606C_Init(void);
void DM8606C_Watchdog(void);
#if DM8606C_SUPERVISE
void DM8606C_Supervisory(void);
#endif
/**
  * @}
  */


#endif /* __DM8606C_H */
