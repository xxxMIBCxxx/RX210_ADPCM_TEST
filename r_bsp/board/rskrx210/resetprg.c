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
* Copyright (C) 2014 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : resetprg.c
* Device(s)    : RX210
* Description  : Defines post-reset routines that are used to configure the MCU prior to the main program starting. 
*                This is were the program counter starts on power-up or reset.
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 26.10.2011 1.00     First Release
*         : 13.03.2012 1.10     Stack sizes are now defined in r_bsp_config.h. Because of this the #include for 
*                               stacksct.h was removed. Settings for SCKCR are now set in r_bsp_config.h and used here
*                               to setup clocks based on user settings.
*         : 16.07.2012 1.20     Added ability to enable NMI interrupts based on the BSP_CFG_NMI_ISR_CALLBACK. Also added
*                               code to enable BCLK output based on settings in r_bsp_config.h.
*         : 09.08.2012 1.30     Added checking of BSP_CFG_IO_LIB_ENABLE macro for calling I/O Lib functions.
*         : 09.10.2012 1.40     Added support for all clock options specified by BSP_CFG_CLOCK_SOURCE in r_bsp_config.h.
*                               The code now handles starting the clocks and the required software delays for the clocks
*                               to stabilize. Created clock_source_select() function.
*         : 19.11.2012 1.50     Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 08.05.2013 1.60     Added option to use r_cgc_rx module for clock management. Added code to change HOCO
*                               frequency based on BSP_CFG_HOCO_FREQUENCY macro.
*         : 24.06.2013 1.70     Removed auto-enabling of NMI pin interrupt handling. Fixed a couple of typos. Fixed
*                               a warning that was being produced by '#pragma inline_asm' directive. Added a call
*                               to bsp_interrupt_open() to init callbacks. Added ability to use 1 or both stacks.
*         : 25.11.2013 1.71     LOCO is now turned off when it is not chosen as system clock.
*         : 31.03.2014 1.80     Added the ability for the user to define two 'warm start' callback functions which when
*                               defined result in a callback from PowerON_Reset_PC() before and/or after initialization
*                               of the C runtime environment. Uses the new R_BSP_SoftwareDelay() to do software delays
*                               instead of a loop count.
*         : 15.12.2014 1.90     Replaced R_BSP_SoftwareDelay() calls with loops because clock not set yet.
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
/* Defines MCU configuration functions used in this file */
#include    <_h_c_lib.h>
/* This macro is here so that the stack will be declared here. This is used to prevent multiplication of stack size. */
#define     BSP_DECLARE_STACK
/* Define the target platform */
#include    "platform.h"

/* BCH - 01/16/2013 */
/* 0602: Defect for macro names with '_[A-Z]' is also being suppressed since these are defaut names from toolchain.  
   3447: External linkage is not needed for these toolchain supplied library functions. */
/* PRQA S 0602, 3447 ++ */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
/* If the user chooses only 1 stack then the 'U' bit will not be set and the CPU will always use the interrupt stack. */
#if (BSP_CFG_USER_STACK_ENABLE == 1)
    #define PSW_init  (0x00030000)
#else
    #define PSW_init  (0x00010000)
#endif

/***********************************************************************************************************************
Pre-processor Directives
***********************************************************************************************************************/
/* Set this as the entry point from a power-on reset */
#pragma entry PowerON_Reset_PC

/***********************************************************************************************************************
External function Prototypes
***********************************************************************************************************************/
/* Functions to setup I/O library */
extern void _INIT_IOLIB(void);
extern void _CLOSEALL(void);

#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_PRE_C_FUNCTION(void);
#endif

#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED != 0
/* If user is requesting warm start callback functions then these are the prototypes. */
void BSP_CFG_USER_WARM_START_POST_C_FUNCTION(void);
#endif

/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
/* Power-on reset function declaration */
void PowerON_Reset_PC(void);

#if BSP_CFG_RUN_IN_USER_MODE==1
    #if __RENESAS_VERSION__ < 0x01010000
        /* Declare the contents of the function 'Change_PSW_PM_to_UserMode' as assembler to the compiler */
        #pragma inline_asm Change_PSW_PM_to_UserMode

    /* MCU user mode switcher function declaration */
    static void Change_PSW_PM_to_UserMode(void);
    #endif
#endif

/* Main program function declaration */
void main(void);
#if (BSP_CFG_USE_CGC_MODULE == 0)
static void operating_frequency_set(void);
static void clock_source_select(void);
#else
static void cgc_clock_config(void);
#endif

/***********************************************************************************************************************
* Function name: PowerON_Reset_PC
* Description  : This function is the MCU's entry point from a power-on reset.
*                The following steps are taken in the startup code:
*                1. The User Stack Pointer (USP) and Interrupt Stack Pointer (ISP) are both set immediately after entry 
*                   to this function. The USP and ISP stack sizes are set in the file bsp_config.h.
*                   Default sizes are USP=4K and ISP=1K.
*                2. The interrupt vector base register is set to point to the beginning of the relocatable interrupt 
*                   vector table.
*                3. The MCU operating frequency is set by configuring the Clock Generation Circuit (CGC) in
*                   operating_frequency_set.
*                4. Calls are made to functions to setup the C runtime environment which involves initializing all 
*                   initialed data, zeroing all uninitialized variables, and configuring STDIO if used
*                   (calls to _INITSCT and _INIT_IOLIB).
*                5. Board-specific hardware setup, including configuring I/O pins on the MCU, in hardware_setup.
*                6. Global interrupts are enabled by setting the I bit in the Program Status Word (PSW), and the stack 
*                   is switched from the ISP to the USP.  The initial Interrupt Priority Level is set to zero, enabling 
*                   any interrupts with a priority greater than zero to be serviced.
*                7. The processor is optionally switched to user mode.  To run in user mode, set the macro 
*                   BSP_CFG_RUN_IN_USER_MODE above to a 1.
*                8. The bus error interrupt is enabled to catch any accesses to invalid or reserved areas of memory.
*
*                Once this initialization is complete, the user's main() function is called.  It should not return.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
void PowerON_Reset_PC(void)
{
    /* Stack pointers are setup prior to calling this function - see comments above */    
    
    /* Initialise the MCU processor word */
#if __RENESAS_VERSION__ >= 0x01010000    
    set_intb((void *)__sectop("C$VECT"));
#else
    set_intb((unsigned long)__sectop("C$VECT"));
#endif    
    
#if (BSP_CFG_USE_CGC_MODULE == 0)
    /* Switch to high-speed operation */
    operating_frequency_set();
#else
    cgc_clock_config();
#endif

    /* If the warm start Pre C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_PRE_C_FUNCTION();
#endif

    /* Initialize C runtime environment */
    _INITSCT();

    /* If the warm start Post C runtime callback is enabled, then call it. */
#if BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED == 1
     BSP_CFG_USER_WARM_START_POST_C_FUNCTION();
#endif

#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib */
    _INIT_IOLIB();
#endif

    /* Initialize MCU interrupt callbacks. */
    bsp_interrupt_open();

    /* Initialize register protection functionality. */
    bsp_register_protect_open();

    /* Configure the MCU and board hardware */
    hardware_setup();

    /* Change the MCU's user mode from supervisor to user */
    nop();
    set_psw(PSW_init);      
#if BSP_CFG_RUN_IN_USER_MODE==1
    /* Use chg_pmusr() intrinsic if possible. */
    #if __RENESAS_VERSION__ >= 0x01010000
    chg_pmusr() ;
    #else
    Change_PSW_PM_to_UserMode();
    #endif
#endif

    /* Enable the bus error interrupt to catch accesses to illegal/reserved areas of memory */
    R_BSP_InterruptControl(BSP_INT_SRC_BUS_ERROR, BSP_INT_CMD_INTERRUPT_ENABLE, FIT_NO_PTR);

    /* Call the main program function (should not return) */
    main();
    
#if BSP_CFG_IO_LIB_ENABLE == 1
    /* Comment this out if not using I/O lib - cleans up open files */
    _CLOSEALL();
#endif

    /* BCH - 01/16/2013 */
    /* Infinite loop is intended here. */    
    while(1) /* PRQA S 2740 */
    {
        /* Infinite loop. Put a breakpoint here if you want to catch an exit of main(). */
    }
}


#if (BSP_CFG_USE_CGC_MODULE == 0)

/***********************************************************************************************************************
* Function name: operating_frequency_set
* Description  : Configures the clock settings for each of the device clocks
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void operating_frequency_set (void)
{
    /* Used for constructing value to write to SCKCR register. */
    uint32_t temp_clock = 0;
    
    /* 
    Default settings:
    Clock Description              Frequency
    ----------------------------------------
    Input Clock Frequency............  20 MHz
    PLL frequency (x5)............... 100 MHz
    Internal Clock Frequency.........  50 MHz    
    Peripheral Clock Frequency.......  25 MHz   
    External Bus Clock Frequency.....  25 MHz */
            
    /* Protect off. */
    SYSTEM.PRCR.WORD = 0xA50B;

    /* Select the clock based upon user's choice. */
    clock_source_select();

    /* Figure out setting for FCK bits. */
#if   BSP_CFG_FCK_DIV == 1
    /* Do nothing since FCK bits should be 0. */
#elif BSP_CFG_FCK_DIV == 2
    temp_clock |= 0x10000000;
#elif BSP_CFG_FCK_DIV == 4
    temp_clock |= 0x20000000;
#elif BSP_CFG_FCK_DIV == 8
    temp_clock |= 0x30000000;
#elif BSP_CFG_FCK_DIV == 16
    temp_clock |= 0x40000000;
#elif BSP_CFG_FCK_DIV == 32
    temp_clock |= 0x50000000;
#elif BSP_CFG_FCK_DIV == 64
    temp_clock |= 0x60000000;
#else
    #error "Error! Invalid setting for BSP_CFG_FCK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for ICK bits. */
#if   BSP_CFG_ICK_DIV == 1
    /* Do nothing since ICK bits should be 0. */
#elif BSP_CFG_ICK_DIV == 2
    temp_clock |= 0x01000000;
#elif BSP_CFG_ICK_DIV == 4
    temp_clock |= 0x02000000;
#elif BSP_CFG_ICK_DIV == 8
    temp_clock |= 0x03000000;
#elif BSP_CFG_ICK_DIV == 16
    temp_clock |= 0x04000000;
#elif BSP_CFG_ICK_DIV == 32
    temp_clock |= 0x05000000;
#elif BSP_CFG_ICK_DIV == 64
    temp_clock |= 0x06000000;
#else
    #error "Error! Invalid setting for BSP_CFG_ICK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for BCK bits. */
#if   BSP_CFG_BCK_DIV == 1
    /* Do nothing since BCK bits should be 0. */
#elif BSP_CFG_BCK_DIV == 2
    temp_clock |= 0x00010000;
#elif BSP_CFG_BCK_DIV == 4
    temp_clock |= 0x00020000;
#elif BSP_CFG_BCK_DIV == 8
    temp_clock |= 0x00030000;
#elif BSP_CFG_BCK_DIV == 16
    temp_clock |= 0x00040000;
#elif BSP_CFG_BCK_DIV == 32
    temp_clock |= 0x00050000;
#elif BSP_CFG_BCK_DIV == 64
    temp_clock |= 0x00060000;
#else
    #error "Error! Invalid setting for BSP_CFG_BCK_DIV in r_bsp_config.h"
#endif

    /* Configure PSTOP1 bit for BCLK output. */
#if BSP_CFG_BCLK_OUTPUT == 0    
    /* Set PSTOP1 bit */
    temp_clock |= 0x00800000;
#elif BSP_CFG_BCLK_OUTPUT == 1
    /* Clear PSTOP1 bit */
    temp_clock &= ~0x00800000;
#elif BSP_CFG_BCLK_OUTPUT == 2
    /* Clear PSTOP1 bit */
    temp_clock &= ~0x00800000;
    /* Set BCLK divider bit */
    SYSTEM.BCKCR.BIT.BCLKDIV = 1;
#else
    #error "Error! Invalid setting for BSP_CFG_BCLK_OUTPUT in r_bsp_config.h"
#endif

    /* Figure out setting for PCKB bits. */
#if   BSP_CFG_PCKB_DIV == 1
    /* Do nothing since PCKB bits should be 0. */
#elif BSP_CFG_PCKB_DIV == 2
    temp_clock |= 0x00000100;
#elif BSP_CFG_PCKB_DIV == 4
    temp_clock |= 0x00000200;
#elif BSP_CFG_PCKB_DIV == 8
    temp_clock |= 0x00000300;
#elif BSP_CFG_PCKB_DIV == 16
    temp_clock |= 0x00000400;
#elif BSP_CFG_PCKB_DIV == 32
    temp_clock |= 0x00000500;
#elif BSP_CFG_PCKB_DIV == 64
    temp_clock |= 0x00000600;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKB_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKD bits. */
#if   BSP_CFG_PCKD_DIV == 1
    /* Do nothing since PCKD bits should be 0. */
#elif BSP_CFG_PCKD_DIV == 2
    temp_clock |= 0x00000001;
#elif BSP_CFG_PCKD_DIV == 4
    temp_clock |= 0x00000002;
#elif BSP_CFG_PCKD_DIV == 8
    temp_clock |= 0x00000003;
#elif BSP_CFG_PCKD_DIV == 16
    temp_clock |= 0x00000004;
#elif BSP_CFG_PCKD_DIV == 32
    temp_clock |= 0x00000005;
#elif BSP_CFG_PCKD_DIV == 64
    temp_clock |= 0x00000006;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKB_DIV in r_bsp_config.h"
#endif

    /* b7 to b4 should be 0x1.
       b15 to b12 should be 0x1. */
    temp_clock |= 0x00001010;

    /* Set SCKCR register. */
    SYSTEM.SCKCR.LONG = temp_clock;

    if (SYSTEM.SCKCR.LONG == temp_clock)
    {
        /* Confirm that previous write to SCKCR register has completed. RX MCU's have a 5 stage pipeline architecture
         * and therefore new instructions can be executed before the last instruction has completed. By reading the
         * value of register we will ensure that the value has been written. Not all registers require this
         * verification but the SCKCR is marked as such in the HW manual.
         */
    }

    /* Choose clock source. Default for r_bsp_config.h is PLL. */
    SYSTEM.SCKCR3.WORD = ((uint16_t)BSP_CFG_CLOCK_SOURCE) << 8;

#if (BSP_CFG_CLOCK_SOURCE != 0)
    /* We can now turn LOCO off since it is not going to be used. */
    SYSTEM.LOCOCR.BYTE = 0x01;
#endif

    /* Protect on. */
    SYSTEM.PRCR.WORD = 0xA500;          
}

/***********************************************************************************************************************
* Function name: clock_source_select
* Description  : Enables and disables clocks as chosen by the user.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void clock_source_select (void)
{
    volatile uint8_t read_verify, REG_RCR3;
     /* Declared volatile for software delay purposes. */
    volatile uint32_t i;

    /* Set to High-speed operating mode if ICLK or PCLKD is > 32MHz. */
#if ((BSP_ICLK_HZ > 32000000) || (BSP_PCLKD_HZ > 32000000))
    SYSTEM.OPCCR.BYTE = 0x00;
    
    while(SYSTEM.OPCCR.BIT.OPCMTSF == 1)
    {
        /* Wait for transition to finish. */
    }
#endif

    /* If the main oscillator is >8MHz then the main clock oscillator forced oscillation control register (MOFCR) must
     be changed. The assumption here is that a 16-MHz to 20-MHz non-lead type ceramic resonator is being used.*/
#if (BSP_CFG_XTAL_HZ > 16000000) // if main clock source is 16.1 - 20MHz.
    SYSTEM.MOFCR.BYTE =  0x30;
#elif (BSP_CFG_XTAL_HZ > 8000000) // if main clock source is 8.1 - 16MHz.
    SYSTEM.MOFCR.BYTE = 0x20;
#else                            // if main clock source is 1 - 8MHz.
    SYSTEM.MOFCR.BYTE = 0x10;
#endif


    /* The delay loops used below have been measured to take 86us per iteration. This has been verified using the 
       Renesas RX Toolchain with optimizations set to 2, size priority. The same result was obtained using 2, speed 
       priority. At this time the MCU is still running on the 125kHz LOCO. */

#if (BSP_CFG_CLOCK_SOURCE == 1)
    /* Make sure HOCO is stopped before changing frequency. */
    SYSTEM.HOCOCR.BYTE = 0x01;

    /* Set frequency for the HOCO. */
    SYSTEM.HOCOCR2.BIT.HCFRQ = BSP_CFG_HOCO_FREQUENCY;

    /* For 32 MHz, 36.864 MHz, and 40 MHz, set HOCOWTCR2 to 0x02, For 50 MHz, set HOCOWTCR2 to 0x13. */
#if (BSP_CFG_HOCO_FREQUENCY == 3)
    SYSTEM.HOCOWTCR2.BYTE = 0x03;         // Set HOCO wait time
#else
    SYSTEM.HOCOWTCR2.BYTE = 0x02;
#endif

    /* HOCO is chosen. Start it operating. */
    SYSTEM.HOCOCR.BYTE = 0x00;          

    /* The delay period needed is to make sure that the HOCO has stabilized. According to Rev.1.10 of the RX210's HW 
       manual the delay period is tHOCOWT which is 350us. A delay of 400us has been used below to account for variations 
       in the LOCO.
       400us / 86us (per iteration) = 5 iterations */
    for(i = 0; i < 5; i++)             
    {
        /* Wait 400us. See comment above for why. */
        nop() ;
    }

#else
    /* HOCO is not chosen. */
    /* Stop the HOCO. */
    SYSTEM.HOCOCR.BYTE = 0x01;          

    /* Turn off power to HOCO. */
    SYSTEM.HOCOPCR.BYTE = 0x01;
#endif

#if (BSP_CFG_CLOCK_SOURCE == 2)
    /* Main clock oscillator is chosen. Start it operating. */
    /* Wait 131072 cycles * 20 MHz = 6.55ms */
    SYSTEM.MOSCWTCR.BYTE = 0x0D;        

    /* Set the main clock to operating. */
    SYSTEM.MOSCCR.BYTE = 0x00;          

    /* The delay period needed is to make sure that the main clock has stabilized. This delay period is tMAINOSCWT in 
       the HW manual and according to the Renesas Technical Update document TN-RX*-A021A/E this is defined as:
       n = Wait time selected by MOSCWTCR.MSTS[] bits
       tMAINOSC = Main clock oscillator start-up time. From referring to various vendors, a start-up time of 4ms appears
                  to be a common maximum. To be safe we will use 5ms.
       tMAINOSCWT = tMAINOSC + ((n+16384)/fMAIN)
       tMAINOSCWT = 5ms + ((131072 + 16384)/20MHz)
       tMAINOSCWT = 12.37ms
       A delay of 13ms has been used below to account for variations in the LOCO. 
       13ms / 86us (per iteration) = 151 iterations */
    for(i = 0; i < 151; i++)             
    {
        /* Wait 13ms. See comment above for why. */
        nop() ;
    }
#else
    /* Set the main clock to stopped. */
    SYSTEM.MOSCCR.BYTE = 0x01;          
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3)
    /* Sub-clock oscillator is chosen. Start it operating. */
    /* Wait 65,536 cycles * 32768Hz = 2s. This meets the timing requirement tSUBOSC which is 2 seconds. */
    SYSTEM.SOSCCR.BYTE = 0x01;      // Make sure sub-clock is initially stopped
    REG_RCR3 = RTC.RCR3.BYTE;       // Get existing contents
    while (0x01 != SYSTEM.SOSCCR.BYTE)
    {
        /* Confirm that the written value can be read correctly. */
    }
    RTC.RCR3.BIT.RTCEN = 0;     // Disable RTC module to stop subclock

    // Rev.1.50 Page 920 of UM of RX210, do dummy reads..
    /* dummy read three times */
    for (i = 0; i < 3; i++)
    {
        read_verify = RTC.RCR3.BIT.RTCEN;
    }

    while (0 != RTC.RCR3.BIT.RTCEN)
     {
        /* Confirm that the written */
     }

    /* ---- Wait for five sub-clock cycles ---- */
    /* Wait time is 5 sub-clock cycles (approx. 152 us). 
       152us / 86us (per iteration) = 2 iterations */
    for(i = 0; i < 2; i++)             
    {
        nop() ;
    }
    RTC.RCR3.BYTE = REG_RCR3;       // Restore original sub-clock and state

    /* dummy read three times */
    for (i = 0; i < 3; i++)
    {
        read_verify = RTC.RCR3.BYTE;
    }
    SYSTEM.SOSCWTCR.BYTE = 0x0C;
    /* Set the sub-clock to operating. */
    SYSTEM.SOSCCR.BYTE = 0x00;          

    /* The delay period needed is to make sure that the sub-clock has stabilized. According to Rev.1.00 of the RX63N's 
       HW manual the delay period is tSUBOSCWT0 which is at minimum 1.8s and at maximum 2.6 seconds. We will use the
       maximum value to be safe.    
       2.6s / 86us (per iteration) = 30233 iterations */
    for(i = 0; i < 30233; i++)             
    {
        /* Wait 2.6s. See comment above for why. */
        nop() ;
    }
#else
    /* Set the sub-clock to stopped. */
    SYSTEM.SOSCCR.BYTE = 0x01;          
#endif

#if (BSP_CFG_CLOCK_SOURCE == 4)
    /* PLL is chosen. Start it operating. Must start main clock as well since PLL uses it. */
    /* Wait 131072 cycles * 20 MHz = 6.55ms */
    SYSTEM.MOSCWTCR.BYTE = 0x0D;        

    /* Set the main clock to operating. */
    SYSTEM.MOSCCR.BYTE = 0x00;          

    /* PLL wait is 524,288 cycles * 100 MHz (20 MHz * 5) = 5.24 ms*/
    SYSTEM.PLLWTCR.BYTE = 0x0C;         

    /* Set PLL Input Divisor. */
    SYSTEM.PLLCR.BIT.PLIDIV = BSP_CFG_PLL_DIV >> 1;

    /* Set PLL Multiplier. */
    SYSTEM.PLLCR.BIT.STC = BSP_CFG_PLL_MUL - 1;

    /* Set the PLL to operating. */
    SYSTEM.PLLCR2.BYTE = 0x00;          

    /* The delay period needed is to make sure that the main clock and PLL have stabilized. This delay period is 
       tPLLWT2 when the main clock has not been previously stabilized. According to the Renesas Technical Update 
       document TN-RX*-A021A/E this delay is defined as:
       n = Wait time selected by PLLWTCR.PSTS[] bits
       tMAINOSC = Main clock oscillator start-up time. From referring to various vendors, a start-up time of 4ms appears
                  to be a common maximum. To be safe we will use 5ms.
       tPLLWT2 = tMAINOSC + tPLL1 + ((n + 131072)/fPLL)
       tPLLWT2 = 5ms + 500us + ((524288 + 131072)/100MHz)
       tPLLWT2 = 12.05ms       
       A delay of 13ms has been used below to account for variations in the LOCO. 
       13ms / 86us (per iteration) = 151 iterations */
    for(i = 0; i < 151; i++)             
    {
        /* Wait 13ms. See comment above for why. */
        nop() ;
    }

#else
    /* Set the PLL to stopped. */
    SYSTEM.PLLCR2.BYTE = 0x01;          
#endif

    /* LOCO is saved for last since it is what is running by default out of reset. This means you do not want to turn
       it off until another clock has been enabled and is ready to use. */
#if (BSP_CFG_CLOCK_SOURCE == 0)
    /* LOCO is chosen. This is the default out of reset. */
    SYSTEM.LOCOCR.BYTE = 0x00;
    SYSTEM.VRCR = 0x00;

#else
    /* LOCO is not chosen but it cannot be turned off yet since it is still being used. */
#endif

    /* Make sure a valid clock was chosen. */
#if (BSP_CFG_CLOCK_SOURCE > 4) || (BSP_CFG_CLOCK_SOURCE < 0)
    #error "ERROR - Valid clock source must be chosen in r_bsp_config.h using BSP_CFG_CLOCK_SOURCE macro."
#endif 
}

#else

/***********************************************************************************************************************
* Function name: cgc_clock_config
* Description  : Configures the clock settings for each of the device clocks. Uses the r_cgc_rx module.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
static void cgc_clock_config (void)
{
    cgc_clocks_t                clock;
    cgc_clock_config_t          pll_config;
    cgc_system_clock_config_t   sys_config;

    /* Figure out setting for FCK bits. */
#if   BSP_CFG_FCK_DIV == 1
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_1;
#elif BSP_CFG_FCK_DIV == 2
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_2;
#elif BSP_CFG_FCK_DIV == 4
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_4;
#elif BSP_CFG_FCK_DIV == 8
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_8;
#elif BSP_CFG_FCK_DIV == 16
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_16;
#elif BSP_CFG_FCK_DIV == 32
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_32;
#elif BSP_CFG_FCK_DIV == 64
    sys_config.fclk_div = CGC_SYS_CLOCK_DIV_64;
#else
    #error "Error! Invalid setting for BSP_CFG_FCK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for ICK bits. */
#if   BSP_CFG_ICK_DIV == 1
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_1;
#elif BSP_CFG_ICK_DIV == 2
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_2;
#elif BSP_CFG_ICK_DIV == 4
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_4;
#elif BSP_CFG_ICK_DIV == 8
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_8;
#elif BSP_CFG_ICK_DIV == 16
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_16;
#elif BSP_CFG_ICK_DIV == 32
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_32;
#elif BSP_CFG_ICK_DIV == 64
    sys_config.iclk_div = CGC_SYS_CLOCK_DIV_64;
#else
    #error "Error! Invalid setting for BSP_CFG_ICK_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKB bits. */
#if   BSP_CFG_PCKB_DIV == 1
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_1;
#elif BSP_CFG_PCKB_DIV == 2
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_2;
#elif BSP_CFG_PCKB_DIV == 4
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_4;
#elif BSP_CFG_PCKB_DIV == 8
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_8;
#elif BSP_CFG_PCKB_DIV == 16
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_16;
#elif BSP_CFG_PCKB_DIV == 32
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_32;
#elif BSP_CFG_PCKB_DIV == 64
    sys_config.pclkb_div = CGC_SYS_CLOCK_DIV_64;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKB_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for PCKD bits. */
#if   BSP_CFG_PCKD_DIV == 1
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_1;
#elif BSP_CFG_PCKD_DIV == 2
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_2;
#elif BSP_CFG_PCKD_DIV == 4
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_4;
#elif BSP_CFG_PCKD_DIV == 8
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_8;
#elif BSP_CFG_PCKD_DIV == 16
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_16;
#elif BSP_CFG_PCKD_DIV == 32
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_32;
#elif BSP_CFG_PCKD_DIV == 64
    sys_config.pclkd_div = CGC_SYS_CLOCK_DIV_64;
#else
    #error "Error! Invalid setting for BSP_CFG_PCKD_DIV in r_bsp_config.h"
#endif

    /* Figure out setting for BCK bits. */
#if   BSP_CFG_BCK_DIV == 1
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_1;
#elif BSP_CFG_BCK_DIV == 2
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_2;
#elif BSP_CFG_BCK_DIV == 4
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_4;
#elif BSP_CFG_BCK_DIV == 8
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_8;
#elif BSP_CFG_BCK_DIV == 16
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_16;
#elif BSP_CFG_BCK_DIV == 32
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_32;
#elif BSP_CFG_BCK_DIV == 64
    sys_config.bclk_div = CGC_SYS_CLOCK_DIV_64;
#else
    #error "Error! Invalid setting for BSP_CFG_BCK_DIV in r_bsp_config.h"
#endif

    /* Initialize CGC. */
    R_CGC_Open();

#if (BSP_CFG_CLOCK_SOURCE == 1)
    /* HOCO is chosen. Start it operating. */
    clock = CGC_HOCO;
    R_CGC_ClockStart(clock, (cgc_clock_config_t *)FIT_NO_PTR);
#else
    /* HOCO is not chosen. Stop the HOCO. */
    R_CGC_ClockStop(CGC_HOCO);
#endif

#if (BSP_CFG_CLOCK_SOURCE == 2)
    /* Main clock oscillator is chosen. Start it operating. */
    clock = CGC_MAIN_OSC;
    R_CGC_ClockStart(clock, (cgc_clock_config_t *)FIT_NO_PTR);
#else
    /* Set the main clock to stopped. */
    R_CGC_ClockStop(CGC_MAIN_OSC);
#endif

#if (BSP_CFG_CLOCK_SOURCE == 3)
    /* Sub-clock oscillator is chosen. Start it operating. */
    clock = CGC_SUBCLOCK;
    R_CGC_ClockStart(clock, (cgc_clock_config_t *)FIT_NO_PTR);
#else
    /* Set the sub-clock to stopped. */
    R_CGC_ClockStop(CGC_SUBCLOCK);
#endif

#if (BSP_CFG_CLOCK_SOURCE == 4)
    /* PLL is chosen. Start it operating. Must start main clock as well since PLL uses it. */
    clock = CGC_PLL;

    #if   (BSP_CFG_PLL_DIV == 1)
    pll_config.divider = CGC_PLL_DIV_1;
    #elif (BSP_CFG_PLL_DIV == 2)
    pll_config.divider = CGC_PLL_DIV_2;
    #elif (BSP_CFG_PLL_DIV == 4)
    pll_config.divider = CGC_PLL_DIV_4;
    #else
        #error "Error! Invalid setting for BSP_CFG_PLL_DIV in r_bsp_config.h"
    #endif

    #if   (BSP_CFG_PLL_MUL == 8)
    pll_config.multiplier = CGC_PLL_MUL_8;
    #elif (BSP_CFG_PLL_MUL == 10)
    pll_config.multiplier = CGC_PLL_MUL_10;
    #elif (BSP_CFG_PLL_MUL == 12)
    pll_config.multiplier = CGC_PLL_MUL_12;
    #elif (BSP_CFG_PLL_MUL == 16)
    pll_config.multiplier = CGC_PLL_MUL_16;
    #elif (BSP_CFG_PLL_MUL == 20)
    pll_config.multiplier = CGC_PLL_MUL_20;
    #elif (BSP_CFG_PLL_MUL == 24)
    pll_config.multiplier = CGC_PLL_MUL_24;
    #elif (BSP_CFG_PLL_MUL == 25)
    pll_config.multiplier = CGC_PLL_MUL_25;
    #else
        #error "Error! Invalid setting for BSP_CFG_PLL_MUL in r_bsp_config.h"
    #endif

    R_CGC_ClockStart(CGC_MAIN_OSC, (cgc_clock_config_t *)FIT_NO_PTR);
    R_CGC_ClockStart(clock, &pll_config);
#else
    /* Set the PLL to stopped. */
    R_CGC_ClockStop(CGC_PLL);
#endif

    /* LOCO is saved for last since it is what is running by default out of reset. This means you do not want to turn
       it off until another clock has been enabled and is ready to use. */
#if (BSP_CFG_CLOCK_SOURCE == 0)
    /* LOCO is chosen. This is the default out of reset. */
#else
    /* LOCO is not chosen. Turn off the LOCO and switch to other clock. */
    while (R_CGC_ClockCheck(clock) == CGC_ERR_NOT_STABILIZED)
    {
        /* Wait for chosen clock to stabilize. */
    }

    R_CGC_SystemClockSet(clock, &sys_config);
#endif

    /* Make sure a valid clock was chosen. */
#if (BSP_CFG_CLOCK_SOURCE > 4) || (BSP_CFG_CLOCK_SOURCE < 0)
    #error "ERROR - Valid clock source must be chosen in r_bsp_config.h using BSP_CFG_CLOCK_SOURCE macro."
#endif
}

#endif /* BSP_CFG_USE_CGC_MODULE */

/***********************************************************************************************************************
* Function name: Change_PSW_PM_to_UserMode
* Description  : Assembler function, used to change the MCU's usermode from supervisor to user.
* Arguments    : none
* Return value : none
***********************************************************************************************************************/
#if BSP_CFG_RUN_IN_USER_MODE==1
    #if __RENESAS_VERSION__ < 0x01010000
static void Change_PSW_PM_to_UserMode(void)
{
    MVFC   PSW,R1
    OR     #00100000h,R1
    PUSH.L R1
    MVFC   PC,R1
    ADD    #10,R1
    PUSH.L R1
    RTE
    NOP
    NOP
}
    #endif
#endif


