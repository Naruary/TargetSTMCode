/*******************************************************************************
*       @brief      This file contains the implementation for the Clear All Hole Data
*                   Decision Panel on the Main tab. That gives a option to select Yes/No
*       @file       Uphole/inc/UI_Panels/UI_SartNewHoleDecisionPanel.h
*       @date       November 2015
*       @copyright  COPYRIGHT (c) 2015 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "LoggingManager.h"
#include "RecordManager.h"
#include "FlashMemory.h"
#include "UI_Alphabet.h"
#include "UI_ScreenUtilities.h"
#include "UI_Frame.h"
#include "UI_LCDScreenInversion.h"
#include "UI_api.h"
#include "UI_ClearAllHoleDecisionPanel.h"
#include "UI_RecordDataPanel.h"
#include "UI_DataTab.h"
#include "UI_GroupBox.h"
#include "SysTick.h"

//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

static void ClearAllHoleData_Decision_Paint(TAB_ENTRY* tab);
static void ClearAllHoleData_Decision_Show(TAB_ENTRY* tab);
static void ClearAllHoleData_Decision_KeyPressed(TAB_ENTRY* tab, BUTTON_VALUE key);
static void ClearAllHoleData_Decision_TimerElapsed(TAB_ENTRY* tab);
static MENU_ITEM* ClearAllHoleData_Decision_Menu(U_BYTE index);
static void Yes_ClearAllHoleData_Decision(MENU_ITEM* item);
static void No_ClearAllHoleData_Decision(MENU_ITEM* item);
void ShowClearHoleDecisionMessage(char* message);
void ShowClearHoleInfoMessage(char* message1, char* message2, char* message3);

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

static MENU_ITEM ClearAllHoleData_DecisionMenu[] =
{
    CREATE_MENU_ITEM(TXT_YES,    &LabelFrame1, Yes_ClearAllHoleData_Decision),
    CREATE_MENU_ITEM(TXT_NO,     &LabelFrame2, No_ClearAllHoleData_Decision)
};

BOOL ClearAllHoleData_DecisionPanelActive = false;

static TIME_LR tCallClearAllHole = 0;

PANEL ClearAllHoleData_DecisionPanel = {ClearAllHoleData_Decision_Menu, sizeof(ClearAllHoleData_DecisionMenu) / sizeof(MENU_ITEM), ClearAllHoleData_Decision_Paint, ClearAllHoleData_Decision_Show, ClearAllHoleData_Decision_KeyPressed, ClearAllHoleData_Decision_TimerElapsed};


//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*!
********************************************************************************
*       @details
*******************************************************************************/

void setClearAllHoleDataDecisionPanelActive(BOOL bFlag)
{
    ClearAllHoleData_DecisionPanelActive = bFlag;
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

BOOL getClearAllHoleDataDecisionPanelActive(void)
{
    return ClearAllHoleData_DecisionPanelActive;
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void ClearAllHoleData_Decision_Paint(TAB_ENTRY* tab)
{
    ClearAllHoleData_Decision_Show(tab);
    TabWindowPaint(tab);
    //GroupBoxPaint(&surveyGroup);
    ShowClearHoleDecisionMessage("Are you sure. Clear All Boreholes?");
    ShowClearHoleInfoMessage("ERASE data from Memory PERMANENTLY.", "Remember to SET correct JOB","values before taking any survey.");
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void ClearAllHoleData_Decision_Show(TAB_ENTRY* tab)
{
    MENU_ITEM* item = tab->MenuItem(tab, 0);
    UI_SetActiveFrame(item->labelFrame);
    SetActiveLabelFrame(item->labelFrame->eID);
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void ClearAllHoleData_Decision_KeyPressed(TAB_ENTRY* tab, BUTTON_VALUE key)
{
    switch(key)
    {
		default:
			break;

        case BUTTON_DASH:
        {
            setClearAllHoleDataDecisionPanelActive(false);
            RepaintNow(&WindowFrame);
            break;
        }
    }
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void ClearAllHoleData_Decision_TimerElapsed(TAB_ENTRY* tab)
{
    //RepaintNow(&WindowFrame);
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static MENU_ITEM* ClearAllHoleData_Decision_Menu(U_BYTE index)
{
    return &ClearAllHoleData_DecisionMenu[index];
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void Yes_ClearAllHoleData_Decision(MENU_ITEM* item)
{

    // put your logic here
    setClearAllHoleDataDecisionPanelActive(false);
    LoggingManager_StartLogging();
    SetLoggingState(CLEAR_ALL_HOLE);
    tCallClearAllHole = ElapsedTimeLowRes(0);
    RepaintNow(&WindowFrame);
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

static void No_ClearAllHoleData_Decision(MENU_ITEM* item)
{
    // put your logic here
    //RECORD_InitBranchParam();
    setClearAllHoleDataDecisionPanelActive(false);
    SetLoggingState(LOGGING);
    RepaintNow(&WindowFrame);
    SetActiveLabelFrame(LABEL4);
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

TIME_LR GetClearAllHoleTimer(void)
{
  return tCallClearAllHole;
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

void ShowClearHoleDecisionMessage(char* message)
{
    RECT area;
    const FRAME* frame = &WindowFrame;
    area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 5;
    area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 70;
    area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 5;
    area.ptBottomRight.nRow = area.ptTopLeft.nRow + 15;
    UI_DisplayStringCentered(message, &area);
}

/*!
********************************************************************************
*       @details
*******************************************************************************/

void ShowClearHoleInfoMessage(char* message1, char* message2, char* message3)
{
    RECT area;
    const FRAME* frame = &WindowFrame;
    area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 5;
    area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 140;
    area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 5;
    area.ptBottomRight.nRow = area.ptTopLeft.nRow + 15;
    UI_DisplayStringCentered(message1, &area);
    area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 100;
    UI_DisplayStringCentered(message2, &area);
    area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 60;
    UI_DisplayStringCentered(message3, &area);
}
