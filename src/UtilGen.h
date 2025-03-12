/******************************************************************************/

/**
 *  \file    UtilGen.h
 *  \author  https://github.com/gwyacnt/
 *  \brief   General utilities component definitions
 *
 *  \remarks 
 */

#ifndef __UTIL_GEN_H__
#define __UTIL_GEN_H__
#include <stdint.h>
 
/******************************************************************************/
/* Exported defines and types                                                 */
/******************************************************************************/
/**
 *  \brief  Error Handling values
 */
#define UTIL_GEN_SUCCESS         0x00
#define UTIL_GEN_ERROR_GENERIC   0xE0
#define UTIL_GEN_ERROR_NULL_PTR  0xE9

/******************************************************************************/
/* Exported data                                                              */
/******************************************************************************/

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/

void UtilGen_delay_ms(int32_t ms);
#endif /* defined __UTIL_GEN_H__ */
