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
* Copyright (C) 2011(2012) Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : r_s2_decode.c
* Version       : 1.01
* Device(s)     :
* Tool-Chain    :
* H/W Platform  :
* Description   : sound middleware sample program.
******************************************************************************/
/******************************************************************************
* History		: DD.MM.YYYY Version Description
* 				: 24.11.2011 1.00	 First Release
* 				: 20.09.2012 1.01
*******************************************************************************
*/
/******************************************************************************
Includes <System Includes> , "Project Includes"
******************************************************************************/
#if defined(__K0R__)
#pragma sfr
#include "r_cg_timer.h"
#endif
#include "r_stdint.h"
#include "r_adpcm.h"
#include "r_s2_driver.h"
#include "r_s2_sample.h"
#include "r_s2_peripheral_if.h"

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
DECINFO 		g_dec_info0;						/* decoded information structure */
RINGBUFFER		g_rbf0;								/* ring buffer structure */
adpcm_env		g_st_adpcm0;                        /* ADPCM decoding structure */
uint32_t		g_readErrCntCH1;					/* Output Error Counter */


/*""FUNC COMMENT""******************************************************************
* Function Name	: decode_main
* Include		: r_s2_sample.h
* Declaration	: void decode_main(void)
* Description	: This function plays sound data (ADPCM data) compressed in
*				: ADPCM with PWM/DA output.
* Arguments		: none
* Return value	: none
*""FUNC COMMENT END""***************************************************************/
void R_decode_main(void)
{
	/* decode initialization(4bytes each) */
	decode_interval_timer_init(TIMER_FREQ_11025);
	SOUND_MODULE_INIT();

	R_InitDecInfo(&g_dec_info0, &g_st_adpcm0, (const uint8_t  *)ADPCM_ADDR1);
	R_InitRingBuffer(&g_rbf0);

	/* decode: get PCM data 4bytes(initial) */
	R_DecodeProc(&g_dec_info0, &g_rbf0);
	g_dec_info0.playmode = PLAY_PLAYBACK;

	/* enable timer interrupt for audio outputs */
	SOUND_MODULE_START();
	decode_interval_timer_start();

	/* decode for sound data size */
	while (g_dec_info0.decno < PCM_DATA_SIZE1)
	{
		/* decode and update decode information */
		R_DecodeProc(&g_dec_info0, &g_rbf0);
	}

	/* When finished decoding all data, wait till an audio output is available */
	while (g_dec_info0.playmode != PLAY_END) {}

	SOUND_MODULE_STOP();
	decode_interval_timer_stop();

	SOUND_MODULE_SLEEP();
	decode_interval_timer_sleep();

}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_InitDecInfo
* Include		: r_s2_sample.h
* Declaration	: void R_InitDecInfo( DECINFO *info, adpcm_env *adpcm, const uint8_t *addr)
* Description	: decoded information structure(DECINFO) initialization.
* Arguments     : DECINFO       *info        ; decoded information structure
*               : adpcm_env     *adpcm       ; ADPCM decoding structure
*               : const uint8_t *addr        ; top address of ADPCM sound data
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_InitDecInfo(DECINFO  *info, adpcm_env  *adpcm, const uint8_t  *addr)
{
	info->adpcm = adpcm;
	info->playmode = PLAY_NONE;
	info->decno = 0;
	info->playno = 0;
	info->inputaddr = (uint8_t  *)addr;

	/* initialization and the initial setting of ADPCM decoding structure */
	R_adpcm_initDec(info->adpcm);

}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_InitRingBuffer
* Include		: r_s2_sample.h
* Declaration	: void R_InitRingBuffer( RINGBUFFER *rbf )
* Description	: Initializes setting of the ring buffer to store decoded data.
* Arguments     : RINGBUFFER *rbf       ; ring buffer structure
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_InitRingBuffer(RINGBUFFER  *rbf)
{
	rbf->w_pos = &rbf->buff[0];
	rbf->r_pos = &rbf->buff[0];
	rbf->empty = PCMBUFSIZE - PCMDECSIZE;
}


/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_DecodeProc
* Include		: r_s2_sample.h
* Declaration	: void R_DecodeProc( DECINFO *info, RINGBUFFER *rbf )
* Description	: Converts ADPCM data into PCM data and stores decoded data
*               : in a ring buffer.
* Arguments     : DECINFO *info       ; decoded information structure
*               : RINGBUFFER *rbf     : ring buffer structure
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_DecodeProc(DECINFO  *info, RINGBUFFER  *rbf)
{
	if (rbf->empty >= PCMDECSIZE)
	{
		R_adpcm_refreshDec(info->inputaddr, info->pcm_data, info->adpcm);
		R_adpcm_decode(4, info->adpcm);
		info->inputaddr += (PCMDECSIZE / 2);
		R_RingBufferSetData(info, rbf , PCMDECSIZE);
	}
}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_RingBufferSetData
* Include		: r_s2_sample.h
* Declaration	: void R_RingBufferSetData( DECINFO *info, RINGBUFFER *rbf ,uint16_t size)
* Description	: Check state of the ring buffer before writing data.
* Arguments     : DECINFO *info       ; decoded information structure
*               : RINGBUFFER *rbf     : ring buffer structure
*               : uint16_t size       : ring buffer set size (Unit: 2byte)
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_RingBufferSetData(DECINFO  *info, RINGBUFFER  *rbf , uint16_t size)
{
	uint8_t push = 0;
	uint16_t work;

	work = &rbf->buff[PCMBUFSIZE] - rbf->w_pos;

	if (rbf->r_pos < rbf->w_pos)
	{
		if (rbf->w_pos + size < &rbf->buff[PCMBUFSIZE])
		{
			/*                              w                 */
			/*               r              [============]    */
			/* --------------+--------------+---------------- */
			/* space available between write position and end of buffer */
			push = 1;
		}
		else if (size < (&rbf->buff[PCMBUFSIZE] - rbf->w_pos) + (rbf->r_pos - &rbf->buff[0]))
		{
			/*                                       w        */
			/* =====]        r                       [======= */
			/* --------------+-----------------------+------- */
			/* write position is at end of buffer, space available at top */
			push = 2;
		}
	}
	else if (rbf->r_pos > rbf->w_pos)
	{
		if ((rbf->w_pos + size) < rbf->r_pos)
		{
			/*               w                                */
			/*               [============] r                 */
			/* --------------+--------------+---------------- */
			/* space available between write position and read position */
			push = 3;
		}
	}
	else
	{
		if (rbf->empty > size)
		{
			if ((rbf->w_pos + size) < &rbf->buff[PCMBUFSIZE])
			{
				/*               r                                */
				/*               w                                */
				/*               [============]                   */
				/* --------------+------------------------------- */
				/* space available between write position and end of buffer */

				push = 4;
			}
			else
			{
				/*                                        r       */
				/*                                        w       */
				/* ======]                                [====== */
				/* ---------------------------------------+------ */
				/* write position is at end of buffer, space available at top */
				push = 5;
			}
		}
	}


	if (push)
	{
		if ((1 == push) || (3 == push) || (4 == push))
		{
			R_RingBufferPush(info->pcm_data, rbf->w_pos, size);
			rbf->w_pos += size;
			rbf->empty -= size;
			info->decno += size;
			if (rbf->w_pos >= &rbf->buff[PCMBUFSIZE])
			{
				rbf->w_pos = &rbf->buff[0];
			}
		}
		else
		{
			work = (&rbf->buff[PCMBUFSIZE] - rbf->w_pos);
			R_RingBufferPush(info->pcm_data, rbf->w_pos, work);
			rbf->w_pos = &rbf->buff[0];
			R_RingBufferPush(info->pcm_data + work, rbf->w_pos, (size - work));
			rbf->w_pos = rbf->w_pos + (size - work);
			rbf->empty -= size;
			info->decno += size;
		}
	}
}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_RingBufferPush
* Include		: r_s2_sample.h
* Declaration	: void R_RingBufferPush( int16_t *src, uint16_t *dest, int32_t times )
* Description	: Convert data length.Push data to ring buffer.
* Arguments     : int16_t  *src   ; source address
*               : uint16_t *dest  ; destination address
*               : int32_t  times  ; number of data items
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_RingBufferPush(int16_t  *src, uint16_t  *dest, int32_t times)
{
	int32_t i;

	for (i = 0 ; i < times ; i++)
	{
		dest[i] = SOUND_OUTPUT_DATA_CNV(src[i]);
	}
}


/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_Convert16toPWM
* Include		: r_s2_sample.h
* Declaration	: uint16_t R_Convert16toPWM( int16_t data )
* Description	: Convert PCM data(16bits) into output PWM data(16bits).
* Arguments     : int16_t  data   ; PCM data(16bits)
* Return value	: uint16_t        ; output PWM data(16bits)
*""FUNC COMMENT END""*****************************************************************/
uint16_t R_Convert16toPWM(int16_t data)
{
	return (uint16_t)(((int32_t)(data + 32768)* PWM_SAMPLING) >> 16);
}


/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_Convert16to10
* Include		: r_sample.h
* Declaration	: uint16_t R_Convert16to10( int16_t data )
* Description	: Convert PCM data(16bits) into  D/A output data(10bits).
* Arguments     : int16_t  data   ; PCM data(16bits)
* Return value	: uint16_t        ; D/A output data(10bits)
*""FUNC COMMENT END""*****************************************************************/
uint16_t R_Convert16to10(int16_t data)
{
	return (uint16_t)(((data + 0x20) >> 6) + 512);
}


/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_SetPCMdata
* Include		: r_s2_sample.h
* Declaration	: void R_SetPCMdata( void )
* Description	: read sound data(PCM) from ring buffer.
*               : send sound data(PCM) to DA/PWM output.
* Arguments     : none
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
void R_SetPCMdata(void)
{
	uint8_t pop = 0;                 /* 0 : no output */

	if (g_rbf0.empty < PCMBUFSIZE)
	{
		/* send sound data(PCM) to DA output every sampling period */
		if (g_rbf0.r_pos < g_rbf0.w_pos)
		{
			pop = 1;
		}
		else if (g_rbf0.r_pos > g_rbf0.w_pos)
		{
			/* read position is not at end of buffer */
			pop = 2;
		}
		else
		{
			/* no data in buffer(read position == write position) */
			g_readErrCntCH1++;
		}

		if (pop)
		{
			/* set PCM data to PWM data reg. */
			SOUND_OUTPUT_DATA_SFR = *g_rbf0.r_pos;
			g_rbf0.r_pos++;
			if (g_rbf0.r_pos >= &g_rbf0.buff[PCMBUFSIZE])
			{
				g_rbf0.r_pos = &g_rbf0.buff[0];
			}
			g_rbf0.empty++;
			g_dec_info0.playno++;
		}
	}
}

/*""FUNC COMMENT""*********************************************************************
* Function Name	: R_interrupt_decode_timer
* Include		: r_s2_sample.h
* Declaration	: void R_interrupt_decode_timer(void)
* Description	: Timer interrupt function for decoder.
* Arguments     : none
* Return value	: none
*""FUNC COMMENT END""*****************************************************************/
#pragma interrupt R_interrupt_decode_timer(vect=VECT_MTU1_TGIA1)
void R_interrupt_decode_timer(void)
{
	if (g_dec_info0.playno >= PCM_DATA_SIZE1)
	{
		/* Audio output is completed */
		g_dec_info0.playmode = PLAY_END;
	}
	else
	{
		/* PCM data(decoded data) --> DA/PWM output */
		R_SetPCMdata();
	}
}
