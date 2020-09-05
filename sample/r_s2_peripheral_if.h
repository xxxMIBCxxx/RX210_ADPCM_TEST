/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name	   : r_s2_peripheral_if.h
* Version      : 1.0
* Description  : Peripheral module function for M3S-S2-Tiny
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 20.09.2012 1.00     First Release
***********************************************************************************************************************/
#ifndef	__R_S2_PERIPHERAL_IF_H__
#define	__R_S2_PERIPHERAL_IF_H__

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_stdint.h"
#include "platform.h"
#include "r_adpcm.h"
#include "r_s2_driver.h"

/******************************************************************************
Macro definitions
******************************************************************************/
#if (SOUND_OUTPUT_MODULE == SOUND_DA)
#define SOUND_MODULE_INIT		decode_da_converter_init
#define SOUND_MODULE_START		decode_da_converter_start
#define SOUND_MODULE_STOP		decode_da_converter_stop
#define SOUND_MODULE_SLEEP		decode_da_converter_sleep
#define SOUND_OUTPUT_DATA_SFR	DA.DADR0
#define SOUND_OUTPUT_DATA_CNV	R_Convert16to10

#elif (SOUND_OUTPUT_MODULE == SOUND_PWM)
#define SOUND_MODULE_INIT		decode_pwm_init
#define SOUND_MODULE_START		decode_pwm_start
#define SOUND_MODULE_STOP		decode_pwm_stop
#define SOUND_MODULE_SLEEP		decode_pwm_sleep
#define SOUND_OUTPUT_DATA_SFR	MTU4.TGRC
#define SOUND_OUTPUT_DATA_CNV	R_Convert16toPWM

#else
#error	"Please define "SOUND_DA" or "SOUND_PWM" to "SOUND_OUTPUT_MODULE" macro. "
#endif

#define SOUND_INPUT_DATA_SFR	S12AD.ADDR5


/* Sapling rate table macros */
#define TIMER_FREQ_08000		0
#define TIMER_FREQ_11025		1
#define TIMER_FREQ_16000		2
#define TIMER_FREQ_22050		3
#define TIMER_FREQ_32000		4
#define TIMER_FREQ_44100		5
#define TIMER_FREQ_LIST_MAX		6

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
/* Encode function                               */
void encode_ad_converter_init(void);
void encode_ad_converter_start(void);
void encode_ad_converter_stop(void);
void encode_ad_converter_sleep(void);
void encode_interval_timer_init(uint8_t sampling_rate);
void encode_interval_timer_start(void);
void encode_interval_timer_stop(void);
void encode_interval_timer_sleep(void);

/* Decode function                               */
void decode_interval_timer_init(uint8_t sampling_rate);
void decode_interval_timer_start(void);
void decode_interval_timer_stop(void);
void decode_interval_timer_sleep(void);
void decode_pwm_init(void);
void decode_pwm_start(void);
void decode_pwm_stop(void);
void decode_pwm_sleep(void);
void decode_da_converter_init(void);
void decode_da_converter_start(void);
void decode_da_converter_stop(void);
void decode_da_converter_sleep(void);


#endif 	/*#ifndef	__S2_PERIPHERAL_IF_H__	*/

