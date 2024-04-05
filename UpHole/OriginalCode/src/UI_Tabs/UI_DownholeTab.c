/*******************************************************************************
*       @brief      This file contains the implementation for the Diag
*                   tab on the Uphole LCD screen.
*       @file       Uphole/src/UI_Tabs/UI_DownholeTab.c
*       @date       January 2016
*       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//

#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#include "Manager_DataLink.h"
#include "TextStrings.h"
#include "UI_Alphabet.h"
#include "UI_ScreenUtilities.h"
#include "UI_LCDScreenInversion.h"
#include "UI_Frame.h"
#include "UI_api.h"
#include "UI_BooleanField.h"
#include "UI_YesNoField.h"
#include "UI_FixedField.h"
#include "UI_StringField.h"
#include "FlashMemory.h"
#include "RecordManager.h"
#include "UI_UpdateDiagnosticDownholeDecisionPanel.h"
#include "UI_DownholeTab.h"
#include "UI_DownholeMainPanel.h"
#include "DownholeBatteryAndLife.h"
#include "TargetProtocol.h"
#include "GammaSensor.h"
#include "version.h"

//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

static PANEL *CurrentState(void);
static MENU_ITEM *GetMenuItem(TAB_ENTRY* tab, U_BYTE index);
static U_BYTE GetMenuSize(TAB_ENTRY* tab);
static void TablePaint(TAB_ENTRY* tab);
static void TableShow(TAB_ENTRY* tab);
static void TableTimeElapsed(TAB_ENTRY* tab);
static void TableKeyPressed(TAB_ENTRY* tab, BUTTON_VALUE key);
void ShowStatusMessageTabDiag(char* message1, char* message2);

//============================================================================//
//      DATA DECLARATIONS                                                     //
//============================================================================//

enum { UI_DOWNHOLE_MAIN, };

U_INT16 panelIndex = UI_DOWNHOLE_MAIN;

const TAB_ENTRY DownholeTab = {
	&TabFrame5,
	TXT_DIAG,
	ShowTab,
	GetMenuItem,
	GetMenuSize,
	TablePaint,
	TableShow,
	TableTimeElapsed,
	TableKeyPressed
};



//============================================================================//
//      CONSTANTS                                                             //
//============================================================================//
// will move to panel
#define MENU_SIZE (sizeof(menu) / sizeof(MENU_ITEM))

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*******************************************************************************
 *       @details
 *******************************************************************************/
static PANEL* CurrentState(void)
{
	switch (panelIndex)
	{
		case UI_DOWNHOLE_MAIN:
			return &DownholePanel;
			break;
	}
	return NULL;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static MENU_ITEM* GetMenuItem(TAB_ENTRY * tab, U_BYTE index)
{
	PANEL *apanel;
	if (index < tab->MenuSize(tab))
	{
		apanel = CurrentState();
		if (apanel == NULL)
			return NULL;
		return CurrentState()->MenuItem(index);
	}
	return NULL;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static U_BYTE GetMenuSize(TAB_ENTRY * tab)
{
	tab = tab;

	return CurrentState()->MenuCount;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void TablePaint(TAB_ENTRY * tab)
{
	CurrentState()->Paint(tab);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void TableShow(TAB_ENTRY * tab)
{
	if (CurrentState()->Show)
	{
		CurrentState()->Show(tab);
	}
	PaintNow(&HomeFrame);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void TableTimeElapsed(TAB_ENTRY * tab)
{
	if (CurrentState()->TimerElapsed)
	{
		CurrentState()->TimerElapsed(tab);
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void TableKeyPressed(TAB_ENTRY * tab, BUTTON_VALUE key)
{
	if (CurrentState()->KeyPressed)
	{
		CurrentState()->KeyPressed(tab, key);
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void ShowStatusMessageTabDiag(char * message1, char * message2)
{
	RECT area;
	const FRAME *frame = &WindowFrame;
	area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 5;
	area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 80;
	area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 5;
	area.ptBottomRight.nRow = area.ptTopLeft.nRow + 15;
	UI_DisplayStringCentered(message1, &area);
	area.ptTopLeft.nRow = frame->area.ptBottomRight.nRow - 50;
	UI_DisplayStringCentered(message2, &area);
}


