/*******************************************************************************
*       @brief      This file contains the implementation for the
*                   Downhole main panel
*       @file       Uphole/src/UI_Panels/UI_DownholeMainPanel.c
*       @date       June 2019
*       @copyright  COPYRIGHT (c) 2019 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SysTick.h"
#include "DownholeBatteryAndLife.h"
#include "GammaSensor.h"
#include "TextStrings.h"
#include "TargetProtocol.h"
#include "UI_ScreenUtilities.h"
#include "UI_Frame.h"
#include "UI_FixedField.h"
#include "UI_BooleanField.h"
#include "UI_api.h"
#include "UI_Alphabet.h"
#include "UI_MainTab.h"
#include "UI_DownholeMainPanel.h"
#include "version.h"
#include "LoggingManager.h"

//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

static MENU_ITEM* GetMenu(U_BYTE index);
static void Paint(TAB_ENTRY* tab);
static void Show(TAB_ENTRY* tab);
static void TimerElapsed(TAB_ENTRY* tab);
static void ShowDownholeVoltageTabDiag(char* message1, int rowbit);
static void ShowDownholeVoltageTabDiag2(char* message1, int rowbit);
//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//
static MENU_ITEM DownholeMenu[] =
{
	CREATE_BOOLEAN_FIELD(TXT_GAMMA_ON_OFF,			&LabelFrame1, &ValueFrame1,
		CurrrentLabelFrame,     GetGammaPoweredState,	TargProtocol_RequestSendGammaEnable),
};

//============================================================================//
//      CONSTANTS                                                             //
//============================================================================//
#define MENU_SIZE (sizeof(DownholeMenu) / sizeof(MENU_ITEM))

PANEL DownholePanel = {
	GetMenu,
	MENU_SIZE,
	Paint,
	Show,
	0,
	TimerElapsed
};

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*******************************************************************************
 *       @details
 *******************************************************************************/
static MENU_ITEM* GetMenu(U_BYTE index)
{
	if (index >= MENU_SIZE)
		return NULL ;
	return &DownholeMenu[index];
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void Paint(TAB_ENTRY * tab)
{
	char text[100];
	INT16 awakeTime;

	TabWindowPaint(tab);
	U_BYTE nMenuCount = tab->MenuSize(tab);

	snprintf(text, 100, "Downhole Voltage:         %.2f", (double) GetDownholeBatteryVoltage() / 1000);
	ShowDownholeVoltageTabDiag(text, ((nMenuCount + 0) * 15) + 4);

	snprintf(text, 100, "Downhole Batt 2 Voltage:         %.2f", (double) GetDownholeBattery2Voltage() / 1000);
	ShowDownholeVoltageTabDiag2(text, ((nMenuCount + 1) * 15) + 4);

	snprintf(text, 100, "Downhole Signal Strength:        %d", GetDownholeSignalStrength());
	ShowDownholeVoltageTabDiag(text, ((nMenuCount + 2) * 15) + 4);

	snprintf(text, 100, "Dwn Software Version:     %s  %s", GetDownholeSWVersion(), GetDownholeSWDate());
	ShowDownholeVoltageTabDiag(text, ((nMenuCount + 3) * 15) + 4);

	snprintf(text, 100, "Uph Software Version:     %s", GetSWVersion());
	ShowDownholeVoltageTabDiag(text, ((nMenuCount + 4) * 15) + 4);

	if (LoggingManager_IsConnected()) // whs 10Dec2021 yitran modem is connected to Downhole
	{
		awakeTime = GetAwakeTimeLeft();
		if (awakeTime < 0)
			awakeTime = 0;
		snprintf(text, 100, "Down on press Survey - Sleeps in > %d", awakeTime);
		ShowStatusMessage(text);
	}
	else
	{
		ShowStatusMessage("Downhole Disconnected or Dead");
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void Show(TAB_ENTRY * tab)
{
	tab = tab;
	UI_SetActiveFrame(&LabelFrame1);
	SetActiveLabelFrame(LABEL1);
	PaintNow(&HomeFrame);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void TimerElapsed(TAB_ENTRY * tab)
{
	tab = tab;
	MENU_ITEM *time = &DownholeMenu[0];
	if (time == NULL)
		return;
	{
		RepaintNow(time->valueFrame);
		RepaintNow(&HomeFrame);
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void ShowDownholeVoltageTabDiag(char * message1, int rowbit)
{
	RECT area;
	const FRAME *frame = &WindowFrame;
	area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 2;
	area.ptTopLeft.nRow = frame->area.ptTopLeft.nRow + rowbit;
	area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 5;
	area.ptBottomRight.nRow = area.ptTopLeft.nRow + 15;
	UI_DisplayStringLeftJustified(message1, &area);
}
static void ShowDownholeVoltageTabDiag2(char * message1, int rowbit)
{
	RECT area;
	const FRAME *frame = &WindowFrame;
	area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 3;
	area.ptTopLeft.nRow = frame->area.ptTopLeft.nRow + rowbit;
	area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 6;
	area.ptBottomRight.nRow = area.ptTopLeft.nRow + 16;
	UI_DisplayStringLeftJustified(message1, &area);
}
