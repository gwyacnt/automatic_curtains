/*****************************************************************************/

/**
 * \addtogroup Philips SCS
 * \{
 */
 
/**
 *  \file    scsVersion.h
 *  \author  Philips Intellectual Property & Standards, The Netherlands
 *  \brief   System version file
 *
 *  \remarks (c) Copyright 2024 Koninklijke Philips N.V. All rights reserved.
 * 
 */

/*============================================================================*/
/*  Multiple inclusion protection                                             */
/*============================================================================*/

#ifndef __SCS_VERSION_H__
#define __SCS_VERSION_H__

/*============================================================================*/
/*  C++ protection                                                            */
/*============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* Include files                                                              */
/*============================================================================*/


/*============================================================================*/
/* Constant definitions                                                       */
/*============================================================================*/

#if !defined(SCS_VERSION_MAJOR)

# define SCS_VERSION_MAJOR       (0)
# define SCS_VERSION_MAJOR_STR   "0"

# define SCS_VERSION_MINOR       (0)
# define SCS_VERSION_MINOR_STR   "0"

# define SCS_VERSION_REVIS       (0)
# define SCS_VERSION_REVIS_STR   "0"

# define SCS_VERSION_PATCH       (0)
# define SCS_VERSION_PATCH_STR   "0"

# define SCS_VERSION_BID_STR "LocalBuild!"

#endif



#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define SCS_VERSION_VID_STR     "SCS_" SCS_VERSION_MAJOR_STR "." SCS_VERSION_MINOR_STR "." SCS_VERSION_REVIS_STR "." SCS_VERSION_PATCH_STR
#if defined ( DEBUG ) || defined (_DEBUG)
#define SCS_VERSION_STR SCS_VERSION_VID_STR "-" SCS_VERSION_BID_STR
#else
#define SCS_VERSION_STR SCS_VERSION_VID_STR "_" SCS_VERSION_BID_STR
#endif

/*============================================================================*/
/*  End of C++ protection                                                     */
/*============================================================================*/

#ifdef __cplusplus
}
#endif

/*============================================================================*/
/*  End of multiple inclusion protection                                      */
/*============================================================================*/

#endif

/**
 * \}
 * End of file.
 */ 
