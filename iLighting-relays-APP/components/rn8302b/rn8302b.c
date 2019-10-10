/*
 * rn8302b.c
 *
 *  Created on: 2019年7月31日
 *      Author: alien
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <assert.h>
#include <stdlib.h>
//#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"
#include "include/rn8302b.h"
#include "rn8302b_priv.h"

/* Private macros ------------------------------------------------------------*/
#define RN8302B_OSC_FREQ        (8.192e6) //晶振频率Hz

#define RMS_REFRESH_PERIOD      (250)    //有效值寄存器刷新时间
#define RN8302B_CALI_REPEAT_CNT    (10)   //校准重复读取次数

#define RN8302B_DEFAULT_I_RATIO         (1)
#define RN8302B_DEFAULT_V_RATIO         (1)
#define RN8302B_DEFAULT_I_DEADBAND      (5)
#define RN8302B_DEFAULT_V_DEADBAND      (50)

#define POWER_STD     ((int)((CURRENT_STD*VOLTAGE_STD)/((uint32_t)(1<<23))+0.5)) //四舍五入

#define GET_NVS_REAL_ID(dev,id)     (dev->nvs.id_offset + id)
/*
#ifdef RN8302B_USE_SLIST_API
#include <sys/queue.h>
typedef struct rn8302b_dev_item_{
  uint32_t id;
  struct rn8302b_dev dev;
  SLIST_ENTRY(rn8302b_dev_item_) next;
} rn8302b_dev_item_t;

static SLIST_HEAD(rn8302b_dev_list_, rn8302b_dev_item_) s_rn8302b_dev_list;
#endif
*/



/* Private types -------------------------------------------------------------*/


/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

//extern int printf(const char *format, ...);
/* Private function prototypes -----------------------------------------------*/
static void get_default_conf(rn8302b_conf_param_t *conf);
static void get_default_ratio(rn8302b_ratio_t *ratio);
static void get_default_deadband(rn8302b_deadband_t *deadband);

static int probe(struct rn8302b_dev *dev);

static void set_gain(struct rn8302b_dev *dev,rn8302b_gain_t *gain);
static void set_phase(struct rn8302b_dev *dev,rn8302b_phase_t *phase);
static void set_offset(struct rn8302b_dev *dev,rn8302b_offset_t *offset);

#ifdef RN8302B_ENERGY_STORGAE_ESP32_NVS
static int energy_frozen(struct rn8302b_dev *dev);
#endif

/* Private functions ---------------------------------------------------------*/
static void get_default_conf(rn8302b_conf_param_t *conf)
{
    conf->e_const = 3200;
    conf->rated_vol = 220;
    conf->rated_cur = 5;
    conf->std_vol = 44000000;
    conf->std_cur = 60000000;
}

static void get_default_ratio(rn8302b_ratio_t *ratio)
{
  ratio->V[0] = RN8302B_DEFAULT_V_RATIO;
  ratio->V[1] = RN8302B_DEFAULT_V_RATIO;
  ratio->V[2] = RN8302B_DEFAULT_V_RATIO;
  ratio->I[0] = RN8302B_DEFAULT_I_RATIO;
  ratio->I[1] = RN8302B_DEFAULT_I_RATIO;
  ratio->I[2] = RN8302B_DEFAULT_I_RATIO;
  ratio->I[3] = RN8302B_DEFAULT_I_RATIO;
}

static void get_default_deadband(rn8302b_deadband_t *deadband)
{
  deadband->V[0] = RN8302B_DEFAULT_V_DEADBAND;
  deadband->V[1] = RN8302B_DEFAULT_V_DEADBAND;
  deadband->V[2] = RN8302B_DEFAULT_V_DEADBAND;
  deadband->I[0] = RN8302B_DEFAULT_I_DEADBAND;
  deadband->I[1] = RN8302B_DEFAULT_I_DEADBAND;
  deadband->I[2] = RN8302B_DEFAULT_I_DEADBAND;
  deadband->I[3] = RN8302B_DEFAULT_I_DEADBAND;
}

static int probe(struct rn8302b_dev *dev)
{
  int err;
  uint32_t id;

  rn8302b_read(dev,DeviceID,&id,3);

  err = ((id>>8) == 0x830200)?0:-1;

  return err;
}



static void set_gain(struct rn8302b_dev *dev,rn8302b_gain_t *gain)
{   
  /* 电压增益初始化 */
  for (int i = 0; i < 3;i++)
  {
      rn8302b_write(dev,GSUA + i,gain->V[i],2);
  }

  /* 电流增益初始化 */
  for (int i = 0; i < 4;i++)
  {
      rn8302b_write(dev,GSIA + i,gain->I[i],2);
  }
}

static void set_phase(struct rn8302b_dev *dev,rn8302b_phase_t *phase)
{
  for (int i = 0; i < 3;i++)
  {
      /* 功率相位校正 */
      rn8302b_write(dev,PA_PHS + i,phase->P[i],2);
      rn8302b_write(dev,QA_PHS + i,phase->Q[i],2);
  }
}

static void set_offset(struct rn8302b_dev *dev,rn8302b_offset_t *offset)
{
  /* 电流offset校正 */
  for (int i = 0; i < 4;i++)
  {
      rn8302b_write(dev,IA_OS + i,offset->I[i],2);
  }

}



#ifdef RN8302B_ENERGY_STORGAE_ESP32_NVS
static int energy_frozen(struct rn8302b_dev *dev)
{
    int err;
    err = nvs_set_blob(dev->nvs,"energy",dev->energy->data,sizeof(struct energy_data));
    //todo : failure prccess.
    return err;
}
#endif

/* Exported functions --------------------------------------------------------*/
/*
#ifdef RN8302B_USE_SLIST_API
int rn8302b_dev_register(struct rn8302b_dev *dev, uint32_t id)
{
  rn8302b_dev_item_t *item = (rn8302b_dev_item_t *)pvPortMalloc(sizeof(rn8302b_dev_item_t));
  if(item == NULL)
  {
    return -1;
  }

  if(dev == NULL)
  {
    vPortFree(item);
    return -2;

  }

  item->dev = *dev;
  item->id = id;

  rn8302b_dev_item_t *last = SLIST_FIRST(&s_rn8302b_dev_list);
  if (last == NULL) {
      SLIST_INSERT_HEAD(&s_rn8302b_dev_list, item, next);
  } else {
    rn8302b_dev_item_t *it;
      while ((it = SLIST_NEXT(last, next)) != NULL) {
          last = it;
      }
      SLIST_INSERT_AFTER(last, item, next);
  }

  return 0;
}
#endif

*/
/**
* @brief  
* @param  
* @retval 
**/
int rn8302b_write(struct rn8302b_dev *dev,uint16_t addr,uint32_t val, uint16_t len)
{
  assert(len<= 3);

  int rc;


  return rc;
}

/**
* @brief  
* @param  
* @retval 
**/
int rn8302b_read(struct rn8302b_dev *dev,uint16_t addr,uint32_t *val, uint16_t len)
{
  assert(len<= 4);

  int rc;

  uint8_t buf[5];
  memset(buf, 0, 5);


  return 0;
}

int rn8302b_write_protect_dis(struct rn8302b_dev *dev)
{
  return rn8302b_write(dev,WREN,0xdc,1);
}

int rn8302b_write_protect_en(struct rn8302b_dev *dev)
{
  return rn8302b_write(dev,WREN,0xe5,1);
}

int rn8302b_dev_init(struct rn8302b_dev *dev)
{
  int rc;

  rc = probe(dev);
  if(rc) return -1;

  size_t len;

  len = sizeof(rn8302b_ratio_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_RATIO),dev->ratio,len);
  if(rc != len)
  {
    get_default_ratio(dev->ratio);
  }

  len = sizeof(rn8302b_deadband_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_DEADBAND),dev->deadband,len);
  if(rc != len)
  {
    get_default_deadband(dev->deadband);
  }

  rn8302b_conf_param_t conf;
  get_default_conf(&conf);
  len = sizeof(rn8302b_conf_param_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_CONF),&conf,len);

  dev->calc.rms_vol = conf.std_vol/(conf.rated_vol*100);// 0.01V 电压对应的有效值寄存器标准值
  // ESP_LOGI(APP_TAG,"rms_vol: %d ",dev->calc.rms_vol);
  dev->calc.rms_cur = conf.std_cur/(conf.rated_cur*1000);//0.001A 电流对应的有效值寄存器标准值
  // ESP_LOGI(APP_TAG,"rms_cur: %d ",dev->calc.rms_cur);
  dev->calc.rms_power = dev->calc.rms_vol* (1.0f) * dev->calc.rms_cur*(1000.0f) / (1<< 23) ;//0.01W 功率对应的有效值寄存器标准值
  // ESP_LOGI(APP_TAG,"rms_power: %f ",dev->calc.rms_power);

  uint16_t hfconst  =  (dev->calc.rms_power*3.6e8*RN8302B_OSC_FREQ)/ (32.0f*conf.e_const *((uint32_t)(1<<31)));
  // ESP_LOGI(APP_TAG,"hfconst: %d ",hfconst);

  rn8302b_gain_t gain = {0};
  len = sizeof(rn8302b_gain_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_GAIN),&conf,len);

  rn8302b_phase_t phase ={0};
  len = sizeof(rn8302b_phase_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_PHASE),&conf,len);

  rn8302b_offset_t offset = {0};
  len = sizeof(rn8302b_offset_t);
  rc = nvs_read(dev->nvs.fs,GET_NVS_REAL_ID(dev,RN8302B_ID_OFFSET),&conf,len);


#ifndef RN8302B_POLL_WITHOUT_ENERGY

  RN8302B_CHECK(dev->energy != NULL,"calloc memory for dev->energy failed",-1);

  if(err != 0 )
  {
      memset(dev->energy,0,sizeof(struct energy_data));
  }
#endif

  rn8302b_write_protect_en(dev);

  rn8302b_write(dev,WMSW,0xa2,1); ///EMM模式

  rn8302b_write(dev,SOFTRST,0xfa,1); /// soft reset

  vTaskDelay(10 / portTICK_RATE_MS);

  rn8302b_write_protect_en(dev);

  rn8302b_write(dev,WMSW,0xa2,1); ///EMM模式

  rn8302b_write(dev,0x162,0x777777,3); ///计量控制：使能有功，无功，视在；基波有功，无功，视在计量

  rn8302b_write(dev,HFCONST1,hfconst,2);
  rn8302b_write(dev,HFCONST2,hfconst,2);

  set_gain(dev,&gain);
  set_phase(dev,&phase);
  set_offset(dev,&offset);

  rn8302b_write(dev,IStart_PS,0x250,2); /* 设置过零阈值寄存器 */
  rn8302b_write(dev,IStart_Q,0x250,2); /* 设置过零阈值寄存器 */
  rn8302b_write(dev,LostVoltT,0x0400,2); /* 设置过零阈值寄存器 */
  rn8302b_write(dev,ZXOT,0x73,2); /* 设置过零阈值寄存器 */

  rn8302b_write(dev,CFCFG,0x047777,3); /* CF引脚配置寄存器 */

  rn8302b_write(dev,EMUCFG,0x400040,3); /* 计量单元配置寄存器 */

  rn8302b_write(dev,WSAVECON,0x01,1); /* 清空采样数据缓存区 */

  rn8302b_write_protect_dis(dev);

  return 0;
}

void rn8302b_poll_metric(struct rn8302b_dev *dev)
{
    int32_t *temp = dev->table;
    int32_t val = 0;
    
//    spi_device_acquire_bus(dev->spi,portMAX_DELAY);

    /* 电压 放大100倍*/
    for (int i = 0; i < 3; i++)
    {
        rn8302b_read(dev,UA+i,(uint32_t *)&val,4);
        if(abs(val)<(dev->calc.rms_vol * dev->deadband->V[i]))
        {
            val = 0;
        }
        else
        {
            val =  (val / dev->calc.rms_vol) * (dev->ratio->V[i]);
        }
        
        *temp = val;
        temp++;
    }
    
    /* 电流 放大1000倍*/
    for (int i = 0; i < 4; i++)
    {
        rn8302b_read(dev,IA+i,(uint32_t *)&val,4);
        if(abs(val)<(dev->calc.rms_cur* dev->deadband->I[i]))
        {
            val = 0;
        }
        else 
        {
            val=  (val / dev->calc.rms_cur) * (dev->ratio->I[i] );
        }

        *temp = val;
        temp++;
    }

    /* A,B,C ；有功 功率   放大100倍*/
    for (int i = 0; i < 3; i++)
    {
        rn8302b_read(dev,PA+i,(uint32_t *)&val,4);
        val =  val  * (dev->ratio->V[i]) * (dev->ratio->I[i])  / dev->calc.rms_power;
        *temp = val;
        temp++;
    }

    /* A,B,C ；无功 功率  放大100倍*/
    for (int i = 0; i < 3; i++)
    {
        rn8302b_read(dev,PA+i,(uint32_t *)&val,4);
        val =  val  * (dev->ratio->V[i]) * (dev->ratio->I[i])  / dev->calc.rms_power;
        *temp = val;
        temp++;
    }

    /* A,B,C ；视在 功率  放大100倍*/
    for (int i = 0; i < 3; i++)
    {
        rn8302b_read(dev,PA+i,(uint32_t *)&val,4);
        val =  val  * (dev->ratio->V[i]) * (dev->ratio->I[i])  / dev->calc.rms_power;
        *temp = val;
        temp++;
    }

    /* 全波A ,B ,C , RMS合相 功率因数 放大1000倍*/
    for (int i = 0; i < 4; i++)
    {
        rn8302b_read(dev,PfA+i,(uint32_t *)&val,3);
        val = val >> 8;
        val = (val * 1000) / ((uint32_t)1<<23);
        *temp = val;
        temp++;
    } 
    
    /* 电压线频率*/
    rn8302b_read(dev,UFreq,(uint32_t *)&val,3);   
    val = (*(uint32_t*)&val)>>8;
    val = RN8302B_OSC_FREQ * 8 * 100 /val;
    *temp = val;
    temp++;

//    spi_device_release_bus(dev->spi);
}

#ifndef RN8302B_POLL_WITHOUT_ENERGY
void rn8302b_poll_energy(struct rn8302b_dev *dev)
{
    int32_t *temp = dev->table;
    temp = temp + RN8302B_BASE_DATA_CNT;
    int32_t val = 0;
    
    spi_device_acquire_bus(dev->spi,portMAX_DELAY);  
    /* A,B,C,合相；有功，正向有功，反向有功，无功，正向无功，反向无功，视在 电能*/
    for (int i = 0; i <RN8302B_MAX_ENERGY_DATA_CNT; i++)
    {
        rn8302b_read(dev,EP_A + i,((uint32_t)val),3);
        val = ((uint32_t)val)>>8;
        dev->energy->data[i] = dev->energy->data[i] + val;
        val = val / (RN8302B_EC_VALUE/100);
        *temp = val;
        temp++;
    }
    
    spi_device_release_bus(dev->spi);

    energy_frozen(dev);    
}
#endif

