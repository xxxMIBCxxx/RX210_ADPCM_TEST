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
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : hwsetup.c
* Device(s)    : RX
* H/W Platform : RSKRX210
* Description  : Defines the initialization routines used each time the MCU is restarted.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 14.02.2012 1.00     First Release
*         : 07.05.2013 1.10     Added a call to bsp_non_existent_port_init() which initializes non-bonded out GPIO pins.
*         : 25.06.2013 1.20     Replaced manual calls to unlock MPC registers with calls to r_bsp API functions.
*                               Moved call to bsp_non_existent_port_init() in hardware_setup() to last so that user
*                               does not have to worry about writing 'missing' bits in PDR registers.
*         : 03.12.2013 1.30     Cleaned up pin setup code to set PMR registers after MPC registers.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* I/O Register and board definitions */
#include "platform.h"

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* MCU I/O port configuration function declaration */
static void output_ports_configure(void);

/* Interrupt configuration function declaration */
static void interrupts_configure(void);

/* MCU peripheral module configuration function declaration */
static void peripheral_modules_enable(void);


/***********************************************************************************************************************
* Function name: hardware_setup
* Description  : Contains setup functions called at device restart
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void hardware_setup(void)
{
    output_ports_configure();
    interrupts_configure();
    peripheral_modules_enable();
    bsp_non_existent_port_init();
}

/***********************************************************************************************************************
* Function name: output_ports_configure
* Description  : Configures the port and pin direction settings, and sets the pin outputs to a safe level.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void output_ports_configure(void)
{
#if 0
    /* Enable LEDs. */
    /* Start with LEDs off. */
    LED0 = LED_OFF;
    LED1 = LED_OFF;
    LED2 = LED_OFF;
    LED3 = LED_OFF;

    /* Set LED pins as outputs. */
    LED0_PDR = 1;
    LED1_PDR = 1;
    LED2_PDR = 1;
    LED3_PDR = 1;

    /* Enable switches. */
    /* Set pins as inputs. */
    SW1_PDR = 0;
    SW2_PDR = 0;
    SW3_PDR = 0;

    /* Set port mode registers for switches. */
    SW1_PMR = 0;
    SW2_PMR = 0;
    SW3_PMR = 0;
#endif
	
    /* Unlock MPC registers to enable writing to them. */
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
    
#if 0
    /* TXD0 is output. */        
    PORT2.PMR.BIT.B0 = 0;
    MPC.P20PFS.BYTE  = 0x0A;   
    PORT2.PDR.BIT.B0 = 1;
    PORT2.PMR.BIT.B0 = 1;
    /* RXD0 is input. */    
    PORT2.PMR.BIT.B1 = 0;
    MPC.P21PFS.BYTE  = 0x0A;    
    PORT2.PDR.BIT.B1 = 0;
    PORT2.PMR.BIT.B1 = 1;
    
    /* Port A -  SPI signals, chip selects */   
    PORTA.PMR.BYTE  = 0x00 ;    /* All GPIO for now */
    MPC.PA5PFS.BYTE = 0x0D ;    /* PA5 is RSPCKA */
    MPC.PA6PFS.BYTE = 0x0D ;    /* PA6 is MOSIA */
    MPC.PA7PFS.BYTE = 0x0D ;    /* PA7 is MISOA */    
    PORTA.PODR.BYTE = 0x17 ;    /* All outputs low to start */
    PORTA.PDR.BYTE  = 0x7F ;    /* All outputs except MISO */       
    PORTA.PMR.BYTE  = 0xE0 ;    /* PA5-7 assigned to SPI peripheral */
#endif
	
    /* Configure the pin connected to the ADC Pot as an analog input */
    PORT4.PMR.BIT.B0 = 0;
    MPC.P40PFS.BYTE = 0x80;     //Set ASEL bit and clear the rest
    PORT4.PDR.BIT.B0 = 0;   

    /* Lock MPC registers. */
    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

/***********************************************************************************************************************
* Function name: interrupts_configure
* Description  : Configures interrupts used
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void interrupts_configure(void)
{
    /* Add code here to setup additional interrupts */
}

/***********************************************************************************************************************
* Function name: peripheral_modules_enable
* Description  : Enables and configures peripheral devices on the MCU
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void peripheral_modules_enable(void)
{
    /* Add code here to enable peripherals used by the application */
}
