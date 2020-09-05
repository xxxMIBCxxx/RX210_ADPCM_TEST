/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_s2_driver.h
* Version       : 1.00
* Device(s)     : R5F52108
* Tool-Chain    : RX Standard Toolchain V.1.2.1.0
* H/W Platform  : RX210 renesas Starter Kit
* Description   : sound middleware sample program.
******************************************************************************/
/******************************************************************************
* History		: DD.MM.YYYY Version Description
* 				: 20.09.2012 1.00	 First Release
*******************************************************************************
*/
#ifndef _R_S2_DRIVER_H_
#define	_R_S2_DRIVER_H_
#include "r_adpcm.h"
/**************************************************************************************
Common Macro Definitions
**************************************************************************************/
#define SET_BIT_HIGH	(1)
#define SET_BIT_LOW		(0)
#define SET_BYTE_HIGH	(0xFF)
#define SET_BYTE_LOW	(0x00)
#define ON				(1)
#define OFF				(0)

#define TRUE            (1)
#define FALSE           (0)

/**************************************************************************************
Decode Macro Definitions
**************************************************************************************/
#define ADPCM_ADDR1		0xFFF80000			/* top address of ADPCM sound data storage */
#define PWM_SAMPLING	(249UL)

								// AE-RX210 GND : CN2-1, CN2-2
//#define SOUND_OUTPUT_MODULE		SOUND_DA		// AE-RX210 : CN2-30
#define SOUND_OUTPUT_MODULE		SOUND_PWM		// AE-RX210 : CN1-12

/**************************************************************************************
Encode Macro Definitions
**************************************************************************************/
#define MAX_DATA_LENGTH	( 0x1EF4 )			/*    Max. size of ADPCM encoded data								*/


#endif	/* _R_S2_DRIVER_H_ */
