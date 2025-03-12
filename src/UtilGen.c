/******************************************************************************/

/**
 *  \file    UtilGen.c
 *  \author  https://github.com/gwyacnt/
 *  \brief   General utilities component implementation
 *
 *  \remarks 
 */


#include <stdint.h>
#include "UtilGen.h"

// Target specific includes
#include <zephyr/kernel.h>

/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/

void UtilGen_delay_ms(int32_t ms)
{
    k_msleep(ms);
}

/**
 * \}
 * End of file.
 */