#ifndef COMMUNICTIOANFRAME_H
#define COMMUNICTIOANFRAME_H

//typedef unsigned char  		uint8_t;
//typedef signed char  		int8_t;
//typedef unsigned short  	uint16_t;
//typedef signed short  		int16_t;
//typedef unsigned long int  	uint32_t;
//typedef signed long int  	int32_t;
#define  Comm_TimerOUT      5000
#define  Check_TimerOUT     8000
typedef struct
{

    float  VoltageThreshould;   // 比较阀值
    float  CurrentThreshould;   // 比较阀值
    float  PowerThreshould;    // 比较阀值
    uint32_t FrequencyThreshould;
    uint32_t IRmsConst;			//10
    uint32_t VRmsConst;			//30
    uint32_t PRmsConst;
//uint16_t	NOLOAD;           // 启动电流阈值寄存器
//uint16_t	ZEROSTAR;					////过零阈值寄存
//uint32_t ChkSum1;
    uint16_t HFConst1;
//uint16_t HFConst2;
    uint16_t	VGain[3];				// 42
    uint16_t	IGain[4];				//48
    uint16_t  Ovlvl;        //过压
    uint16_t  Oilvl;        //过流
    uint16_t	P_PHS[3];/*有功相位校正寄存器*/				// 92
//uint16_t	PGain[3];/*有功增益校正寄存器*/

} sRN8302FirmParaFile_TypeDef;
typedef struct{
     uint8_t Config_flag;
     uint8_t MD_Adrr;
     uint8_t Bsp_EquipmentType;
     uint8_t Firmware_version;
     sRN8302FirmParaFile_TypeDef RN8302FirmPara[2];
}Config_ParameterTypeDef;

struct sSystemProtcolFrame{
    uint32_t destinationAddress[3];
    uint32_t SourceAddress[3];
    uint32_t Command;
    Config_ParameterTypeDef Config_Parameter;
};
#endif // COMMUNICTIOANFRAME_H
