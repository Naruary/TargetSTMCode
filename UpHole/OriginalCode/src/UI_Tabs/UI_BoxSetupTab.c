/*******************************************************************************
*       @brief      This file contains the implementation for the Setup
*                   tab on the Uphole LCD screen.
*       @file       Uphole/src/UI_Tabs/UI_BoxSetupTab.c
*       @date       July 2014
*       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adc.h"
#include "ModemDriver.h"
#include "RecordManager.h"
#include "FlashMemory.h"
#include "TextStrings.h"
#include "UI_ScreenUtilities.h"
#include "UI_Frame.h"
#include "UI_api.h"
#include "UI_TimeField.h"
#include "UI_DateField.h"
#include "UI_BooleanField.h"
#include "UI_YesNoField.h"
#include "UI_FixedField.h"
#include "UI_ListField.h"
#include "UI_Alphabet.h"
#include "UI_BoxSetupTab.h"
#include "UI_DownholeTab.h"
#include "keypad.h"
//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

static void GetTime(RTC_TimeTypeDef* time);
static void SetTime(RTC_TimeTypeDef* time);
static void GetDate(RTC_DateTypeDef* date);
static void SetDate(RTC_DateTypeDef* date);
static MENU_ITEM* GetBoxSetupMenuItem(TAB_ENTRY* tab, U_BYTE index);
static U_BYTE GetBoxSetupMenuSize(TAB_ENTRY* tab);
static void BoxSetupTabRefresh(TAB_ENTRY* tab);
static void BoxSetupTabShow(TAB_ENTRY* tab);
static void BoxSetupTabPaint(TAB_ENTRY* tab);
void ShowUpholeVoltageTabDiag(char* message1, int rowbit);

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

const TAB_ENTRY BoxSetupTab = { &TabFrame4, TXT_SETUP, ShowTab, GetBoxSetupMenuItem, GetBoxSetupMenuSize, BoxSetupTabPaint, BoxSetupTabShow, BoxSetupTabRefresh };

//============================================================================//
//      CONSTANTS                                                             //
//============================================================================//

//#define NUM_LANGUAGES (sizeof(languages)/sizeof(LIST_ITEM))

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

static MENU_ITEM menu[] =
{
    CREATE_TIME_FIELD(TXT_SETUP_SET_TIME,            &LabelFrame1, &ValueFrame1,
		CurrrentLabelFrame, GetTime, SetTime),
    CREATE_DATE_FIELD(TXT_SETUP_SET_DATE,            &LabelFrame2, &ValueFrame2,
		CurrrentLabelFrame, GetDate, SetDate),
    CREATE_BOOLEAN_FIELD(TXT_SETUP_CHANGE_BUZZER,    &LabelFrame3, &ValueFrame3,
		CurrrentLabelFrame, GetKeyBeeperAvailable, SetKeyBeeperAvailable),
	CREATE_FIXED_FIELD(TXT_LCD_BACKLT_OFF_TIME,		 &LabelFrame4, &ValueFrame4,
		CurrrentLabelFrame,	GetBacklightOnTime, SetBacklightOnTime, 4, 0, 0, 9999),
	CREATE_FIXED_FIELD(TXT_LCD_OFF_TIME,		 	&LabelFrame5, &ValueFrame5,
		CurrrentLabelFrame,	GetLCDOnTime, SetLCDOnTime, 4, 0, 0, 9999),
        CREATE_FIXED_FIELD(TXT_SETUP_DEBOUNCE_TIME, &LabelFrame6, &ValueFrame6, CurrrentLabelFrame, GetDebounceTime, SetDebounceTime, 2, 0, 1, 20),
};

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*******************************************************************************
 *       @details
 *******************************************************************************/
static MENU_ITEM* GetBoxSetupMenuItem(TAB_ENTRY * tab, U_BYTE index)
{
	if (index < tab->MenuSize(tab))
	{
		return &menu[index];
	}
	return NULL ;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static U_BYTE GetBoxSetupMenuSize(TAB_ENTRY * tab)
{
	tab = tab;

	return sizeof(menu) / sizeof(MENU_ITEM);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void GetTime(RTC_TimeTypeDef * time)
{
	RTC_GetTime(RTC_Format_BIN, time);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void SetTime(RTC_TimeTypeDef * time)
{
	RTC_SetTime(RTC_Format_BIN, time);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void GetDate(RTC_DateTypeDef * date)
{
	RTC_GetDate(RTC_Format_BIN, date);

	if (date->RTC_Date == 0 || date->RTC_Month == 0 || date->RTC_WeekDay == 0 || date->RTC_Year == 0)
	{
		date->RTC_Year = 1;
		date->RTC_Month = 1;
		date->RTC_Date = 1;
		date->RTC_WeekDay = 1;
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void SetDate(RTC_DateTypeDef * date)
{
	if (date->RTC_Date == 0 || date->RTC_Month == 0 || date->RTC_WeekDay == 0 || date->RTC_Year == 0)
	{
		date->RTC_Year = 1;
		date->RTC_Month = 1;
		date->RTC_Date = 1;
		date->RTC_WeekDay = 1;
	}
	RTC_SetDate(RTC_Format_BIN, date);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void BoxSetupTabRefresh(TAB_ENTRY * tab)
{
	tab = tab;

	MENU_ITEM *time = &menu[0];
	if ((!time->editing) && (UI_GetActiveFrame()->eID != ALERT_FRAME))
	{
		RepaintNow(time->valueFrame);
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void BoxSetupTabShow(TAB_ENTRY * tab)
{
	tab = tab;

	UI_SetActiveFrame(&LabelFrame1);
	SetActiveLabelFrame(LABEL1);
	PaintNow(&HomeFrame);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static void BoxSetupTabPaint(TAB_ENTRY * tab)
{
	char text[100];
	TabWindowPaint(tab);
	U_BYTE nMenuCount = tab->MenuSize(tab);
	snprintf(text, 100, "Saved Boreholes:           %d", CurrentBoreholeNumber());
	ShowUpholeVoltageTabDiag(text, (nMenuCount * 15) + 4); //Used to be + 4
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void ShowUpholeVoltageTabDiag(char * message1, int rowbit)
{
	RECT area;
	const FRAME *frame = &WindowFrame;
	area.ptTopLeft.nCol = frame->area.ptTopLeft.nCol + 2;
	area.ptTopLeft.nRow = frame->area.ptTopLeft.nRow + rowbit;
	area.ptBottomRight.nCol = frame->area.ptBottomRight.nCol - 5;
	area.ptBottomRight.nRow = area.ptTopLeft.nRow + 15;
	UI_DisplayStringLeftJustified(message1, &area);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void SetKeyBeeperAvailable(BOOL bState)
{
	NVRAM_data.fKeyBeeperEnable = bState;
} // SetKeyBeeperAvailable

/*******************************************************************************
 *       @details
 *******************************************************************************/
BOOL GetKeyBeeperAvailable(void)
{
	return NVRAM_data.fKeyBeeperEnable;
} // GetKeyBeeperAvailable

/*******************************************************************************
 *       @details
 *******************************************************************************/
void SetBacklightOnTime(INT16 OnTime)
{
	NVRAM_data.nBacklightOnTime_sec = OnTime;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
INT16 GetBacklightOnTime(void)
{
	return NVRAM_data.nBacklightOnTime_sec;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void SetLCDOnTime(INT16 OnTime)
{
	NVRAM_data.nLCDOnTime_sec = OnTime;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
INT16 GetLCDOnTime(void)
{
	return NVRAM_data.nLCDOnTime_sec;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void SetErrorCorrect(BOOL OnTime)
{
	NVRAM_data.fEnableErrorCorrectAzimuth = OnTime;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
BOOL GetErrCorrect(void)
{
	return (BOOL) NVRAM_data.fEnableErrorCorrectAzimuth;
}
