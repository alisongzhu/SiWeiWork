/*
 * env.h
 *
 *  Created on: Nov 2 2017
 *      Author: LH
 */
 
#ifndef _ENV_H_
#define _ENV_H_

#include <stdint.h>

/* define max I/O count -----------------------------------*/
#define COM_MAX_COUNT               2
#define INTERNAL_DISCRETE_COUNT		16
#define INTERNAL_COIL_COUNT			12
#define INTERNAL_INPUT_COUNT        0
#define INTERNAL_HOLDING_COUNT      0

#define EXTERNAL_DISCRETE_COUNT     0
#define EXTERNAL_COIL_COUNT         0
#define EXTERNAL_INPUT_COUNT        0
#define EXTERNAL_HOLDING_COUNT      0

/* Coil output mode ------------------------------------------------------*/
#define COIL_MODE_SBO           0
#define COIL_MODE_DIRECT        1

/* IEC104 object information mode -----------------------------------*/
#define IEC104_OI_MODE_SP         0
#define IEC104_OI_MODE_DP         1

/* Internal IEC104 I/O object combinational mode -------------------------*/
#define COMBINATIONAL_MODE_SP        	0           // single point
#define COMBINATIONAL_MODE_DP        	1           // double point
#define COMBINATIONAL_MODE_DD			2			// double point with double channel


#define COIL_LOCK_OFF           0
#define COIL_LOCK_ON            1
#define COIL_LOCK_SET           2

//reserve address for IEC104 broadcasting
#define COMMOM_ADDRESS_BROADCAST    0xffff

/* COM operate mode -------------------------*/
#define COM_OPMODE_NONE             0  
#define COM_OPMODE_DTU              1
#define COM_OPMODE_MASTER_RTU       2
#define COM_OPMODE_MASTER_ASC       3
#define COM_OPMODE_MASTER_TCP       4
#define COM_OPMODE_SLAVE_RTU        5
#define COM_OPMODE_SLAVE_ASC        6
#define COM_OPMODE_SLAVE_TCP        7

typedef struct sDiscreteInfo{
    uint8_t  val;
    uint8_t  flag;
    uint8_t  HasEvent;
    uint64_t timestamp;
} DiscreteInfo;

struct sCombinationalModeDP{
    uint16_t off;
    uint16_t on;
};

struct sCombinationalModeDD{
    uint8_t off1;
    uint8_t on1;
    uint8_t off2;
    uint8_t on2;
};
struct sSystemEnv{
    uint8_t config_flag;
    uint8_t need_reload;
    uint8_t load_started;
    uint8_t load_success;
    uint32_t firmware_size;
    uint32_t boot_times;
};

struct sBasicEnv{
    uint8_t CanDevCount;
    uint8_t iec104_enalbe;
    uint8_t autosave;
    uint8_t autosavetime;// Unit: Minute
    uint8_t CoilOutputMode;
    uint8_t InternalDiscreteCNT;
    uint8_t InternalCoilCNT;
    uint8_t InternalInputCNT;
    uint8_t InternalHoldingCNT;
    uint8_t ExternalDiscreteCNT;
    uint8_t ExternalCoilCNT;
    uint8_t ExternalInputCNT;
    uint8_t ExternalHoldingCNT;
    uint16_t ScanPeriod;// Unit: milliSecond
};

struct sNetworkEnv{
    uint8_t    ip[4];
    uint8_t    netmask[4];
    uint8_t    gw[4];
};

struct sComEnv{
    uint8_t     opMode;//see COM_OPMODE_XX mode define.
    uint8_t 	address;
    uint8_t     eParity;  // none = 0; odd = 1; even = 2;
    uint8_t     ucDataBits;//unused, default= 8
    uint16_t    port;
    uint16_t    StopBits;// unused, default = 1
    uint32_t    baudrate;
};
struct sIEC104Env{
    uint8_t     address;
    uint8_t     SBO_Deadband;// SBO timeout, Unit: Second
    uint16_t    port;
};

//struct sDiscreteGlobal{
//    uint8_t     UseReversFlag;
//    uint8_t     ReverseFlag;
//	uint8_t		UseDeadband;
//	uint16_t	Deadband; // Unit: milliSecond
//};

struct sDiscreteIndividual{
    uint8_t    ReverseFlag;
    uint16_t    Deadband;
};

struct sLogicalDiscreteGlobal{
////    uint8_t     UseObjectMode;//IEC104 information object Mode ,see IEC104_OI_MODE_XX @define
    uint8_t     ObjectMode;
//    uint8_t     UseGenEvent;
//    uint8_t     GenEvent;
//    uint8_t     UseGenSOE;
//    uint8_t     GenSOE;
//    uint8_t     UseCombinationalMode;
//    uint8_t     CombinationalMode;
};

struct sLogicalDiscreteIndividual{
//  uint8_t   mode;//IEC104 information object Mode ,see IEC104_OI_MODE_XX @define
	uint8_t		GenEvent;
	uint8_t		GenSOE;
	uint8_t		CombinationalMode;
	union uDiscreteCombination{
		uint32_t sp;
		struct sCombinationalModeDP dp;
	}combination;
};

//struct sLogicalCoilGlobal{
//    uint8_t     UseSelectFlag;
//    uint8_t     Selected;
//    uint8_t     UseIsLocked;
//    uint8_t     IsLocked;    // Control rejection flag
//    uint8_t     UseCombinationalMode;
//    uint8_t     CombinationalMode;
//    uint8_t     UsePulseWidth;
//    uint16_t    PulseWidth; // Unit: milliSecond
//};

struct sLogicalCoilIndividual{
		uint8_t     Selected;
    uint8_t     IsLocked;    // Control rejection flag
    uint16_t    PulseWidth; // Unit: milliSecond
    uint8_t     CombinationalMode;
	union uCoilCombination{
		uint32_t sp;
		struct sCombinationalModeDP dp;
		struct sCombinationalModeDD dd;
	} combination;
};


typedef struct sEnvTable{
    struct sBasicEnv       	BasicEnv;
    struct sNetworkEnv      	NetworkParameters;   
    struct sComEnv  	        ComEnv[COM_MAX_COUNT];
    struct sIEC104Env       	IEC104Env;
//    struct sDiscreteGlobal		DiscreteGlobal;
    struct sDiscreteIndividual    DiscreteIndividual[INTERNAL_DISCRETE_COUNT];
    struct sLogicalDiscreteGlobal       LogicalDiscreteGlobal;
    struct sLogicalDiscreteIndividual 	LogicalDiscreteIndividual[INTERNAL_DISCRETE_COUNT];
//    struct sLogicalCoilGlobal           LogicalCoilGlobal;
    struct sLogicalCoilIndividual		LogicalCoilIndividual[INTERNAL_COIL_COUNT];
}EnvTable;

extern EnvTable env;
extern struct sSystemEnv SystemEnv;
extern uint32_t user_env_address,sys_env_address;
void env_load(void);//load env value from MCU inside flash. 
uint8_t sys_env_save(void);//save env value to MCU inside flash. 
uint8_t user_env_save(void);//save env value to MCU inside flash. 
#endif
