/*******************************************************************************
*       @brief      This module provides system Timer initialization and
*                   interrupts.
*       @file       Uphole/src/timer.c
*       @date       December 2014
*       @copyright  COPYRIGHT (c) 2014 Target Drilling Inc. All rights are
*                   reserved.  Reproduction in whole or in part is prohibited
*                   without the prior written consent of the copyright holder.
*******************************************************************************/

//============================================================================//
//      INCLUDES                                                              //
//============================================================================//


#include <stm32f4xx.h>
#include <stdbool.h>
#include "Portable.h"
#include "timer.h"

//============================================================================//
//      FUNCTION IMPLEMENTATIONS                                              //
//============================================================================//

/*!
********************************************************************************
*       @details
*******************************************************************************/
/*
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; Function:
;   Delay5us()
;
; Description:
;   Delays 5 uS by busy waiting.
;
; Reentrancy:
;   No
;
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/
void Delay5us(void)
{
    // Empirical testing shows that a count of about 168 yields 5 uS.
    // However, since we want to make sure that we delay at least 5 uS,
    // we're setting the count slightly higher.  175 seems good.
    U_INT32 x = 175;

    while(x--)
    {
        ;
    }
}

void DelayHalfSecond(void)
{
    // Empirical testing shows that a count of about 168 yields 5 uS.
    // However, since we want to make sure that we delay at least 5 uS,
    // we're setting the count slightly higher.  175 seems good.
    U_INT32 x = 17500000;

    while(x--)
    {
        ;
    }
}

void Modem90KHzInit(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;

  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;

  	/* GPIOE Configuration: Channel 2 as alternate function push-pull */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  	GPIO_Init(GPIOE, &GPIO_InitStructure);

  	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_TIM9);

  	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 2;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 619;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;

	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 310;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM9, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM9, ENABLE);

	//Modem90KHzEnable(true); //ZD 7/10/2024 This is the line of code to enable Modem output for 90kHz

}


void SetModemOutput(bool state)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_StructInit(&GPIO_InitStructure);

	// GPIO LED status Pins
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_WriteBit(GPIOE, GPIO_Pin_6, state ? Bit_SET : Bit_RESET);

}

void Modem90KHzEnable(bool enable)
{
	if (enable)
	{
		Modem90KHzInit();
		/* TIM1 counter enable */
		TIM_Cmd(TIM9, ENABLE);
	}
	else
	{
		/* TIM1 counter disable */
		TIM_Cmd(TIM9, DISABLE);

		SetModemOutput(true);

	}
}

