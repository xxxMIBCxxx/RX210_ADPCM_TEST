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
* File Name     : r_s2_sample.h
* Version       : 1.00
* Device(s)     :
* Tool-Chain    :
* H/W Platform  :
* Description   : sound middleware sample program header file.
******************************************************************************/
/******************************************************************************
* History		: DD.MM.YYYY Version Description
* 				: 20.09.2012 1.00	 First Release
*******************************************************************************
*/
#ifndef _R_S2_SAMPLE_H_
#define	_R_S2_SAMPLE_H_

#include "r_adpcm.h"
/**************************************************************************************
Common Macro Definitions
**************************************************************************************/
#define	NO_SELECT		(0x00)
#define SELECT_ENCODE	(0x01)
#define	SELECT_DECODE	(0x02)

/**************************************************************************************
Decode Macro Definitions
**************************************************************************************/
#define PCMDECSIZE		4					/* number to convert from ADPCM into PCM */

/* Size of decoded sound data buffer(Please make it a numerical integer double in order to
 convert from ADPCM into PCM) */
#define PCMBUFSIZE		(PCMDECSIZE*10)
#define PCM_DATA_SIZE1	0x3de4				/* size of sound data after decode(compressed data) */

/* Output Mode */
#define SOUND_PWM		1
#define SOUND_DA		2

/**************************************************************************************
Encode Macro Definitions
**************************************************************************************/
#define SAMPLE_CNT		(4u)				/*    No. of input samples passed for ADPCM conversion at a time	*/
#define ADPCM_DATA_CNT	(2u)				/*    No. of ADPCM samples received after ADPCM conversion			*/


/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum
{
	PLAY_NONE       = 0,					/* no sound data output */
	PLAY_PLAYBACK,							/* during sound data output */
	PLAY_END								/* end of sound data output */
} PLAYMODE;

typedef struct								/* decoded information structure */
{
	adpcm_env	*adpcm;						/* ADPCM decoding structure */
	uint8_t 	*inputaddr;					/* input adpcm address */
	int16_t		pcm_data[PCMDECSIZE];		/* decoded sound data(16bit) storage */
	volatile	PLAYMODE	playmode;		/* state of playback */
	uint32_t	decno;						/* number of sound data(decoded) */
	uint32_t	playno;						/* number of sound data(completed output) */
} DECINFO;

typedef struct								/* ring buffer structure */
{
	uint16_t	buff[PCMBUFSIZE];			/* 16bit PCM data storage */
	uint16_t	*w_pos;						/* write address of ring buffer */
	uint16_t	*r_pos;						/* read address of ring buffer */
	volatile	uint16_t  empty;			/* writable size of ring buffer */
} RINGBUFFER;

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/
extern DECINFO 		g_dec_info0;						/* decoded information structure							*/
extern RINGBUFFER	g_rbf0;								/* ring buffer structure									*/
extern adpcm_env	g_st_adpcm0;						/* ADPCM decoding structure									*/
extern uint32_t		g_readErrCntCH1;					/* Output Error Counter										*/
extern int16_t		g_InputData[SAMPLE_CNT];			/* Input data storage buffer                                */
extern uint8_t		g_EncodedData[MAX_DATA_LENGTH];		/* ADPCM Encoded data storage buffer                        */
extern uint8_t		g_EncodedDataTemp[ADPCM_DATA_CNT];	/* Temporary data storage buffer                            */
extern uint32_t		g_EncodedDataLength;                /* Size of ADPCM encoded data buffer                        */
extern uint8_t		g_ConversionOver;					/* Flag to indicate whether ADPCM data buffer is full       */
extern uint8_t		g_bADPCMEn;							/* Flag to indicate whether ADPCM conversion is enabled     */

void R_decode_main(void);
void R_InitDecInfo(DECINFO *info, adpcm_env *adpcm, const uint8_t *addr);
void R_InitRingBuffer(RINGBUFFER *rbf);
void R_DecodeProc(DECINFO *info, RINGBUFFER *rbf);
void R_RingBufferSetData(DECINFO *info, RINGBUFFER *rbf, uint16_t lsize);
void R_RingBufferPush(int16_t *src, uint16_t *dest, int32_t times);
void R_SetPCMdata(void);
uint16_t R_Convert16to10(int16_t data);
uint16_t R_Convert16toPWM(int16_t data);
void R_interrupt_decode_timer(void);

void R_encode_main(void);
void R_convert_to_ADPCM(void);
void R_interrupt_encode_ad_convert(void);
void R_interrupt_encode_timer(void);

#endif	/* _R_S2_SAMPLE_H_ */
