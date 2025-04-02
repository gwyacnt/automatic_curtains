/******************************************************************************/
/**
 *  \file    SrvMenu.h
 *  \author  Philips Intellectual Property & Standards, The Netherlands
 *  \brief   SCS Firmware - Console Menu service
 *
 *  \remarks (c) Copyright 2024 Koninklijke Philips N.V.
 *                All rights reserved.
 * 
 *  \remarks Service module for control/service/debug menu
 */

#ifndef __SCS_SRV_MENU_H__
#define __SCS_SRV_MENU_H__

/*
Notes on parsing output:
 \n#  : Line starting with # is comment and extra info 
 \n>  : Line starting with > is response to previous command
 \*   : Line starting with * is an error response (this can replace a response line)
*/

#include <stdint.h>

#ifndef STRINGIZE
#define STRINGIZE(x) #x
#endif

#define SRV_MENU_CHAR_HANDLED    0x10000
#define SRV_MENU_CHAR_ERROR      0x20000
#define SRV_MENU_CHAR_FINISHED   0x40000

#define SRV_MENU_IS_CHAR_HANDLED(c)      ((c) & SRV_MENU_CHAR_HANDLED)
#define SRV_MENU_IS_CHAR_ERROR(c)        ((c) & SRV_MENU_CHAR_ERROR)
#define SRV_MENU_IS_CHAR_FINISHED(c)     ((c) & SRV_MENU_CHAR_FINISHED)
#define SRV_MENU_CHAR(c)                 ((c) & 0xffff)
 



typedef struct srvMenuElement_tag srvMenuElement_ts;

typedef int srvMenuTypeInChar_ft(const srvMenuElement_ts * elm, int chr);
typedef int srvMenuTypeStr_ft(const srvMenuElement_ts * elm, char * str, int max);
typedef int srvMenuTypeEval_ft(const srvMenuElement_ts * elm, char * str);
typedef int srvMenuTypeAction_ft(const srvMenuElement_ts * elm, int opt);



#define _MENU_VALUE_STR         0x80
#define _MENU_VALUE_NR_MSK      0xE0
#define _MENU_VALUE_UINT        0x00
#define _MENU_VALUE_SINT        0x20
//#define _MENU_VALUE_ENUM        0x40
#define _MENU_VALUE_FLOAT       0x60

#define MENU_VALUE_STR(s)       (_MENU_VALUE_STR + ((s)&~_MENU_VALUE_STR))
#define MENU_VALUE_INT(s)       (((s)<0)?-s|_MENU_VALUE_SINT:s|_MENU_VALUE_UINT)
#define MENU_VALUE_FLOAT(s)     (_MENU_VALUE_FLOAT+(s))
#define MENU_VALUE_ENUM(s)      (_MENU_VALUE_ENUM+(s))

#define MENU_VALUE_IS_STR(v)    ((v)&_MENU_VALUE_STR)
#define MENU_VALUE_IS_UINT(v)   (((v)& _MENU_VALUE_NR_MSK)== _MENU_VALUE_UINT)
#define MENU_VALUE_IS_SINT(v)   (((v)& _MENU_VALUE_NR_MSK)== _MENU_VALUE_SINT)
#define MENU_VALUE_IS_INT(v)    (MENU_VALUE_IS_UINT(v)|MENU_VALUE_IS_SINT(v))
#define MENU_VALUE_IS_FLOAT(v)  (((v)& _MENU_VALUE_NR_MSK)== _MENU_VALUE_FLOAT)
//#define MENU_VALUE_IS_ENUM(v)   ((v)& (_MENU_VALUE_NR_MSK)== _MENU_VALUE_ENUM)


#define MENU_VALUE_FMT_HEX      0x01
#define MENU_VALUE_FMT_DEC      0x02
#define MENU_VALUE_FMT_SIGNED   0x04
#define MENU_VALUE_FMT_LIMITS   0x40
#define MENU_VALUE_FMT_NO_LIMITS   0
#define MENU_VALUE_FMT_WRITABLE 0x80


#define MENU_ENUM_FMT_VALUES      1
#define MENU_ENUM_FMT_BITFIELDS   2
//#define MENU_ENUM_FMT_NOCASE      4




typedef struct srvMenuType_tag
{
    const char              * name;     //eg: "menu"/"value"
    srvMenuTypeStr_ft       * str;
    srvMenuTypeEval_ft      * eval;
    srvMenuTypeInChar_ft    * inChar;
}srvMenuType_ts;



/* This struct is used to describe a value/menu/data entity. */
struct srvMenuElement_tag
{
    const char            * name;            //The name of the data. eg: "frequency"
    const char            * desc;            //A description of the data. eg: "Output frequency in Hz"
    const srvMenuType_ts  * typ;             //A reference to the type of a basic data type. eg: &SRV_MENU_TYPE_VALUE
    const void            * cfg;             //A reference to the configuration of above mentioned data type:
    void                  * storage;         //A reference to the data described. eg: &myIntVariable
};
   
   
/* This struct is an element of a menu entry or action parameter  */
/* This struct can be pointed to by a srvMenuElement_ts.cfg field */
typedef struct srvMenuList_tag
{
    const srvMenuElement_ts * elm;  // Reference to described data
    const int                 opt;  // Options like a menu shortcut char
}srvMenuList_ts;


/* Configuration of a value type  */
/* This struct can be pointed to by a srvMenuElement_ts.cfg field */
typedef struct srvMenuValueCfg_tag
{
    const uint8_t     typ;              // type: string/int/float
    const uint8_t     fmt;              // string format; write permission;
//    const uint16_t    len;              //
    const uint32_t    min;              // minimum allowed value
    const uint32_t    max;              // maximum allowed value
}srvMenuValueCfg_ts;


/* This struct is used to define the configuration for an action */
/* This struct can be pointed to by a srvMenuElement_ts.cfg field */
typedef struct srvMenuActionCfg_tag
{
    srvMenuTypeAction_ft   * action;    // Pointer to an action function
    const int                option;    // Usable for easy distinction in the action function
    const srvMenuList_ts     params[];  // List of action parameters
}srvMenuActionCfg_ts;



/* This struct is used to describe an enum value. */
typedef struct srvEnumElement_tag
{
    const char            * name;       // The name of the enum value eg:"B"
    const char            * desc;       // A description of the enum value. eg:"Use channel B"
    const uintptr_t       value;        // The value associated with this enum value
}srvEnumElement_ts;



typedef struct srvMenuEnumCfg_tag
{
    const int                option;    // Usable for easy distinction in the action function
    const srvEnumElement_ts* enumVals[];  // List of action parameters
}srvMenuEnumCfg_ts;






extern const srvMenuType_ts SRV_MENU_TYPE_MENU;
extern const srvMenuType_ts SRV_MENU_TYPE_VALUE;
extern const srvMenuType_ts SRV_MENU_TYPE_ACTION;
extern const srvMenuType_ts SRV_MENU_TYPE_ENUM;

extern const srvMenuValueCfg_ts SRV_MENU_TYPE_VALUE_CFG_ZSTRING;
extern const srvMenuValueCfg_ts SRV_MENU_TYPE_VALUE_CFG_LOGGING;





void SrvMenu_Init(void);
void SrvMenu_Process(void);



/**
 * \brief  Macros to help create menu elements:
To create an action:
  1) define params of the action with \ref SRVMENU_ACTIONPARAMSTR and or \ref SRVMENU_ACTIONPARAMINT
  2) define action itself with SRVMENU_ACTION_BEGIN, SRVMENU_ACTION_PARAM and SRVMENU_ACTION_END

 */



#define SRVMENU_ACTIONPARAMSTRREF(actionName, var, paramName, text) \
  static const srvMenuValueCfg_ts action_##actionName##_##paramName##_CFG = {MENU_VALUE_STR(sizeof(var)), MENU_VALUE_FMT_WRITABLE, 0, 0}; \
  static const srvMenuElement_ts action_##actionName##_##paramName##_DESC = {STRINGIZE(paramName), text, &SRV_MENU_TYPE_VALUE, &action_##actionName##_##paramName##_CFG, &var}

/**
 *  \brief  SRVMENU_ACTIONPARAMSTR create a string variable named 'action_<actionName>_<paramName>' and needed structs.
 *  \param  actionName: name of action this parameter belongs to
 *  \param  typ: type (char/int8_t/uint8_t) for string. This can include 'static'.
 *  \param  size: the size of the string
 *  \param  paramName: is the name of the parameter
 *  \param  text: short help text to describes the parameter
 */
#define SRVMENU_ACTIONPARAMSTR(actionName, typ, size, paramName, text) \
  typ action_##actionName##_##paramName[size]; \
  SRVMENU_ACTIONPARAMSTRREF(actionName, action_##actionName##_##paramName, paramName, text)
  
/**
 *  \brief  SRVMENU_ACTIONPARAMINT create a variable named 'action_<actionName>_<paramName>' and needed structs.
 *  \param  actionName: name of action this parameter belongs to
 *  \param  typ: give the type of this variable. This can include 'static'. Eg: static uint32_t
 *  \param  paramName: is the name of the parameter
 *  \param  text: short help text to describes the parameter
 *  \param  opt: Use MENU_VALUE_FMT_LIMITS or MENU_VALUE_FMT_NO_LIMITS
 *  \param  low: The lower limit when MENU_VALUE_FMT_LIMITS is used for 'opt'
 *  \param  high: The upper limit when MENU_VALUE_FMT_LIMITS is used for 'opt'
 */

#define SRVMENU_ACTIONPARAMINT(actionName, typ, paramName, text, opt, low, high) \
  typ action_##actionName##_##paramName; \
  static const srvMenuValueCfg_ts action_##actionName##_##paramName##_CFG = {MENU_VALUE_INT(sizeof(action_##actionName##_##paramName)), MENU_VALUE_FMT_WRITABLE | opt, low, high}; \
  static const srvMenuElement_ts action_##actionName##_##paramName##_DESC = {STRINGIZE(paramName), text, &SRV_MENU_TYPE_VALUE, &action_##actionName##_##paramName##_CFG, &action_##actionName##_##paramName}

  
/**
 *  \brief  SRVMENU_ACTIONPARAMFLOAT create a variable named 'action_<actionName>_<paramName>' and needed structs.
 *  \param  actionName: name of action this parameter belongs to
 *  \param  typ: give the type of this variable. This can include 'static'. Eg: static uint32_t
 *  \param  paramName: is the name of the parameter
 *  \param  text: short help text to describes the parameter
 *  \param  opt: Use MENU_VALUE_FMT_LIMITS or MENU_VALUE_FMT_NO_LIMITS
 *  \param  low: The lower limit when MENU_VALUE_FMT_LIMITS is used for 'opt'
 *  \param  high: The upper limit when MENU_VALUE_FMT_LIMITS is used for 'opt'
 */
#define SRVMENU_ACTIONPARAMFLOAT(actionName, typ, paramName, text, opt, low, high) \
  typ action_##actionName##_##paramName; \
  static const srvMenuValueCfg_ts action_##actionName##_##paramName##_CFG = {MENU_VALUE_FLOAT(sizeof(action_##actionName##_##paramName)), MENU_VALUE_FMT_WRITABLE | opt, low, high}; \
  static const srvMenuElement_ts action_##actionName##_##paramName##_DESC = {STRINGIZE(paramName), text, &SRV_MENU_TYPE_VALUE, &action_##actionName##_##paramName##_CFG, &action_##actionName##_##paramName}

  
#if 0
/**
 *  \brief  SRVMENU_ACTIONPARAMENUM create an enum variable named 'action_<actionName>_<paramName>' and needed structs.
 *  \param  actionName: name of action this parameter belongs to
 *  \param  typ: give the type of this variable. This can include 'static'. Eg: static uint32_t
 *  \param  paramName: is the name of the parameter
 *  \param  text: short help text to describes the parameter
 *  \param  enums: name of enum list
 */
#define SRVMENU_ACTIONPARAMENUM(actionName, typ, paramName, text, enums) \
  typ action_##actionName##_##paramName; \
  static const srvMenuValueCfg_ts action_##actionName##_##paramName##_CFG = {MENU_VALUE_INT(sizeof(action_##actionName##_##paramName)), MENU_VALUE_FMT_WRITABLE | opt, low, high}; \
  static const srvMenuElement_ts action_##actionName##_##paramName##_DESC = {STRINGIZE(paramName), text, &SRV_MENU_TYPE_VALUE, &action_##actionName##_##paramName##_CFG, &action_##actionName##_##paramName}
#endif

  
/**
 *  \brief  SRVMENU_ACTION_BEGIN Is the first macro to use to create a an action.
 *          A function of the name 'action_<actionName>' of type srvMenuTypeAction_ft
 *          needs to be created within scope of this file.
 *  \param  actionName: name of action
 */
#define SRVMENU_ACTION_BEGIN(actionName) \
  static srvMenuTypeAction_ft action_##actionName; \
  static const srvMenuActionCfg_ts elem_##actionName##Cfg = {&action_##actionName, 0, {

/**
 *  \brief  SRVMENU_ACTION_PARAM add an earlier defined param for the action named 'actionName'
 *  \param  actionName: name of action
 *  \param  paramName: name of parameter
 */
#define SRVMENU_ACTION_PARAMREF(actionName, ref, sc) {&ref, sc},
#define SRVMENU_ACTION_PARAM(actionName, paramName, sc) SRVMENU_ACTION_PARAMREF(actionName, action_##actionName##_##paramName##_DESC, sc)

    
/**
 *  \brief  SRVMENU_ACTION_END will end the action named 'actionName'
 *  \param  actionName: name of action
 *  \param  text: short help text to explain this action
 */
#define SRVMENU_ACTION_END(actionName, text) {0,0}}}; \
  static const srvMenuElement_ts elem_##actionName = {STRINGIZE(actionName), text, &SRV_MENU_TYPE_ACTION, (void*)&elem_##actionName##Cfg, 0}
  


/**
 *  \brief  SRVMENU_ENUM_BEGIN Is the first macro to use to create an enum from earlier
 *          enum values.
 *  \param  enumName: name of enum
 *  \param  opt: Use MENU_ENUM_FMT_VALUES or MENU_ENUM_FMT_BITFIELDS
 */
#define SRVMENU_ENUM_BEGIN(enumName, opt) \
  static const srvMenuEnumCfg_ts enum_##enumName##_Cfg = {opt, {

/**
 *  \brief  SRVMENU_ENUM_VAL add an earlier defined enum value for the enum named 'enumName'
 *  \param  enumName: name of enum
 *  \param  enumVal: defined value
 */
#define SRVMENU_ENUM_VALREF(enumName,enumValRef) {&enumValRef},
#define SRVMENU_ENUM_VAL(enumName,enumVal) SRVMENU_ENUM_VALREF(enum_##enumName##_##enumVal##_DESC)
    
/**
 *  \brief  SRVMENU_ENUM_END will end the action named 'enumName'
 *  \param  actionName: name of action
 *  \param  text: short help text to explain this action
 */
#define SRVMENU_ENUM_END(enumName, text) NULL}}; \
    static uint32_t enum_##enumName##Value = 0; \
    static const srvMenuElement_ts elem_##enumName = {STRINGIZE(enumName), text, &SRV_MENU_TYPE_ENUM, (void*)&enum_##enumName##_Cfg, &enum_##enumName##Value}

  
  
  
#endif

