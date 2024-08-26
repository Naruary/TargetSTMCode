/*******************************************************************************
 *       @brief      Source file for UI_FixedField.c.
 *       @file       Uphole/src/UI_DataFields/UI_FixedField.c
 *       @date       December 2014
 *       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
 *                   reserved.  Reproduction in whole or in part is prohibited
 *                   without the prior written consent of the copyright holder.
 *******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "UI_FixedPointValue.h"
#include "UI_FixedField.h"
#include "UI_Alphabet.h"
#include "UI_LCDScreenInversion.h"
#include "UI_Frame.h"
#include "UI_api.h"

#define NUM_EDIT_DIGITS 10
static char digits[NUM_EDIT_DIGITS];
static int signHolder = 0;
static int decimalSelected = 0;
static int digitsEntered = 0;
#define NUM_BEFORE_CHARS 50
static char before[NUM_BEFORE_CHARS];
static REAL32 edit_value;

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*******************************************************************************
 *       @details
 *******************************************************************************/
void FixedDisplay(MENU_ITEM * item)
{
	char astring[NUM_EDIT_DIGITS];
	int loopy;
	int places;
	FRAME *frame;

	if (item->editing)
	{
		item->highlight = 0;
		item->HighlightValue = 0;
		places = item->fixed.numberDigits + item->fixed.fractionDigits + 2;
		for (loopy = 0; loopy < places; loopy++)
		{
			astring[loopy] = '_';
		}
		astring[loopy] = 0;
		if (item->fixed.position > 0)
		{
			int from = item->fixed.position - 1;
			int too = item->fixed.numberDigits;
			for (loopy = 0; loopy < item->fixed.position; loopy++)
			{
				astring[too--] = digits[from--];
			}
			if (signHolder)
			{
				astring[too] = '-';
			}
		}
		UI_ClearLCDArea(&item->valueFrame->area, LCD_FOREGROUND_PAGE);
		UI_DisplayStringLeftJustified(astring, (RECT*) &item->valueFrame->area);
		LCD_Refresh(LCD_FOREGROUND_PAGE);
	}
	else
	{
		frame = (FRAME*) item->valueFrame;
		UI_ClearLCDArea(&frame->area, LCD_FOREGROUND_PAGE);
		if (!item->editing)
		{
			item->fixed.value = item->fixed.GetValue();
		}
		UI_DisplayStringLeftJustified(FixedValueFormat(&item->fixed), &frame->area);
	}
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void FixedBeginEdit(MENU_ITEM * item)
{
	item->editing = true;
	item->fixed.position = 0;
	digits[0] = '0';
	digits[1] = 0;
	signHolder = 0;
	decimalSelected = 0;
	digitsEntered = 0;
	FixedDisplay(item);

}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static char* BeginString(char * string, int endIndex)
{
	if (endIndex >= NUM_BEFORE_CHARS)
		endIndex = NUM_BEFORE_CHARS - 1;
	before[endIndex] = 0;
	if (endIndex > 0)
	{
		strncpy(before, string, endIndex);
	}
	return before;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
static RECT* GetCharacterRect(MENU_ITEM * item, U_BYTE index)
{
	static RECT rect;
	RECT *frame;

	frame = (RECT*) &item->valueFrame->area;
	U_INT16 offset = UI_GetTextSize(BeginString(FixedValueFormat(&item->fixed), index));
	U_INT16 xSize = UI_GetTextSize(BeginString(FixedValueFormat(&item->fixed), index + 1));
	rect.ptTopLeft.nCol = frame->ptTopLeft.nCol + offset;
	rect.ptTopLeft.nRow = frame->ptTopLeft.nRow + 1;
	rect.ptBottomRight.nCol = frame->ptTopLeft.nCol + xSize + 2;
	rect.ptBottomRight.nRow = frame->ptBottomRight.nRow - 2;
	return &rect;
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void FixedHighlight(MENU_ITEM * item)
{
	int position = item->fixed.position + 1;
	if (position >= (item->fixed.numberDigits - item->fixed.fractionDigits))
	{
		position++;
	}
	if (item->fixed.value < 0)
	{
		position++;
	}
	UI_InvertLCDArea(GetCharacterRect(item, position), LCD_FOREGROUND_PAGE);
}

/*******************************************************************************
 *       @details
 *******************************************************************************/
void FixedFinishEdit(MENU_ITEM * item)
{
    const REAL32 convert[5] =
    { 1.0, 10.0, 100.0, 1000.0, 10000.0 };

    // If no digits were entered, keep the previous value
    if (digitsEntered == 0)
    {
        goto FixedFinishEdit1;
    }

    // Convert digits entered to a floating-point value
    edit_value = atof(digits);

    // Handle negative values if the sign holder is set
    if (signHolder)
    {
        edit_value = -edit_value;
    }

    // Adjust based on the fraction digits specified (for example, handling 2 decimal places)
    edit_value *= convert[item->fixed.fractionDigits];

    // Ensure the value stays within the specified range
    if (edit_value > item->fixed.maxValue)
    {
        edit_value = item->fixed.maxValue;
    }
    else if (edit_value < item->fixed.minValue)
    {
        edit_value = item->fixed.minValue;
    }

    // Store the value using the SetValue callback, scaling it back down for display
    item->fixed.SetValue(edit_value / convert[item->fixed.fractionDigits]);

FixedFinishEdit1:
    item->editing = false;
    item->NextFrame(item);
}


/*******************************************************************************
 *       @details
 *******************************************************************************/
void FixedKeyPressed(MENU_ITEM * item, BUTTON_VALUE keyPressed)
{
	switch (keyPressed)
	{
		case BUTTON_DOWN:
			break;
		case BUTTON_UP:
			break;
		case BUTTON_RIGHT:
			break;
		case BUTTON_LEFT:
			if (item->fixed.position)
			{
				item->fixed.position--;
			}
			digits[item->fixed.position] = 0;
			FixedDisplay(item);
			break;
		case BUTTON_PERIOD:
			// if no digits entered, do not allow decimal
			if (digitsEntered == 0)
				break;
			// if no fraction allowed anyhow, do not allow
			if (item->fixed.fractionDigits == 0)
				break;
			// only one decimal is allowed in the string
			if (decimalSelected)
				break;
			decimalSelected = 1;
			digits[item->fixed.position] = '.';
			if (item->fixed.position < (item->fixed.numberDigits + item->fixed.fractionDigits))
			{
				item->fixed.position++;
				digits[item->fixed.position] = 0;
			}
			FixedDisplay(item);
			break;
		case BUTTON_DASH:
			// if no digits entered, do not allow decimal
//			if(digitsEntered == 0) break;
			if (item->fixed.minValue < 0)
			{
				if (signHolder)
				{
					signHolder = 0;
				}
				else
				{
					signHolder = 1;
				}
				FixedDisplay(item);
			}
			break;
			// numeric keys 0-9
		default:
			if (digitsEntered >= item->fixed.numberDigits)
				break;
			digitsEntered++;
			digits[item->fixed.position] = keyPressed;
			digits[item->fixed.position + 1] = 0;
			if (item->fixed.position < (item->fixed.numberDigits + item->fixed.fractionDigits))
			{
				item->fixed.position++;
				digits[item->fixed.position] = 0;
			}
			FixedDisplay(item);
			break;
	}
}
