/*
 * env.c
 *
 *  Created on: Nov 29 2017
 *      Author: LH
 */
/* Includes ------------------------------------------------------------------*/
#include "env.h"
#include "flash_if.h"
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/
EnvTable env;
struct sSystemEnv SystemEnv;
uint32_t user_env_address,sys_env_address;
/* Private function prototypes -----------------------------------------------*/

static void env_init(void);//initialize env

/* Private functions ---------------------------------------------------------*/

static void env_init(void)
{
	//System parameters initialize.
	struct sBasicEnv *basic = &env.BasicEnv;
	basic->CanDevCount = 2;
	basic->iec104_enalbe = 1;
	basic->autosave = 0;
	basic->autosavetime = 0;// Unit: Minute
	basic->CoilOutputMode = COIL_MODE_SBO;
	basic->InternalDiscreteCNT = 16;
	basic->InternalCoilCNT = 6;
	basic->InternalInputCNT = 0;
	basic->InternalHoldingCNT = 0;
	basic->ExternalDiscreteCNT = 0;
	basic->ExternalCoilCNT = 0;
	basic->ExternalInputCNT = 0;
	basic->ExternalHoldingCNT = 0;
	basic->ScanPeriod = 900;// Unit: milliSecond

	//Network parameters initialize.
	struct sNetworkEnv *net = &env.NetworkParameters;
	net->ip[0] = 192;
	net->ip[1] = 168;
	net->ip[2] = 1;
	net->ip[3] = 3;
	net->gw[0] = 192;
  net->gw[1] = 168;
  net->gw[2] = 1;
  net->gw[3] = 1;
  net->netmask[0] = 255;
	net->netmask[1] = 255;
	net->netmask[2] = 255;
	net->netmask[3] = 0;

	//COM1 parameters initialize.
	struct sComEnv *com = &env.ComEnv[0];
	com->opMode = COM_OPMODE_NONE;
	com->address = 1;
	com->baudrate = 9600;
	com->eParity = 0;
	com->ucDataBits = 8;
	com->StopBits = 1;
	com->port = 4000;

	//COM2 parameters initialize.
	com = &env.ComEnv[1];
	com->opMode = COM_OPMODE_NONE;
	com->address = 1;
	com->baudrate = 9600;
	com->eParity = 0;
	com->ucDataBits = 8;
	com->StopBits = 1;
	com->port = 4001;

	//iec104 parameters initialize.
	struct sIEC104Env *iec = &env.IEC104Env;
	iec->SBO_Deadband = 15;
	iec->address = 1;
	iec->port = 2404;

	//Internal Discrete parameters initialize.
//	struct sDiscreteGlobal *dg = &env.DiscreteGlobal;
//	dg->UseDeadband = 1;
//	dg->Deadband = 50;
//	dg->UseReversFlag = 1;
//	dg->ReverseFlag = 0;

	struct sLogicalDiscreteGlobal *ldg = &env.LogicalDiscreteGlobal;
	ldg->ObjectMode = IEC104_OI_MODE_SP;
//	ldg->UseGenEvent = 1;
//	ldg->GenEvent = 1;
//	ldg->UseGenSOE = 1;
//	ldg->GenSOE = 1;
//	ldg->UseCombinationalMode = 1;
//	ldg->CombinationalMode = COMBINATIONAL_MODE_SP;

//	struct sLogicalCoilGlobal *lcg = &env.LogicalCoilGlobal;
//	lcg->UseIsLocked = 1;
//	lcg->IsLocked = 0;
//	lcg->UsePulseWidth =1 ;
//	lcg->PulseWidth = 300;
//	lcg->UseSelectFlag = 1;
//	lcg->Selected = 0;
//	lcg->UseCombinationalMode = 1;
//	lcg->CombinationalMode = COMBINATIONAL_MODE_DP;

		//set default DI channel individual parameters 
//		for(uint8_t i=0;i< env.BasicEnv.InternalDiscreteCNT;i++)
//    {
//        env.LogicalDiscreteIndividual[i].combination.sp = i;
//    }

		for(uint8_t i=0; i<env.BasicEnv.InternalCoilCNT; i++)
		{
				env.LogicalCoilIndividual[i].combination.dp.off = 2*i;
				env.LogicalCoilIndividual[i].combination.dp.on = 2*i+1;
		}

    for(uint8_t i=0;i< INTERNAL_DISCRETE_COUNT;i++)
    {
			env.DiscreteIndividual[i].Deadband = 50;
			env.DiscreteIndividual[i].ReverseFlag = 0;
//        if(1 == env.DiscreteGlobal.UseDeadband)
//            env.DiscreteIndividual[i].Deadband = env.DiscreteGlobal.Deadband;
//        if(1 == env.DiscreteGlobal.UseReversFlag)
//            env.DiscreteIndividual[i].ReverseFlag = env.DiscreteGlobal.ReverseFlag;
    }

    for(uint8_t i=0;i< env.BasicEnv.InternalDiscreteCNT;i++)
    {
//			env.LogicalDiscreteIndividual[i].mode = IEC104_OI_MODE_SP;
			env.LogicalDiscreteIndividual[i].GenEvent = 1;
			env.LogicalDiscreteIndividual[i].GenSOE = 1;
			env.LogicalDiscreteIndividual[i].CombinationalMode = COMBINATIONAL_MODE_SP;
			env.LogicalDiscreteIndividual[i].combination.sp = i;
//        if(1 == env.LogicalDiscreteGlobal.UseGenEvent)
//            env.LogicalDiscreteIndividual[i].GenEvent = env.LogicalDiscreteGlobal.GenEvent;

//        if(1 == env.LogicalDiscreteGlobal.UseGenSOE)
//            env.LogicalDiscreteIndividual[i].GenSOE = env.LogicalDiscreteGlobal.GenSOE;

//        if(1 == env.LogicalDiscreteGlobal.UseCombinationalMode)
//            env.LogicalDiscreteIndividual[i].CombinationalMode = env.LogicalDiscreteGlobal.CombinationalMode;
    }

    for(uint8_t i=0; i<env.BasicEnv.InternalCoilCNT; i++)
    {
			env.LogicalCoilIndividual[i].Selected = 0;
			env.LogicalCoilIndividual[i].IsLocked = 0;
			env.LogicalCoilIndividual[i].PulseWidth = 300;
			env.LogicalCoilIndividual[i].CombinationalMode = COMBINATIONAL_MODE_DP;
			
//        if(1 == env.LogicalCoilGlobal.UseSelectFlag)
//        {
//            env.LogicalCoilIndividual[i].Selected = env.LogicalCoilGlobal.Selected;
//        }
//        if(1 == env.LogicalCoilGlobal.UsePulseWidth)
//        {
//            env.LogicalCoilIndividual[i].PulseWidth = env.LogicalCoilGlobal.PulseWidth;
//        }
//        if(1 == env.LogicalCoilGlobal.UseIsLocked)
//        {
//            env.LogicalCoilIndividual[i].IsLocked = env.LogicalCoilGlobal.IsLocked;
//        }
//        if(1 == env.LogicalCoilGlobal.CombinationalMode)
//        {
//            env.LogicalCoilIndividual[i].CombinationalMode = env.LogicalCoilGlobal.CombinationalMode;
//        }
    }
}

/* Exported functions --------------------------------------------------------*/
void env_load(void)
{
		//set default env.
    env_init();
	
		//load system env from flash sector 1
    FLASH_If_Read(SYSTEM_ENV_ADDRESS,(uint32_t *)&SystemEnv,sizeof(struct sSystemEnv)/4);
	
		//get system env from flash sector 1
    if(0 == SystemEnv.config_flag)
    {
        FLASH_If_Read(USER_ENV_ADDRESS,(uint32_t *)&env,sizeof(env)/4);
    }
}

/**
 * Save env value to MCU inside flash.
 *
 * @param p pointer of env
 * @return 0 success,other failure
 */
uint8_t sys_env_save(void)
{
  uint8_t result=0;

  FLASH_If_Init();

  sys_env_address = SYSTEM_ENV_ADDRESS;
  if((FLASHIF_OK == FLASH_If_Erase(SYSTEM_ENV_ADDRESS,SYSTEM_ENV_SECTORS)))
  {
    if(FLASHIF_OK==FLASH_If_Write(&sys_env_address,(uint32_t*)&SystemEnv,sizeof(struct sSystemEnv)/4))
    {
      HAL_FLASH_Lock();
    }
    else{
      result = 1;
    }
  }
  else{
    result = 1;
  }
  return result;
}

/**
 * Save env value to MCU inside flash.
 *
 * @param p pointer of env
 * @return 0 success,other failure
 */
uint8_t user_env_save(void)
{
  uint8_t result=0;

  FLASH_If_Init();
	
	user_env_address = USER_ENV_ADDRESS;

  if(FLASHIF_OK == FLASH_If_Erase(USER_ENV_ADDRESS,USER_ENV_SECTORS))
  {
    if(FLASHIF_OK==FLASH_If_Write(&user_env_address,(uint32_t*)&env,sizeof(env)/4))
    {
      HAL_FLASH_Lock();
    }
    else{
      result = 1;
    }
  }
  else{
    result = 1;
  }
  return result;
}
