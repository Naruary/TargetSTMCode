/*******************************************************************************
 *       @brief      Implementation file for the UI TAB Frames section on the
 *                   screen from Plots
 *       @file       Uphole/src/Graph_Plot/Tab_Graph_Frame.c
 *       @date       Feb 2016
 *       @copyright  COPYRIGHT (c) 2016 Target Drilling Inc. All rights are
 *                   reserved.  Reproduction in whole or in part is prohibited
 *                   without the prior written consent of the copyright holder.
 *******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//
#include <stdio.h>
#include <stdlib.h>
#include "lcd.h"
#include "adc.h"
#include "Manager_DataLink.h"
#include "TextStrings.h"
#include "UI_Alphabet.h"
#include "UI_ScreenUtilities.h"
#include "UI_LCDScreenInversion.h"
#include "UI_Frame.h"
#include "UI_api.h"
#include "UI_BooleanField.h"
#include "UI_FixedField.h"
#include "UI_StringField.h"
#include "RecordManager.h"
#include "Graph_Plot.h"
#include "UI_Alphabet.h"
#include "Plan_Graph_Plot.h"
#include "buzzer.h"
#include "UI_MainTab.h"

//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

static MENU_ITEM* GetPlanMenuItem(TAB_ENTRY * tab, U_BYTE index);
static U_BYTE GetPlanMenuSize(TAB_ENTRY * tab);
static void PlanTabPaint(TAB_ENTRY * tab);
static void PlanTabMakeRequest(TAB_ENTRY * tab);
static void PlanTabShow(TAB_ENTRY * tab);

//============================================================================//
//      DATA DECLARATIONS                                                     //
//============================================================================//

const TAB_ENTRY PlanTab =
{ &TabFrame6, TXT_PLAN, ShowTab, GetPlanMenuItem, GetPlanMenuSize, PlanTabPaint, PlanTabShow, PlanTabMakeRequest };
//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

static MENU_ITEM* GetPlanMenuItem(TAB_ENTRY * tab, U_BYTE index)
{
	tab = tab;
	index = index;
	return NULL;
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

static U_BYTE GetPlanMenuSize(TAB_ENTRY * tab)
{
	tab = tab;
	return 0;
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

static void PlanTabPaint(TAB_ENTRY * tab)
{
	TabWindowPaint(tab);
	DrawPlanGraph();
}

/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

static void PlanTabMakeRequest(TAB_ENTRY * tab)
{
	tab = tab;
}
/*!
 ********************************************************************************
 *       @details
 *******************************************************************************/

static void PlanTabShow(TAB_ENTRY * tab)
{
	tab = tab;
	PaintNow(&HomeFrame);
}

