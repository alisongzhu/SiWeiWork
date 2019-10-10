/*
 * rn8302b_priv.h
 *
 *  Created on: 2019年8月4日
 *      Author: LH
 */

#ifndef COMPONENTS_RN8302B_RN8302B_PRIV_H_
#define COMPONENTS_RN8302B_RN8302B_PRIV_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Macros --------------------------------------------------------------------*/
#define UAWAV       0x0000      ///A 相电压采样数据 
#define UBWAV       0x0001      ///B 相电压采样数据          
#define UCWAV       0x0002      ///C 相电压采样数据 

#define IAWAV       0x0003      ///A 相电流采样数据             
#define IBWAV       0x0004      ///B 相电流采样数据
#define ICWAV       0x0005      ///C 相电流采样数据
#define INWAV       0x0006      ///零线电流采样数据

#define UA          0X0007      ///A 相电压有效值
#define UB          0X0008      ///B 相电压有效值
#define UC          0X0009      ///C 相电压有效值
#define USUM        0X000A      ///电压矢量和有效值

#define IA          0X000B      ///A 相电流有效值
#define IB          0X000C      ///B 相电流有效值
#define IC          0X000D      ///C 相电流有效值
#define IN          0X000E      ///零线电流有效值 
#define ISUM        0X0010      ///电流矢量和有效值 

#define IA_NVM1     0X0011      ///NVM1 A 相电流有效值 
#define IB_NVM1     0X0012      ///NVM1 B 相电流有效值 
#define IC_NVM1     0X0013      ///NVM1 C 相电流有效值 

#define PA          0X0014      
#define PB          0X0015
#define PC          0X0016
#define PT          0X0017

#define QA          0X0018
#define QB          0X0019
#define QC          0X001A
#define QT          0X001B

#define SA          0X001C
#define SB          0X001D
#define SC          0X001E
#define STA         0X001F

#define PfA         0x0020
#define PfB         0x0021
#define PfC         0x0022
#define PfTA        0x0023

#define PAFCnt      0x0024
#define PBFCnt      0x0025
#define PCFCnt      0x0026
#define PTFCnt      0x0027

#define QAFCnt      0x0028
#define QBFCnt      0x0029
#define QCFCnt      0x002A
#define QTFCnt      0x002B

#define SAFCnt      0x002C
#define SBFCnt      0x002D
#define SCFCnt      0x002E
#define STFACnt     0x002F

#define	EP_A		0x0030
#define	EP_B		0x0031
#define	EP_C		0x0032
#define	EP_T		0x0033
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
#define UFreq       0x0057

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
#define	IStart_Q	0x0103
#define LostVoltT   0x0104
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
#define UA_OS       0x0121
#define UB_OS       0x0122
#define UC_OS       0x0123
#define IA_OS       0x0124
#define IB_OS       0x0125
#define IC_OS       0x0126
#define IN_OS       0x0127

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
#define QA_PHS      0x0134
#define QB_PHS      0x0135
#define QC_PHS      0x0136
#define	PA_OS		0x0137
#define	PB_OS		0x0138
#define	PC_OS		0x0139

#define GFPA		0X0143
#define GFPB		0X0144
#define	GFPC		0X0145

#define	GFSA		0x0149
#define	GFSB		0x014A
#define	GFSC		0x014B

#define	CFCFG		0x0160
#define EMUCFG      0x0161
#define EMUCON      0x0162
#define WSAVECON    0x0163
#define	PQSign		0x0166
#define	CheckSum1	0x016A
#define WREN        0x0180
#define WMSW        0x0181
#define SOFTRST     0x0182
#define	DeviceID	0x018f
/* Types ---------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_RN8302B_RN8302B_PRIV_H_ */
