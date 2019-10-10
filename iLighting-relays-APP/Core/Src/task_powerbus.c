/*
 * task_powerbus.c
 *
 *  Created on: 2019年9月27日
 *      Author: luoh
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32_assert.h"

#include "task_list.h"

#include "main.h"

#include "protocol_powerbus/include/protocol_powerbus.h"

/* Private macros ------------------------------------------------------------*/
#define TASK_POWERBUS_THREAD_STACKSIZE    (400)
#define TASK_POWERBUS_THREAD_PRIORITY     (configMAX_PRIORITIES - 1)

#define POWERBUS_BUF_SIZE              (256)

#define POWERBUS_TX_Q_SIZE             (5)

/* Private types -------------------------------------------------------------*/
typedef struct tx_queue_item_{
  uint8_t *p;
  uint16_t len;
}__packed tx_queue_item_t;

/* Private constants ---------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t task_powerbus_comm_handle;

//static xQueueHandle rx_queue;
static QueueHandle_t tx_queue;

static uint8_t recv_buf[POWERBUS_BUF_SIZE];
static __IO uint16_t recv_len;

static __IO uint8_t ubTransmissionComplete = 0;

static protocol_t proto;
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void configure_uart(void);
static void configure_dma(void);

static void process_tx_queue(void);

static void task_powerbus_func(void * args);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function configures USARTx Instance.
  * @note   This function is used to :
  *         -1- Enable GPIO clock, USART1 clock and configures the USART1 pins.
  *         -2- Configure USART1 functional parameters.
  *         -3- Enable USART2.
  * @note   Peripheral configuration is minimal configuration from reset values.
  *         Thus, some useless LL unitary functions calls below are provided as
  *         commented examples - setting is default configuration from reset.
  * @param  None
  * @retval None
  */
void configure_uart(void)
{

  /* (1) Enable GPIO clock and configures the USART pins **********************/

  /* Enable the peripheral clock of GPIO Port */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

  /* Enable USART2 peripheral clock and clock source *****************/
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  /* Configure Tx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_2, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_2, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_2, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_2, LL_GPIO_PULL_UP);

  /* Configure Rx Pin as : Alternate function, High Speed, Push pull, Pull up */
  LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_ALTERNATE);
  LL_GPIO_SetPinSpeed(GPIOA, LL_GPIO_PIN_3, LL_GPIO_SPEED_FREQ_HIGH);
  LL_GPIO_SetPinOutputType(GPIOA, LL_GPIO_PIN_3, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_3, LL_GPIO_PULL_UP);


  /* (2) Configure USART2 functional parameters ********************************/

  /* Disable USART prior modifying configuration registers */
  /* Note: Commented as corresponding to Reset value */
  // LL_USART_Disable(USART2);

  /* TX/RX direction */
  LL_USART_SetTransferDirection(USART2, LL_USART_DIRECTION_TX_RX);

  /* 8 data bit, 1 start bit, 1 stop bit, no parity */
  LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

  /* No Hardware Flow control */
  /* Reset value is LL_USART_HWCONTROL_NONE */
//   LL_USART_SetHWFlowCtrl(USART2, LL_USART_HWCONTROL_NONE);

  /* Set Baudrate to 115200 using APB frequency set to 36000000 Hz */
  /* Frequency available for USART peripheral can also be calculated through LL RCC macro */
  /* Ex :
      Periphclk = LL_RCC_GetUSARTClockFreq(Instance); or LL_RCC_GetUARTClockFreq(Instance); depending on USART/UART instance

      In this example, Peripheral Clock is expected to be equal to 36000000 Hz => equal to SystemCoreClock/2
  */
  LL_USART_SetBaudRate(USART2, SystemCoreClock/2, 9600);

  /* (3) Enable USART2 **********************************************************/
  LL_USART_Enable(USART2);
}

/**
  * @brief  This function configures the DMA Channels for TX and RX transfers
  * @note   This function is used to :
  *         -1- Enable DMA1 clock
  *         -2- Configure NVIC for DMA transfer complete/error interrupts
  *         -3- Configure DMA TX channel functional parameters
  *         -4- Configure DMA RX channel functional parameters
  *         -5- Enable transfer complete/error interrupts
  * @param  None
  * @retval None
  */
static void configure_dma(void)
{
  /* DMA1 used for USART2 Transmission and Reception
   */
  /* (1) Enable the clock of DMA1 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

  /* (2) Configure NVIC for DMA transfer complete/error interrupts */
//  NVIC_SetPriority(DMA1_Channel7_IRQn, 0);
//  NVIC_EnableIRQ(DMA1_Channel7_IRQn);
//  NVIC_SetPriority(DMA1_Channel6_IRQn, 0);
//  NVIC_EnableIRQ(DMA1_Channel6_IRQn);

  /* DMA1_Channel6_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  NVIC_SetPriority(DMA1_Channel7_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
  NVIC_EnableIRQ(DMA1_Channel7_IRQn);

  /* (3) Configure the DMA functional parameters for transmission */
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_7,
                        LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);
//  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_7,
//                         (uint32_t)buf,
//                         LL_USART_DMA_GetRegAddr(USART2),
//                         LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7));
//  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, POWERBUS_BUF_SIZE);

  /* (4) Configure the DMA functional parameters for reception */
  LL_DMA_ConfigTransfer(DMA1, LL_DMA_CHANNEL_6,
                        LL_DMA_DIRECTION_PERIPH_TO_MEMORY |
                        LL_DMA_PRIORITY_HIGH              |
                        LL_DMA_MODE_NORMAL                |
                        LL_DMA_PERIPH_NOINCREMENT         |
                        LL_DMA_MEMORY_INCREMENT           |
                        LL_DMA_PDATAALIGN_BYTE            |
                        LL_DMA_MDATAALIGN_BYTE);
  LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_6,
                         LL_USART_DMA_GetRegAddr(USART2),
                         (uint32_t)recv_buf,
                         LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6));
  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, POWERBUS_BUF_SIZE);

  /* (5) Enable DMA transfer complete/error interrupts  */
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_7);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_7);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);
  LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_6);
}


static void start_tx(void)
{
  tx_queue_item_t item;
//  UBaseType_t uxNumberOfItems = uxQueueMessagesWaiting(tx_queue);
//  if(uxNumberOfItems)
//  {
    xQueueReceive(tx_queue,&item,0);

    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_7,
                           (uint32_t)item.p,
                           LL_USART_DMA_GetRegAddr(USART2),
                           LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7));
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, item.len);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_7);
//  }
}

static void task_powerbus_func(void * args)
{
  (void)(args);


  /* Enable UART DMA RX Mode */
  LL_USART_EnableDMAReq_RX(USART2);

  /* Enable UART DMA TX Mode */
  LL_USART_EnableDMAReq_TX(USART2);

  /* Enable UART IDLE Interrupt */
  LL_USART_EnableIT_IDLE(USART2);

  /* Enable DMA Channel Rx */
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);


  while(1)
  {
    //    ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
    proc_msg(&proto,recv_buf,recv_len);

  }
  vTaskDelete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void initialize_task_powerbus(void)
{
  configure_uart();
  configure_dma();

  BaseType_t status ;

  proto.tx_queue = xQueueCreate(POWERBUS_TX_Q_SIZE,sizeof(tx_queue_item_t));
  assert_param(proto.tx_queue != NULL);

  proto.start_tx = start_tx;

  status = xTaskCreate(task_powerbus_func,
                       "pb_comm",
                       TASK_POWERBUS_THREAD_STACKSIZE,
                       NULL,
                       TASK_POWERBUS_THREAD_PRIORITY,
                       &task_powerbus_comm_handle);

  if (status != pdPASS)
  {
      vTaskDelete(task_powerbus_comm_handle);
  }
  assert_param(task_powerbus_comm_handle != NULL); // The task is created but handle is incorrect
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if(LL_USART_IsActiveFlag_IDLE(USART2))
  {
    LL_USART_ClearFlag_IDLE(USART2);

    LL_USART_DisableDMAReq_TX(USART2);
    LL_USART_DisableDMAReq_RX(USART2);
    recv_len = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
    /* signal kind of IPC to task*/

  }
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/**
  * @brief This function handles DMA1 channel6 global interrupt.
  */
void DMA1_Channel6_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC6(DMA1))
  {
    LL_DMA_ClearFlag_GI6(DMA1);
    /* Call function Reception complete Callback */
//    DMA1_ReceiveComplete_Callback();
  }
  else if(LL_DMA_IsActiveFlag_TE6(DMA1))
  {
    /* Call Error function */
//    USART_TransferError_Callback();
  }
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  if(LL_DMA_IsActiveFlag_TC7(DMA1))
  {
    LL_DMA_ClearFlag_GI7(DMA1);
    /* Call function Transmission complete Callback */
//    DMA1_TransmitComplete_Callback();
    ubTransmissionComplete = 1;
  }
  else if(LL_DMA_IsActiveFlag_TE7(DMA1))
  {
    /* Call Error function */
//    USART_TransferError_Callback();
  }
}
