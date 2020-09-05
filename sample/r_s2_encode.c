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
* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_s2_encode.c
* Version       : 1.01
* Device(s)     :
* Tool-Chain    :
* H/W Platform  :
* Description   : sound middleware sample program.
******************************************************************************/
/******************************************************************************
* History		: DD.MM.YYYY Version Description
* 				: 24.11.2011 1.00	 First Release
* 				: 20.09.2012 1.00	 First Release
*******************************************************************************
*/
/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#if defined(__K0R__)
#pragma sfr
#include "r_cg_timer.h"
#include "r_cg_adc.h"
#endif
#include "r_stdint.h"
#include "r_adpcm.h"
#include "r_s2_driver.h"
#include "r_s2_sample.h"
#include "r_s2_peripheral_if.h"

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
uint8_t			g_EncodedData[MAX_DATA_LENGTH];		/* ADPCM Encoded data storage buffer                        */
int16_t			g_InputData[SAMPLE_CNT];			/* Input data storage buffer                                */
uint8_t			g_EncodedDataTemp[ADPCM_DATA_CNT];	/* Temporary data storage buffer                            */
uint32_t		g_EncodedDataLength;                /* Size of ADPCM encoded data buffer                        */
uint8_t			g_ConversionOver;					/* Flag to indicate whether ADPCM data buffer is full       */
uint8_t			g_bADPCMEn;							/* Flag to indicate whether ADPCM conversion is enabled     */

/*""FUNC COMMENT""******************************************************************
* Function Name	: encode_main
* Include		: r_s2_sample.h
* Declaration	: void encode_main(void)
* Description	: This function plays sound data (analog data) encode in
*				: PCM with ADPCM output.
* Arguments		: none
* Return value	: none
*""FUNC COMMENT END""***************************************************************/
void R_encode_main(void)
{
	/* Initialize the buffers for ADPCM conversion       */
	R_adpcm_initEnc(&g_st_adpcm0) ;
	encode_ad_converter_init();
	encode_interval_timer_init(TIMER_FREQ_08000);

	encode_interval_timer_start();
	/* decode for sound data size */
	/* Wait till the next sample                         */
	while (1)
	{
		/* Check if ADPCM conversion enabled             */
		if (g_bADPCMEn  == TRUE)
		{
			/* Convert the sampled data to ADPCM data    */
			R_convert_to_ADPCM();

			g_bADPCMEn  = FALSE;
		}

		/* Break if the data conversion is complete      */
		if (g_ConversionOver == TRUE)
		{
			g_ConversionOver = FALSE;
			g_EncodedDataLength = 0;
			break;
		}

	}
	encode_interval_timer_stop();
	encode_ad_converter_stop();

	encode_interval_timer_sleep();
	encode_ad_converter_sleep();

}

/******************************************************************************
* Declaration  : void R_convert_to_ADPCM(void);
* Function Name: R_convert_to_ADPCM
* Description  : This function converts the input message into ADPCM data and stores it.
* Argument     : none
* Return Value : none
******************************************************************************/
void R_convert_to_ADPCM(void)
{
	uint8_t count;

	/* Load the i/p and o/p buffers for ADPCM conversion           */
	R_adpcm_refreshEnc(g_InputData , g_EncodedDataTemp , &g_st_adpcm0);

	/* Encode data to ADPCM format                                 */
	R_adpcm_encode(4, &g_st_adpcm0) ;

	/* Store the converted data in RAM                             */
	for (count = 0; count < ADPCM_DATA_CNT; count++)
	{
		/* Check if the ADPCM data buffer is full, exit if full    */
		if (g_EncodedDataLength == MAX_DATA_LENGTH)
		{
			g_ConversionOver = TRUE;
			break;
		}

		g_EncodedData[g_EncodedDataLength] = g_EncodedDataTemp[count];
		g_EncodedDataLength++;
	}
}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_interrupt_encode_timer
* Include		: r_s2_sample.h
* Declaration	: void R_interrupt_encode_timer(void)
* Description	: Timer interrupt function for encoder.
* Arguments     : none
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_interrupt_encode_timer(void)
{
	encode_ad_converter_start();
}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_interrupt_encode_timer
* Include		: r_s2_sample.h
* Declaration	: void R_interrupt_encode_timer(void)
* Description	: D/A converter interrupt function for encoder.
* Arguments     : none
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
#pragma interrupt R_interrupt_encode_ad_convert(vect=VECT_S12AD_S12ADI0)
void R_interrupt_encode_ad_convert(void)
{
	static int8_t s_ucSampleCnt = 0;
	static uint16_t uiADCSample[SAMPLE_CNT];
	uint8_t count;

	uiADCSample[s_ucSampleCnt] = SOUND_INPUT_DATA_SFR;
	uiADCSample[s_ucSampleCnt] += 0x8000;
	s_ucSampleCnt += 1;

	/* Copy the sampled data and pass it for conversion to ADPCM       */
	if (s_ucSampleCnt >= SAMPLE_CNT)
	{
		for (count = 0; count < SAMPLE_CNT; count++)
		{
			g_InputData[count] = uiADCSample[count];
		}
		s_ucSampleCnt = 0;
		g_bADPCMEn = TRUE;
	}

}
