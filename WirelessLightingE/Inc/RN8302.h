/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RN8302_H
#define __RN8302_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "main.h"


#define	EPA			0x0030
#define	EPB			0x0031
#define	EPC			0x0032
#define	EPT			0x0033
#define	PosEPA		0x0034
#define	PosEPB		0x0035
#define	PosEPC		0x0036
#define	PosEPT		0x0037
#define	NegEPA		0x0038
#define	NegEPB		0x0039
#define	NegEPC		0x003A
#define	NegEPT		0x003B

#define	EQA			0x003C
#define	EQB			0x003D
#define	EQC			0x003E
#define	EQT			0x003F
#define	PosEQA		0x0040
#define	PosEQB		0x0041
#define	PosEQC		0x0042
#define	PosEQT		0x0043
#define	NegEQA		0x0044
#define	NegEQB		0x0045
#define	NegEQC		0x0046
#define	NegEQT		0x0047

#define	ESA			0x0048
#define	ESB			0x0049
#define	ESC			0x004A
#define	EST			0x004B

#define YUA			0x0050
#define YUB			0x0051
#define YUC			0x0052
#define YIA			0x0053
#define YIB			0x0054
#define YIC			0x0055
#define YIN			0x0056

#define	FEPA		0x007A
#define	FEPB		0x007B
#define	FEPC		0x007C
#define	FEPT		0x007D
#define	PosFEPA		0x007E
#define	PosFEPB		0x007F
#define	PosFEPC		0x0080
#define	PosFEPT		0x0081
#define	NegFEPA		0x0082
#define	NegFEPB		0x0083
#define	NegFEPC		0x0084
#define	NegFEPT		0x0085

#define	FEQA		0x0086
#define	FEQB		0x0087
#define	FEQC		0x0088
#define	FEQT		0x0089
#define	PosFEQA		0x008A
#define	PosFEQB		0x008B
#define	PosFEQC		0x008C
#define	PosFEQT		0x008D
#define	NegFEQA		0x008E
#define	NegFEQB		0x008F
#define	NegFEQC		0x0090
#define	NegFEQT		0x0091

#define	FESA		0x0092
#define	FESB		0x0093
#define	FESC		0x0094
#define	FEST		0x0095

#define	HFCONST1	0x0100
#define	HFCONST2	0x0101
#define	IStart_PS	0x0102
#define	LostVoltT  0x0104
#define	ZXOT		0x0105
#define	PRTH1L		0x0106
#define	PRTH1H		0x0107
#define	PRTH2L		0x0108
#define	PRTH2H		0x0109
#define	PHSUA		0x010C
#define	PHSUB		0x010D
#define	PHSUC		0x010E
#define	PHSIA		0x010F
#define	PHSIB		0x0110
#define	PHSIC		0x0111
#define	GSUA		0x0113
#define	GSUB		0x0114
#define	GSUC		0x0115
#define	GSIA		0x0116
#define	GSIB		0x0117
#define	GSIC		0x0118
#define	GSIN		0x0119
#define	DCOS_UA		0x011A
#define	DCOS_UB		0x011B
#define	DCOS_UC		0x011C
#define	GPA			0x0128
#define	GPB			0x0129
#define GPC			0x012A
#define	GQA			0x012B
#define	GQB			0x012C
#define	GQC			0x012D
#define	GSA			0x012E
#define	GSB			0x012F
#define	GSC			0x0130
#define	PA_PHS		0x0131
#define	PB_PHS		0x0132
#define	PC_PHS		0x0133
#define	PA_OS		0x0137
#define	PB_OS		0x0138
#define	PC_OS		0x0139

#define GFPA		0X0143
#define GFPB		0X0144
#define	GFPC		0X0145

#define	GFSA		0x0149
#define	GFSB		0x014A
#define	GFSC		0x014B

#define OVLVL  0x0159
#define OILVL   0x015A

#define	CFCFG		0x0160
#define EMUCON      0x0162
#define	PQSign		0x0166
#define	CheckSum1	0x016A
#define	WMSW		0x0181
#define	DeviceID	0x018f


typedef union                                     
{
	uint8_t  ucTempBuf[8];
	uint32_t lTemp32;
	uint16_t wTemp16;
	uint8_t  ucTemp8;
}sRN8302StruDataComm_TypeDef;


typedef struct
{

	float  VoltageThreshould;   // 姣旇緝闃�鍊�
	float  CurrentThreshould;   // 姣旇緝闃�鍊�
	float  PowerThreshould;    // 姣旇緝闃�鍊�
	float  VoltageDeadZone[3];
	float  CurrentDeadZone[3];
	float  PowerDeadZone[3];
	uint32_t FrequencyThreshould;
	uint32_t IRmsConst[3];			//10
	uint32_t VRmsConst[3];			//30
	uint32_t PRmsConst[3];  
//uint16_t	NOLOAD;           // 鍚姩鐢垫祦闃堝�煎瘎瀛樺櫒
//uint16_t	ZEROSTAR;					////杩囬浂闃堝�煎瘎瀛�
//uint32_t ChkSum1;
	uint16_t HFConst1;          
//uint16_t HFConst2;
	uint16_t	VGain[3];				// 42
	uint16_t	IGain[4];				//48
	uint16_t  Ovlvl;        //杩囧帇
	uint16_t  Oilvl;        //杩囨祦
	uint16_t	P_PHS[3];/*鏈夊姛鐩镐綅鏍℃瀵勫瓨鍣�*/				// 92
//uint16_t	PGain[3];/*鏈夊姛澧炵泭鏍℃瀵勫瓨鍣�*/
	uint16_t Reserve;
	
} sRN8302FirmParaFile_TypeDef;
;



	
	
	


#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/**
  * @}
  */

/**
  * @}
  */
