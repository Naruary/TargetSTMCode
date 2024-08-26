/*******************************************************************************
*       @brief      This header file contains callable functions and public
*                   data for the Job tab on the Uphole box.
*       @file       Uphole/inc/UI_Tabs/UI_JobTab.h
*       @date       July 2014
*       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

#ifndef UI_JOB_TAB_H
#define UI_JOB_TAB_H

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//

#include "portable.h"
#include "UI_ScreenUtilities.h"

//============================================================================//
//      DATA DEFINITIONS                                                      //
//============================================================================//

extern const TAB_ENTRY JobTab;

//============================================================================//
//      FUNCTION PROTOTYPES                                                   //
//============================================================================//

#ifdef __cplusplus
extern "C" {
#endif

    float GetDesiredAzimuth(void);
    void SetDesiredAzimuth(float value);
    void SetDefaultPipeLength(float length);
    float GetDefaultPipeLength(void);
    void Set90DegErr(float length);
    float Get90DegErr(void);
    void Set270DegErr(float length);
    float Get270DegErr(void);
    void SetMaxErr(float length);
    float GetMaxErr(void);
    void SetDeclination(float value);
    float GetDeclination(void);
    void SetBoreholeName(char* value);
    char* GetBoreholeName(void);

#ifdef __cplusplus
}
#endif

#endif // UI_JOB_TAB_H
