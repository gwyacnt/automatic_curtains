/******************************************************************************/
/**
 *  \file    SrvMenu.c
 *  \author  Philips Intellectual Property & Standards, The Netherlands
 *  \brief   SCS Firmware - Console Menu service
 *
 *  \remarks (c) Copyright 2024 Koninklijke Philips N.V.
 *                All rights reserved.
 */

#include <stdio.h>
#include <string.h>

#include "SrvMenu.h"
// #include "SrvFilesMenu.h"
// #include "SrvScripting.h"
// #include "SrvGui.h"
// #include "DrvDAC.h"
// #include "DrvIoExt.h"
#include "scsVersion.h"
#include "HalGpio.h"
#include <zephyr/kernel.h>
// #include "HalScsSys.h"
// #include "UtilFileHandling.h"
// #include "AppResistanceUpdate.h"
// #include "HalScsGpio.h"

/******************************************************************************/
/* Local defines and types                                                    */
/******************************************************************************/

// Also handle '/' as special input character:
#define INPUT_HANDLE_MENU_SEP_TOKEN     1


typedef struct menuItemData_tag
{
    int8_t   id;
    int8_t   idx;
    int8_t   max;
    char     str[];
}menuItemData_ts;


/******************************************************************************/
/* Local function prototypes                                                  */
/******************************************************************************/
static int ProcessChar(int _chr);
static void resetLineInput(void);
static char * rubOutStr(void);
static int keyFilter(int chr);



static const srvMenuElement_ts * getSubElement(const srvMenuElement_ts * pMenu, int idx);


//menu funcs
static srvMenuTypeStr_ft menuStr;
static int menuStr(const srvMenuElement_ts * elm, char * str, int max);
static srvMenuTypeInChar_ft menuCharIn;
static int menuCharIn(const srvMenuElement_ts * elm, int chr);
static srvMenuTypeEval_ft menuEval;
static int menuEval(const srvMenuElement_ts * elm, char * str);


static int inpMenuChr(menuItemData_ts * pMd, int chr, int options);
static int8_t findMenuEntry(const srvMenuList_ts * menuStart, char * name, const srvMenuType_ts * typ);
static void printMenuElement(const srvMenuElement_ts * elm);



//value funcs
static int valueStr(const srvMenuElement_ts * elm, char * str, int max);
static int valueEval(const srvMenuElement_ts * elm, char * str);
static int valuePrint(const srvMenuElement_ts * elm);

//enum funcs
static int enumStr(const srvMenuElement_ts * elm, char * str, int max);
static int enumEval(const srvMenuElement_ts * elm, char * str);
static int enumPrint(const srvMenuElement_ts * elm);

//action funcs
static int actionStr(const srvMenuElement_ts * elm, char * str, int max);
static int actionEval(const srvMenuElement_ts * elm, char * str);




static void detectTerminal(int cmd);

/******************************************************************************/
/* Exported data                                                                 */
/******************************************************************************/



const srvMenuType_ts SRV_MENU_TYPE_MENU = {"menu", &menuStr, &menuEval, NULL/*&menuCharIn*/ };
const srvMenuType_ts SRV_MENU_TYPE_VALUE = {"value", &valueStr, &valueEval, NULL };
const srvMenuType_ts SRV_MENU_TYPE_ACTION = {"action", &actionStr, &actionEval, NULL};
const srvMenuType_ts SRV_MENU_TYPE_ENUM = {"enum", &enumStr, &enumEval, NULL};


const srvMenuValueCfg_ts SRV_MENU_TYPE_VALUE_CFG_ZSTRING = {MENU_VALUE_STR(0), 0, 0, 0};
const srvMenuValueCfg_ts SRV_MENU_TYPE_VALUE_CFG_LOGGING = {MENU_VALUE_INT(1), MENU_VALUE_FMT_HEX | MENU_VALUE_FMT_WRITABLE, 0, 3};



/******************************************************************************/
/* Local data                                                                 */
/******************************************************************************/
static char consoleKnowsEscSeq = 0;
static char consoleCanHandleRubout = 0;

static int cursor_X = 0;
static int cursor_Y = 0;
static int cursor_U = 0;
static int termSize_X = 0;
static int termSize_Y = 0;


//----------------- Version string with menu description -------------------
static const char versionString[] = SCS_VERSION_STR;
static const srvMenuElement_ts elemFwVersion = {"ver", "FW version", &SRV_MENU_TYPE_VALUE, &SRV_MENU_TYPE_VALUE_CFG_ZSTRING, (void*)versionString};

//----------------- Help string with menu description -------------------
static const char helpString[] =
"\n#Navigation:"
"\n# '/' is (sub)menu selector. Short-key or menu-name both accepted."
"\n# '=' is assign."
"\n# 'Enter' is to show menu item or execute/assign."
"\n#Eg:"
"\n# '/h'    : Show top level menu item h."
"\n# 'h'     : Show local menu item h (submenu/value/action)."
"\n# 'd/'   : Select submenu 'd'."
"\n# 'f=44'  : Assign 44 to value menu item f."
"\n# 'a()'   : Do action a."
"\n# 'a(4)'  : Do action a with 4 as first parameter."
"\n# 'a(B,3)': Do action a with parameters B and 3."
"\n#Pressing Esc twice is a hard abort of the menu input."
;
static const srvMenuElement_ts elemHelp = {"help", "Console usage", &SRV_MENU_TYPE_VALUE, &SRV_MENU_TYPE_VALUE_CFG_ZSTRING, (void*) helpString};


//----------------- Reboot action string with menu description -------------------
SRVMENU_ACTION_BEGIN(reboot) \
SRVMENU_ACTION_END(reboot, "Reboot");


//----------------- Driver Menu -------------------

static const srvMenuList_ts drvMenu[] =
{
   {&elemGpioDrv,   (int)'g'}
  ,{NULL,0}
};
const srvMenuElement_ts elemDrv = {"drv", "Drivers", &SRV_MENU_TYPE_MENU, drvMenu, NULL};


//----------------- Top Menu -------------------
static const srvMenuList_ts topMenu[] =
{
   {&elemFwVersion, (int)'v'}
  ,{&elemDrv,       (int)'d'}
  ,{&elem_reboot,   (int)'r'}
  ,{&elemHelp,      (int)'h'}
  ,{NULL,0}
};
static const srvMenuElement_ts elemTopMenu = {"", "Skin Conductance Simulator. Main", &SRV_MENU_TYPE_MENU, topMenu, NULL};



//----------------- Menu-input parser -------------------
static const srvMenuElement_ts * browseMenu = &elemTopMenu;
static const srvMenuElement_ts * inpMenu = &elemTopMenu;
static const srvMenuElement_ts * setValueMenu = NULL;
static const srvMenuElement_ts * actionMenu = NULL;


static char inBuf[80];
static int inBufIdx=0;
static int inBufBrowseIdx=0;
static int inBufProcessedIdx=0;
static int inBufInpMenuStart=0;
static int actionParamIdx=0;



/******************************************************************************/
/* Exported functions                                                         */
/******************************************************************************/
/**
  * @brief console menu service Initialization Function
  * @param None
  * @retval None
  */
void SrvMenu_Init(void)
{
  resetLineInput();
}


/**
  * @brief Do regular needed processing
  * @param None
  * @retval None
  */
void SrvMenu_Process(void)
{
    int chr;

    chr = -1;

    if (chr==-1)
    {
        chr = keyFilter(-1);
        
        if (chr<0)
        {    
            chr = getchar();
            chr = keyFilter(chr);
        }
        detectTerminal(0);
    }
    if (chr>=0)
    {
        ProcessChar(chr);
    }
  
}


/******************************************************************************/
/**
  * @brief Process new char
  */
static int ProcessChar(int _chr)
{
    int chr = _chr;

    const srvMenuElement_ts * selectedMenuItem = NULL;
    static char tmpBuf[80+3]={0,0,80};
    menuItemData_ts * pBuf = 0;
    int res=chr;

    if ( inpMenu )
    {
        if ( inpMenu->typ->inChar )
        { // menu element will handle char
            res  = inpMenu->typ->inChar( inpMenu, chr );
            if (SRV_MENU_IS_CHAR_HANDLED(res) && !SRV_MENU_IS_CHAR_FINISHED(res))
            {
                return res;
            }
        }
        else
        { // collect chars to use eval() of menu element
            int options = 0;
            pBuf = (menuItemData_ts*)tmpBuf;
            if (   (inpMenu->typ == &SRV_MENU_TYPE_MENU)
                || (inpMenu->typ == &SRV_MENU_TYPE_ACTION)
                || (inpMenu->typ == &SRV_MENU_TYPE_ENUM))
            {
                options |= INPUT_HANDLE_MENU_SEP_TOKEN;
            }
            res = inpMenuChr(pBuf, chr, options);
        }

        if (SRV_MENU_IS_CHAR_FINISHED(res))
        { // full input of menu item
            int stat=-1;

            if (pBuf)
            {
                memcpy(&inBuf[inBufInpMenuStart], pBuf->str, pBuf->idx);
                inBufIdx = inBufInpMenuStart + pBuf->idx;
                inBuf[inBufIdx]=0;
            }

            pBuf->str[pBuf->idx]=0;
            if (   (inpMenu->typ == &SRV_MENU_TYPE_MENU)
                || (inpMenu->typ == &SRV_MENU_TYPE_ACTION)
               )
            {
                stat = inpMenu->typ->eval(inpMenu, &inBuf[inBufInpMenuStart]);
                if (stat>=0)
                {  // menu entry acceptable
                    int len;
                    selectedMenuItem = getSubElement(inpMenu, stat);
                    
                    len = strlen(selectedMenuItem->name);
                    memcpy(&inBuf[inBufInpMenuStart], selectedMenuItem->name, len);
                    inBufIdx=inBufInpMenuStart+len;
                    inBuf[inBufIdx]=0;
                }
            }
        }
        else
        {
            inBuf[inBufIdx++] = chr;
        }
    }
    else
    {
        res = chr | SRV_MENU_CHAR_FINISHED;
    }

    chr = res&255;    
    
    if (SRV_MENU_IS_CHAR_FINISHED(res))
    {
        switch(chr)
        {
          case 13:
            inBuf[inBufIdx]=0;
            printf("\n");
            if (actionMenu)
            { // do action
                const srvMenuActionCfg_ts * cfg = actionMenu->cfg;
                if (cfg)
                {
                    cfg->action(actionMenu, cfg->option);
                }
                actionMenu=NULL;
            }
            else if (setValueMenu)
            { // assign value
                int stat = setValueMenu->typ->eval(setValueMenu,&inBuf[inBufInpMenuStart]);
                if (stat<0)
                {
                    printf("\n!Error: readonly.");
                }
                printMenuElement(setValueMenu);
                setValueMenu=NULL;
            }
            else if (selectedMenuItem)
            { // print and possibly select menu item
                printMenuElement(selectedMenuItem);
                if (inBuf[inBufIdx-1]=='/')
                { // select menu item as browse menu
                    inBufBrowseIdx = inBufIdx;
                    browseMenu = selectedMenuItem;
                }
                selectedMenuItem = NULL;
            }
            else
            {
                if (inBufIdx == inBufProcessedIdx)
                {
                    if (inBuf[inBufIdx-1]=='/')
                    {
                        if (   (inpMenu->typ == &SRV_MENU_TYPE_MENU)
                            || (inpMenu->typ == &SRV_MENU_TYPE_ACTION)
                            || (inpMenu->typ == &SRV_MENU_TYPE_ENUM))
                      
                        { // select menu item as browse menu
                            inBufBrowseIdx = inBufIdx;
                            browseMenu = inpMenu;
                        }
                        else
                        {
                            printf("\n!***Can't browse into %s", inBuf);
                            inBuf[inBufBrowseIdx]=0;
                        }
                    }
                }
                else
                { // input is unknown
                    printf("\n!***Unknown Item***(%s)", inBuf);
                    inBuf[inBufBrowseIdx]=0;
                }
                printMenuElement(inpMenu);
            }
            
            inBufIdx = inBufBrowseIdx;
            inBufProcessedIdx = inBufIdx;
            inpMenu = browseMenu;
            inBufInpMenuStart = inBufIdx;
            
            pBuf->idx=0;
            if (chr!=10)
            {
                inBuf[inBufIdx] = 0;
            }

            printf("\n#%s:\n$",inBuf);
            break;

          case '=':
            if ((selectedMenuItem) && (  (selectedMenuItem->typ==&SRV_MENU_TYPE_VALUE)
                                       ||(selectedMenuItem->typ==&SRV_MENU_TYPE_ENUM)))
            {
                setValueMenu = selectedMenuItem;
                inBuf[inBufIdx++]=chr;
                inBufInpMenuStart = inBufIdx;
                pBuf->idx=0;
                printf("=");
            }
            break;

          case '(':
            if ((selectedMenuItem) && (selectedMenuItem->typ==&SRV_MENU_TYPE_ACTION))
            {
                const srvMenuActionCfg_ts * cfg;
                actionMenu = selectedMenuItem;
                cfg = actionMenu->cfg;
                inpMenu = cfg->params[0].elm;
                inBuf[inBufIdx++]=chr;
                inBufInpMenuStart = inBufIdx;
                actionParamIdx=0;
                pBuf->idx=0;
                printf("(");
            }
            break;

          case ',':
          case ')':
            if (actionMenu)
            {
                const srvMenuActionCfg_ts * cfg = actionMenu->cfg;
                if (cfg)
                {
                    const srvMenuElement_ts * paramElm = cfg->params[actionParamIdx].elm;
                    if (paramElm)
                    {
                        paramElm->typ->eval(paramElm,&inBuf[inBufInpMenuStart]);
                        inBuf[inBufIdx++]=chr;
                        inBufInpMenuStart = inBufIdx;
                        printf("%c",chr);
                        actionParamIdx++;
                        pBuf->idx=0;
                        inpMenu = cfg->params[actionParamIdx].elm;
                        if (!inpMenu) inpMenu = actionMenu;
                    }
                    else
                    {
                        // No (more) parameters to fill 
                        if (chr==')')
                        {
                            inBuf[inBufIdx++]=chr;
                            printf(")");
                        }
                    }
                }
                else if (chr==')')
                {
                    inBuf[inBufIdx++]=chr;
                    printf(")");
                }
            }
            break;
            
          case '/':
            { // trying to move to sub-menu
                if (inBufProcessedIdx==inBufIdx)
                { // first char of (sub)menu input => reset to main menu
                    inBufIdx=0;
                    inBuf[inBufIdx++] = chr;
                    inBufProcessedIdx=inBufIdx;
                    inBufInpMenuStart = inBufIdx;
                    inBufBrowseIdx=inBufIdx;
                    inpMenu = &elemTopMenu;
                    pBuf->idx=0;
                    printf("/");
                }
                else
                { // enter menu (if recognised)
                    if (selectedMenuItem)
                    {  // switch to (sub)menu
                        inpMenu = selectedMenuItem;
                        inBuf[inBufIdx++] = chr;
                        inBufProcessedIdx = inBufIdx;
                        inBufInpMenuStart = inBufIdx;
                        pBuf->idx=0;
                        printf("/");
                        selectedMenuItem=NULL;
                    }
                }
            }
            break;
            
          default:
            break;
        }
    }
    
    if (SRV_MENU_IS_CHAR_HANDLED(res))
    {
        switch(chr)
        {
          case 127:
            printf(rubOutStr());
            break;
            
          case 8: // left
          case 10: // down
          case 11: // up
          case 12: // right
            break;
          case 27: // esc
            break;
          default:
            printf("%c",chr);
            break;
        }
    }
    else
    {
        switch(chr)
        {
          case '?':
            printf("\r$%s",inBuf);
            break;
        }
    }
    
    switch(chr)
    {
      case 27:
        printf("***ABORT***\n");
        resetLineInput();
        break;
        
      case 8: // left
      case 10: // down
      case 11: // up
      case 12: // right
        break;
    }
    
    return chr | SRV_MENU_CHAR_HANDLED;
}






/******************************************************************************/
/* Local functions                                                         */
/******************************************************************************/


/******************************************************************************/
/* Menu */
/******************************************************************************/
static char sharedMenuStorage[20+sizeof(menuItemData_ts)];


static int inpMenuChr(menuItemData_ts * pMd, int chr, int options)
{
    int stat = chr;
    
    switch (chr & 255)
    {
      case 127:
          if (pMd->idx>0)
          {
              pMd->idx--;
              pMd->str[pMd->idx]=0;
              stat |= SRV_MENU_CHAR_HANDLED;
          }
        break;


      case 8: // left
      case 10: // down
      case 11: // up
      case 12: // right
        //TODO: Infield navigation not implemented yet
        stat |= SRV_MENU_CHAR_HANDLED;
        break;

      case 13:
      case ',':
      case '?':
      case '=':
      case '(':
      case ')':
        stat |= SRV_MENU_CHAR_FINISHED;
        break;

      case '/':
        if (options & INPUT_HANDLE_MENU_SEP_TOKEN)
        {
            stat |= SRV_MENU_CHAR_FINISHED;
            break;
        }
        // NOTE: Fall throuhg to default
      default:
        {
          if (pMd->idx+1 < pMd->max)
          {
              pMd->str[pMd->idx]=chr;
              pMd->idx++;
          }
        }
        stat |= SRV_MENU_CHAR_HANDLED;
        break;
    }
    
    return stat;
}



static int menuStr(const srvMenuElement_ts * elm, char * str, int max)
{
    srvMenuList_ts * pMen = (srvMenuList_ts*) elm->cfg;
    menuItemData_ts * pBuf = (menuItemData_ts*) elm->storage;
    int id = 0;
  
    if (!pBuf)
    {
        pBuf = (menuItemData_ts*)sharedMenuStorage;
    }
    
    id = pBuf->id;
    
    if (id>=0)
    {
        char * pDest = str;
        const char * pSrc = pMen[id].elm->name;
        char * pEnd = str+max;

        for (;(pDest<pEnd)&&(*pDest++=*pSrc++););

        return pDest-str-1;
    }
    return 0;
}


static int menuEval(const srvMenuElement_ts * elm, char * str)
{
    menuItemData_ts * pBuf = (menuItemData_ts*) elm->storage;

    if (!pBuf)
    {
        pBuf = (menuItemData_ts*)sharedMenuStorage;
    }

    pBuf->id = findMenuEntry(elm->cfg, str, NULL);
   
    return pBuf->id;
}




static int menuCharIn(const srvMenuElement_ts * elm, int chr)
{
    int stat=chr;
      
    menuItemData_ts * pBuf = (menuItemData_ts*) elm->storage;

    if (!pBuf)
    {
        pBuf = (menuItemData_ts*)sharedMenuStorage;
    }

    stat = inpMenuChr(pBuf, chr, INPUT_HANDLE_MENU_SEP_TOKEN);

    if (SRV_MENU_IS_CHAR_FINISHED(stat))
    {
        pBuf->str[pBuf->idx] = 0;
        pBuf->id = findMenuEntry(elm->cfg, pBuf->str, NULL);
    }

    return stat;
}





static int8_t findMenuEntry(const srvMenuList_ts * menuStart, char * name, const srvMenuType_ts * typ)
{
    const srvMenuList_ts * menu = menuStart;
    
    if (!menuStart) return -1;
    
    for( ; menu->elm ; menu++ )
    {
        const srvMenuElement_ts * elm = menu->elm;
        if (name)
        {
            if (
                  (strcmp(elm->name, name)!=0)              // not name
                &&((menu->opt&255)!=((name[1]<<8) + name[0])) // not shortkey
               )
            {
                continue;
            }
        }
        if (typ)
        {
            if ( typ != elm->typ )
            {
                continue;
            }
        }
        return menu-menuStart;
      
    }
    return -1;
}





static void printMenuEntries(const srvMenuList_ts * menuEntry)
{
    for( ; menuEntry->elm ; menuEntry++ )
    {
        const srvMenuElement_ts * menuElm = menuEntry->elm;
        char shortKey = menuEntry->opt &255;
        if (shortKey==0) shortKey=' ';
        printf("\n>%c %-9s %-6s %s.", shortKey, menuElm->name, menuElm->typ->name, menuElm->desc);
    }
}



static void printMenuElement(const srvMenuElement_ts * elm)
{
    printf("\n#%s %s (%s)",elm->desc, elm->typ->name, inBuf);
  
    if (elm->typ == &SRV_MENU_TYPE_MENU)
    {
        const srvMenuList_ts * menuEntry = elm->cfg;
        printf("\n");
        
        printMenuEntries(menuEntry);
    }
    else if (elm->typ == &SRV_MENU_TYPE_VALUE)
    {
        valuePrint(elm);
    }
    else if (elm->typ == &SRV_MENU_TYPE_ACTION)
    {
        const srvMenuActionCfg_ts * actionCfg = elm->cfg;
        if (actionCfg && actionCfg->params[0].elm)
        {
            printf(" params:");
            printMenuEntries(actionCfg->params);
        }
        printf("\n");
    }
    else if (elm->typ == &SRV_MENU_TYPE_ENUM)
    {
        enumPrint(elm);
    }
}



/******************************************************************************/
/* Value */
/******************************************************************************/

static int valueStr(const srvMenuElement_ts * elm, char * str, int max)
{
    const srvMenuValueCfg_ts * cfg = elm->cfg;
  
    if (MENU_VALUE_IS_STR(cfg->typ))
    {
        char * pDest = str;
        char * pSrc = elm->storage;
        int len=cfg->typ&127;
        if (max<len) len=max;
        char * pEnd = str+len;

        for (;(pDest<pEnd)&&(*pDest++=*pSrc++););

        return pDest-str-1;
    }
    else if (MENU_VALUE_IS_INT(cfg->typ))
    {
        uint32_t v = ((uint32_t*)elm->storage)[0];
        int len = cfg->typ & 15;
        if (len>4) len=4;
        v = v & (0xffffffff >> 8*(4-len));

        if (cfg->fmt & MENU_VALUE_FMT_HEX)
        {
            snprintf(str,max,"0x%x",v);
        }
        else // default: if (cfg->fmt & MENU_VALUE_FMT_DEC)
        {
            snprintf(str,max,"%d",v);
        }
    }
    else if (MENU_VALUE_IS_FLOAT(cfg->typ))
    {
        int len = cfg->typ & 15;
        if (len>4) len=4;

        if (len==8)
        {
            double * pD = elm->storage;
            snprintf(str, max, "%f", pD[0]);
        }
        else
        {
            float * pF = elm->storage;
            snprintf(str, max, "%f", pF[0]);
        }
    }
    else
    {
        printf("??? %x,%x", cfg->typ, cfg->fmt); 
    }
    
    return 1;
}


static int valueEval(const srvMenuElement_ts * elm, char * str)
{
    const srvMenuValueCfg_ts * cfg = elm->cfg;
    uint32_t res=-1;

    if (cfg->fmt & MENU_VALUE_FMT_WRITABLE)
    {
        if (MENU_VALUE_IS_STR(cfg->typ))
        {
            char * pB=elm->storage;
            
            strncpy(pB, str, cfg->typ &127);
            res=1;
        }
        else if (MENU_VALUE_IS_INT(cfg->typ))
        {
            uint8_t * pB=elm->storage;
            uint32_t v=0;
            int len = cfg->typ&15;
            
            if (len>4) len=4;
            
            sscanf(str,"%i",&v);
            
            for (;len>0;len--)
            {
                *pB = v&255;
                pB++;
                v>>=8;
            }
            res=1;
        }
        else if (MENU_VALUE_IS_FLOAT(cfg->typ))
        {
            int len = cfg->typ&15;
            
            if (len==8)
            {
                double * pD = elm->storage;
                sscanf(str, "%lf", pD);
            }
            else
            {
                float * pF = elm->storage;;
                sscanf(str, "%f", pF);
            }
            
            res=1;
        }
    }
    
    return res;
}



static int valuePrint(const srvMenuElement_ts * elm)
{
    const srvMenuValueCfg_ts * cfg = elm->cfg;
    char buf[48];
  
    if (MENU_VALUE_IS_STR(cfg->typ))
    {
        printf("\n>%s=%s", elm->name, (char*)elm->storage);
    }
    else if (MENU_VALUE_IS_INT(cfg->typ))
    {
        if (cfg->fmt & MENU_VALUE_FMT_LIMITS)
        {
            printf(" [%d-%d]", cfg->min, cfg->max);
        }
        printf("\n>%s=",elm->name);
        elm->typ->str(elm,buf,sizeof(buf));
        printf(buf);
    }
    else if (MENU_VALUE_IS_FLOAT(cfg->typ))
    {
        if (cfg->fmt & MENU_VALUE_FMT_LIMITS)
        {
            printf(" [%f-%f]", *(float*)&cfg->min, *(float*)&cfg->max);
        }
        printf("\n>%s=",elm->name);
        elm->typ->str(elm,buf,sizeof(buf));
        printf(buf);
    }
    else
    {
        printf("\n!Err %x,%x", cfg->typ, cfg->fmt); 
    }

    return 1;
}



/******************************************************************************/
/* Enum */
/******************************************************************************/

static int enumStr(const srvMenuElement_ts * elm, char * str, int max)
{
    const srvMenuEnumCfg_ts* cfg = elm->cfg;
    uint32_t value = *(uint32_t*)elm->storage;
    const srvEnumElement_ts* const* pEV = cfg->enumVals;
    int found = 0;
    uint32_t orredValues = 0;
    int8_t idx=0;
    uint32_t partialMatches=0; 
    const int option = cfg->option;

    if (option & 3)
    {
        while (pEV[idx])
        {
            uint32_t eValue = pEV[idx]->value;
            if (value == eValue)
            {
                strncpy(str, pEV[idx]->name, max);
                found = 1;
                break;
            }
            if (option & 2)
            {
                if ((eValue & ~value)==0)
                { // the bits are a partial part of the needed value
                    orredValues |= eValue;
                    partialMatches |= 1<<idx;
                    if ( orredValues == value )
                    {
                        found=2;
                        break;
                    }
                }
            }
            idx++;
        }
    }
    
    if (found==2)
    {
        int strLen=0;
        for (int i=0; i<sizeof(partialMatches)*8; i++)
        {
            if (partialMatches & (1<<i))
            {
                int added;
                
                if (strLen)
                {
                    str[strLen++] = '|';
                }
                added = snprintf(&str[strLen], max-strLen, "%s", pEV[i]->name);
                strLen += added;
            }
        }
    }
    
    if (!found)
    {
        snprintf(str, max, "0x%x", value);
    }
    
    return 1;
}


static int enumEval(const srvMenuElement_ts * elm, char * str)
{
    const srvEnumElement_ts* const* pEV;
    const srvMenuEnumCfg_ts * cfg = elm->cfg;
    int res=-1;
    uint32_t * pI=elm->storage;
    int sidx;
    int eidx;
    uint32_t value=0;

    if ((str[0]>='0') && (str[0]<='9'))
    {
        sscanf(str, "%i", pI);
        return 1;
    }
    
    eidx=0;
    for (sidx=0; ; sidx++)
    {  // scan through inputstring
        char c=str[sidx];
         
        if ((c==0)||(c=='|')||(c=='+'))
        { // end of enum name
            for ( pEV = cfg->enumVals; pEV[0]; pEV++)
            { // find enum name
                if (0==strncmp(pEV[0]->name, &str[eidx], sidx-eidx))
                { // enum name match: or to value
                    value |= pEV[0]->value;
                    res++;
                    break;
                }
            }
            if (!pEV[0]) break;
           
            eidx=sidx+1;
        }
           
        if (c==0) break;
    }
    
    if (res>=0)
    {
        *pI = value;
        res++;
    }
    
    return res;
}



static int enumPrint(const srvMenuElement_ts * elm)
{
    const srvMenuEnumCfg_ts * cfg = elm->cfg;
    char buf[32];
    const srvEnumElement_ts* const* pEV = cfg->enumVals;
    int idx;
    char sep = (cfg->option & MENU_ENUM_FMT_BITFIELDS)?'+':'/';
    int nw=0;
    char fmts[30];
    
    printf(" [");
    for (idx=0; pEV[idx]; idx++)
    {
        int sl = strlen(pEV[idx]->name);
        if (idx) printf("%c",sep);
        printf("%s",pEV[idx]->name);
        if (nw<sl) nw=sl;
    }
    printf("]");
    
    
    elm->typ->str(elm, buf, sizeof(buf));
    printf("\n>%s=%s", elm->name, buf);
    snprintf(fmts, sizeof(fmts),"\n>%%-%ds : %%%dx : %%s", nw, 6);
    for (idx=0; pEV[idx]; idx++)
    {
        printf(fmts, pEV[idx]->name, pEV[idx]->value, pEV[idx]->desc);
    }

    return 1;
}


/******************************************************************************/
/* Action */
/******************************************************************************/


static int actionStr(const srvMenuElement_ts * elm, char * str, int max)
{
    srvMenuList_ts * pMen = (srvMenuList_ts*) elm->cfg;
    menuItemData_ts * pBuf = (menuItemData_ts*) elm->storage;
    int id = 0;
  
    if (!pBuf)
    {
        pBuf = (menuItemData_ts*)sharedMenuStorage;
    }
    
    id = pBuf->id;
    
    if (id>=0)
    {
        char * pDest = str;
        const char * pSrc = pMen[id].elm->name;
        char * pEnd = str+max;

        for (;(pDest<pEnd)&&(*pDest++=*pSrc++););

        return pDest-str-1;
    }
    return 0;
}


static int actionEval(const srvMenuElement_ts * elm, char * str)
{
    menuItemData_ts * pBuf = (menuItemData_ts*) elm->storage;
    const srvMenuActionCfg_ts * pCfg = elm->cfg;

    if (!pBuf)
    {
        pBuf = (menuItemData_ts*)sharedMenuStorage;
    }

    pBuf->id = findMenuEntry(pCfg->params, str, NULL);
   
    return pBuf->id;
}



/******************************************************************************/

static const srvMenuElement_ts * getSubElement(const srvMenuElement_ts * pMenu, int idx)
{
    if (pMenu->typ == &SRV_MENU_TYPE_MENU)
    {
        const srvMenuList_ts * pMenuList = (srvMenuList_ts *) pMenu->cfg;
        return pMenuList[idx].elm;
    }
    if (pMenu->typ == &SRV_MENU_TYPE_ACTION)
    {
        const srvMenuActionCfg_ts * pCfg = pMenu->cfg;
        return pCfg->params[idx].elm;
    }
    
    return 0;
}  




/******************************************************************************/






static void resetLineInput(void)
{
    browseMenu = &elemTopMenu;
    inpMenu = &elemTopMenu;
    setValueMenu = NULL;
    actionMenu = NULL;
    inBufIdx=0;
    inBufBrowseIdx=0;
    inBufProcessedIdx=0;
    inBufInpMenuStart=0;
    actionParamIdx=0;
}


static char * rubOutStr(void)
{
    if (consoleCanHandleRubout)
    {
        return "\x7F";
    }
    return "\x08\x20\x08";
}



static int action_reboot(const srvMenuElement_ts * elm, int opt)
{
    // HalScsSys_Reboot();
    return 1;
}


typedef enum
{
    keyFilter_Idle      =0,
    keyFilter_Esc       =1,
    keyFilter_SBO_Nr    =2
} keyFilter_States;

#define MAX_SB0_VAL     4

static int keyFilter(int chr)
{
    static keyFilter_States state = keyFilter_Idle;
    static int SB0_NrIdx=0;
    static int SBO_Nr[MAX_SB0_VAL];
  
    if (chr==-1)
    {
        // todo: for macro-key implementation
    }
    else
    {
        switch (state)
        {
          case keyFilter_Idle:
            if (chr==27)  { state=keyFilter_Esc; chr=-1; break; }
            break;
            
          case keyFilter_Esc:
            if (chr==27)  { state=keyFilter_Idle; break; }
            if (chr=='[')  { state=keyFilter_SBO_Nr; SB0_NrIdx=0; SBO_Nr[0]=0; chr=-1; break; }
            break;
            
          case keyFilter_SBO_Nr:
            //printf("%c",chr);
            if ((chr>='0') && (chr<='9')) { SBO_Nr[SB0_NrIdx]=(SBO_Nr[SB0_NrIdx]*10)+(chr-'0'); chr=-1; break; }
            else if ((chr==';')&&(SB0_NrIdx<MAX_SB0_VAL)) {SB0_NrIdx++; SBO_Nr[SB0_NrIdx]=0;chr=-1;break; }
            else if (chr=='~')
            {
                state=keyFilter_Idle;
                chr=-1; break;
            }
            else if ((chr>='A') && (chr<='G'))
            {
                static const int8_t escchr2chr[]={11,12,10,8,-1,-1,-1};
                chr= escchr2chr[chr-'A'];
                state=keyFilter_Idle;
                break;
            }
            else if (chr=='R')
            {
                cursor_X = SBO_Nr[1];
                cursor_Y = SBO_Nr[0];
                cursor_U++;
                state=keyFilter_Idle;
                chr=-1; break;
            }
            else {state=keyFilter_Idle;}
        }
   
    }
    return chr;
}




static void detectTerminal(int cmd)
{
    static int state = 0;
    static int waitCC = 0;
    static int orgCC = 0;
    static int temp = 0;
    int ts = k_uptime_get();
    
    if (cmd==1) state=0;
    
    if (waitCC)
    {
        if ((ts>waitCC)||(orgCC != cursor_U))
        {
            state++;
            waitCC=0;
        }
    }

    switch (state)
    {
      case 0:
        orgCC = cursor_U;
        printf("\33[6nX\x7F\33[6n");
        waitCC = ts+100;
        state++;
        break;
      case 1: break;
      case 2:
        if (orgCC != cursor_U)
        {
            orgCC = cursor_U;
            consoleKnowsEscSeq = 1;
            temp = cursor_X;
            waitCC = ts+100;
            state++;
        }
        else
        {
            state=99;
        }
        break;
      case 3: break;
      case 4:
        if ((orgCC != cursor_U)&&(temp == cursor_X))
        {
            consoleCanHandleRubout = 1;
        }
        temp = cursor_Y;
        printf("\33\x37\33[99;999H\33[6n\33\x38");
        orgCC = cursor_U;
        waitCC = ts+400;
        state++;
        break;
      case 5: break;
      case 6:
        termSize_X = cursor_X;
        termSize_Y = cursor_Y;
        if (temp>=termSize_Y) temp = termSize_Y-1;
        printf("\n\n\33[1;%dr\33\x38\33[%d;1H", termSize_Y-1, temp);
        printf("\33\x37\33[%d;1H\33[7mSkin Conductance Simulator Console\33[0m\33\x38", termSize_Y);
        state++;
        break;
    }
  
  
}


/******************************************************************************/
/* Callback functions                                                         */
/******************************************************************************/

/**
 * \}
 * End of file.
 */

