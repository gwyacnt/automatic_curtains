/******************************************************************************
*  \file    HalScsUart.c
*  \author  Philips Intellectual Property & Standards, The Netherlands
*  \brief   SCS Firmware - Uart component implementation
*
*  \remarks (c) Copyright 2024 Koninklijke Philips N.V.
*                All rights reserved.
******************************************************************************/
#include "HalScsUart.h"
#include "stm32f4xx_hal.h"
#include "HalScsSys.h"
#include "HalScsGpio.h"
#include "stm32f4xx_hal_dma.h"
#include "UtilRingBuffer.h"

/******************************************************************************/
/* Local defines and types                                                    */
/******************************************************************************/
#define RX_BUF_SIZE     256
#define BAUDRATE        115200

/******************************************************************************/
/* Local data                                                                 */
/******************************************************************************/
static UART_HandleTypeDef huart2;
static DMA_HandleTypeDef  hdma_usart2_tx;
static DMA_HandleTypeDef  hdma_usart2_rx;

/**
 *  \brief Buffer to store received data
 */
static uint8_t rxBuffer[RX_BUF_SIZE]; 

/**
 *  \brief Rx data ring buffer
 */
static UtilRingBuffer_t sUartRxRingBuff;

/**
 *  \brief Variable to hold the registered callback for Tx completion
 */
static HalScsUart_TxCompleteCallback_t TxCompleteCallback = NULL;

/******************************************************************************/
/* Local function prototypes                                                  */
/******************************************************************************/
static void MX_USART2_UART_Init(void);

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
void HalScsUart_Init(void)
{
    MX_USART2_UART_Init();
    // Start Continuous Reception (DMA_CIRCULAR rx mode)
    HAL_UART_Receive_DMA(&huart2, rxBuffer, RX_BUF_SIZE);
    // Initialize UART Rx ring buffer - Keep in mind that writing to this ring buffer is done by DMA... not by UtilRingBuffer_Write()
    UtilRingBuffer_Init( &sUartRxRingBuff, &rxBuffer[0], sizeof(rxBuffer), UTIL_RINGBUFFER_WRITEWRAP_ALLOWED);
}

/******************************************************************************/
int HalScsUart_GetUartChar(void)
{
    uint8_t retChar = 0; // Use a local variable to store the character.
    static uint32_t availableData = 0;
    availableData = UtilRingBuffer_GetUsedSize(&sUartRxRingBuff); // Get the amount of data available in the ring buffer.
    
    if (availableData > 0) // Check if there is at least one byte to read.
    {
        UtilRingBuffer_Read(&sUartRxRingBuff, &retChar, 1); // Read one byte from the ring buffer.
        return retChar;
    }
    
    return -1; // Return the read character or Return -1 if no data is available to read.
}

/******************************************************************************/
void HalScsUart_UpdateRingBufferFromDMA(void)
{
  uint32_t dmaCurrentIndex = RX_BUF_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
  static uint32_t rxFree;

  UtilRingBuffer_SyncWithDMA(&sUartRxRingBuff, dmaCurrentIndex);

  
  rxFree = UtilRingBuffer_GetFreeSize(&sUartRxRingBuff);
  if(rxFree <= 0)
  {
    HalScsSys_ErrorHandler();
  }
}

/******************************************************************************/
uint8_t HalScsUart_transmit(const uint8_t *pData, uint16_t Size)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  // Start the DMA UART transmission
  status = HAL_UART_Transmit_DMA(&huart2, pData, Size);
  if(status != HAL_OK) 
  {
      // Handle possible errors
  }
  return (uint8_t)status;
}

/******************************************************************************/
bool HalScsUart_isConnected(void)// always present... needs investigation
{
    bool rv = false;

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin         = USART_RX_Pin;
    GPIO_InitStruct.Mode        = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull        = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART_RX_GPIO_Port, &GPIO_InitStruct);

    HAL_Delay(2);

    if (USART_RX_GPIO_Port->IDR & USART_RX_Pin)
    {
        /* high, then it is present! */
        rv = true;
    }
  
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate   = GPIO_AF7_USART2;
    
    HAL_GPIO_Init(USART_RX_GPIO_Port, &GPIO_InitStruct);
    
    return rv;
}

/******************************************************************************/
// Function to register the Tx callback
void HalScsUart_RegisterTxCallback(HalScsUart_TxCompleteCallback_t callback) 
{
    TxCompleteCallback = callback;
}

/******************************************************************************/
/* Callback functions                                                         */
/******************************************************************************/
/**
* @brief UART MSP Initialization
* This function configures the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_TX Init */
    hdma_usart2_tx.Instance = DMA1_Stream6;
    hdma_usart2_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK)
    {
      HalScsSys_ErrorHandler();
    }
    __HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream5;
    hdma_usart2_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    // DMA_CIRCULAR to continuously receive data on UART2 and handle the incoming data asynchronously as it arrives, 
    // without needing to manually initiate each reception. 
    // This mode allows the DMA to automatically restart the transfer after reaching the end of the buffer, effectively creating a loop
    hdma_usart2_rx.Init.Mode = DMA_CIRCULAR; 
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      HalScsSys_ErrorHandler();
    }
    __HAL_LINKDMA(huart,hdmarx,hdma_usart2_rx);

    /* DMA1_Stream5_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, SCS_HAL_SYS_IRQ_PRIO_DEBUG_UART_RX_DMA, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
    /* DMA1_Stream6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, SCS_HAL_SYS_IRQ_PRIO_DEBUG_UART_TX_DMA, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  }
}

/******************************************************************************/
/**
* @brief UART MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param huart: UART handle pointer
* @retval None
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
  if(huart->Instance==USART2)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(huart->hdmatx);
    HAL_DMA_DeInit(huart->hdmarx);

    /* USART2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
}

/******************************************************************************/

bool HalScsUart_GetDMAState(void)
{
   if(HAL_DMA_GetState(&hdma_usart2_tx) == HAL_DMA_STATE_READY)
   {
    return true;
   }
   else
   {
    return false;
   }
}

/******************************************************************************/
/* IRQ Handlers                                                               */
/******************************************************************************/
/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/******************************************************************************/
/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
void DMA1_Stream6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
}

/******************************************************************************/
/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

/******************************************************************************/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        if (TxCompleteCallback != NULL) 
        {
            TxCompleteCallback();  // Call the registered callback
        }
    }
}

/******************************************************************************/
/* Local functions                                                            */
/******************************************************************************/
/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = BAUDRATE;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    HalScsSys_ErrorHandler();
  }
  HAL_NVIC_SetPriority(USART2_IRQn, SCS_HAL_SYS_IRQ_PRIO_DEBUG_UART, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}


/**
 * \}
 * End of file.
 */

