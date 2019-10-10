/*
 * rn8302b.h
 *
 *  Created on: 2019年7月31日
 *      Author: alien
 */

#ifndef COMPONENTS_RN8302B_RN8302B_H_
#define COMPONENTS_RN8302B_RN8302B_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"

#include "nvs/include/nvs.h"

/* Macros --------------------------------------------------------------------*/
#define RN8302B_HAS_MULTIPLE_INSTANCE
//#define RN8302B_USE_SLIST_API

#define RN8302B_STORAGE_CONFIG_IN_ZEPHRY_NVS

#ifdef RN8302B_STORAGE_CONFIG_IN_ZEPHRY_NVS
#define RN8302B_ID_CONF               (0)
#define RN8302B_ID_GAIN               (1)
#define RN8302B_ID_PHASE              (2)
#define RN8302B_ID_OFFSET             (3)
#define RN8302B_ID_RATIO              (4)
#define RN8302B_ID_DEADBAND           (5)

#define RN8302B_ID_TOTAL_CNT          (6)
#else
#define RN8302B_KEY_CONF               "conf"
#define RN8302B_KEY_GAIN               "gain"
#define RN8302B_KEY_PHASE              "phase"
#define RN8302B_KEY_OFFSET             "offset"
#define RN8302B_KEY_RATIO              "ratio"
#define RN8302B_KEY_DEADBAND           "deadband"
#endif


#define RN8302B_BASE_DATA_CNT            30

#define RN8302B_POLL_WITHOUT_ENERGY
#ifndef RN8302B_POLL_WITHOUT_ENERGY
#define RN8302B_MAX_ENERGY_DATA_CNT     28
#define RN8302B_MAX_DATA_CNT            (RN8302B_BASE_DATA_CNT + RN8302B_MAX_ENERGY_DATA_CNT)
#define RN8302B_ENERGY_STORGAE_ESP32_NVS
#else
#define RN8302B_MAX_DATA_CNT            (RN8302B_BASE_DATA_CNT)
#endif

/* Types ---------------------------------------------------------------------*/
typedef struct rn8302b_ratio_
{
    uint16_t V[3]; ///外置PT变送器变比
    uint16_t I[4]; ///外置CT变送器变比
} rn8302b_ratio_t;

typedef struct rn8302b_deadband_
{
    uint16_t V[3]; ///电压测量值死区=最小测量值倍数（0.01V）
    uint16_t I[4]; ///电流测量值死区=最小测量值倍数（0.001A）
} rn8302b_deadband_t;


typedef struct rn8302b_gain_
{
    int16_t  V[3]; /// 芯片内部电压增益寄存器参数
    int16_t  I[4]; /// 芯片内部电流增益寄存器参数
} rn8302b_gain_t;

typedef struct rn8302b_phase_
{
    int16_t  P[3]; /// 芯片内部有功相位校准寄存器参数
    int16_t  Q[3]; /// 芯片内部无功相位校准寄存器参数
} rn8302b_phase_t;

typedef struct rn8302b_offset_
{
    int16_t I[4]; /// 电流通道offset校正参数
} rn8302b_offset_t;

#ifndef RN8302B_POLL_WITHOUT_ENERGY
struct energy_data
{
    uint32_t data[RN8302B_MAX_ENERGY_DATA_CNT];
};
#endif


#pragma pack(push, 1)
typedef struct rn8302b_dev_param_
{
    uint16_t e_const; //电表常数
    uint16_t rated_vol;//额定电流
    uint16_t rated_cur;//额定电压
    uint32_t std_vol;  // 额定5A电流有效值寄存器标准值
    uint32_t std_cur;   // 额定220V电压有效值寄存器标准值
} rn8302b_conf_param_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct rn8302b_calc_param_
{
    uint16_t rms_vol;// 0.01V 电压对应的有效值寄存器标准值
    uint16_t rms_cur;//0.001A 电流对应的有效值寄存器标准值
    float rms_power;
} rn8302b_calc_param_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct spi_device_handle_{
  SPI_TypeDef *handle;
#ifdef RN8302B_HAS_MULTIPLE_INSTANCE
  SemaphoreHandle_t lock;
#endif
  GPIO_TypeDef *port;//chip select pin port.
  uint32_t PinMask;
}spi_device_handle_t;
#pragma pack(pop)

typedef struct nvs_handle_{
  struct nvs_fs *fs;
  uint16_t id_offset;
}__packed nvs_handle_t;

struct rn8302b_dev  
{       
  nvs_handle_t nvs;
  spi_device_handle_t spi;
  /* parameters,must be initialized first coz used in modbus*/
  void *table;//内存区
  rn8302b_ratio_t *ratio;
  rn8302b_deadband_t * deadband;

  rn8302b_calc_param_t calc;
#ifndef RN8302B_POLL_WITHOUT_ENERGY
  struct energy_data *energy;
#endif
}__packed;

/* Constants -----------------------------------------------------------------*/

/* Global Variables ----------------------------------------------------------*/

/* Function Prototypes -------------------------------------------------------*/
/*
#ifdef RN8302B_USE_SLIST_API
int rn8302b_dev_register(struct rn8302b_dev *dev, uint32_t id);
#endif
*/

int rn8302b_write(struct rn8302b_dev *dev,uint16_t addr,uint32_t val, uint16_t len);
int rn8302b_read(struct rn8302b_dev *dev,uint16_t addr,uint32_t *val, uint16_t len);
int rn8302b_dev_init(struct rn8302b_dev *dev);
int rn8302b_write_protect_dis(struct rn8302b_dev *dev);
int rn8302b_write_protect_en(struct rn8302b_dev *dev);

// esp_err_t rn8302b_get_param_from_nvs(struct rn8302b_dev *dev, const char* key, void* value, size_t* length);

int rn8302b_calibrate_gain(struct rn8302b_dev *dev,uint16_t voltage,uint16_t current);
int rn8302b_calibrate_phase(struct rn8302b_dev *dev,uint16_t voltage,uint16_t current);
int rn8302b_calibrate_offset(struct rn8302b_dev *dev,uint16_t voltage,uint16_t current);
int rn8302b_set_conf_param(struct rn8302b_dev *dev,rn8302b_conf_param_t *conf);

void rn8302b_poll_metric(struct rn8302b_dev *dev);
#ifndef RN8302B_POLL_WITHOUT_ENERGY
void rn8302b_poll_energy(struct rn8302b_dev *dev);
#endif

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_RN8302B_RN8302B_H_ */
