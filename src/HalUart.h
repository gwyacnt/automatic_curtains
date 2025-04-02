/******************************************************************************/

/**
 *  \file    HalUart.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   UART component definitions
 *
 *  \remarks 
 */

#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/* Exported defines and types                                                 */
/******************************************************************************/
typedef void (*HalUart_TxCompleteCallback_t)(void);

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
/**
 *  \brief  Initialize UART
 */
void HalUart_Init(void);

/**
 *  \brief  Transmit buffer via DMA (non blocking)
 */
uint8_t HalUart_transmit(const uint8_t *pData, uint16_t Size);

/**
 *  \brief  Checks if there is a UART terminal is connected
 */
bool HalUart_isConnected(void);// Not working

/**
 *  \brief  Registers a callback function when Tx is complete
 */
void HalUart_RegisterTxCallback(HalUart_TxCompleteCallback_t callback);

bool HalUart_GetDMAState(void);

void HalUart_UpdateRingBufferFromDMA(void);

int HalUart_GetUartChar(void);
/******************************************************************************/
/* IRQ Handlers                                                               */
/******************************************************************************/
void    DMA1_Stream5_IRQHandler(void);
void    DMA1_Stream6_IRQHandler(void);
void    USART2_IRQHandler(void);


#endif /* defined ___HAL_UART_H__ */

