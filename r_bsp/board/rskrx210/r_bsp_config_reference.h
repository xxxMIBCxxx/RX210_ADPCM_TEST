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
* File Name    : r_bsp_config_reference.h
* Device(s)    : RX210
* Description  : The file r_bsp_config.h is used to configure your BSP. r_bsp_config.h should be included
*                somewhere in your package so that the r_bsp code has access to it. This file (r_bsp_config_reference.h)
*                is just a reference file that the user can use to make their own r_bsp_config.h file.
************************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 13.03.2012 1.00    First Release            
*         : 27.06.2012 1.10    Added MCU product part number section. Added endian options for user app and user boot.
*                              Added OFS register macros. Added option for ROM Code Protect. Added callback function
*                              options for exception interrupts.
*         : 16.07.2012 1.20    Fixed bug with exception callback function names. Added BSP_CFG_BCLK_OUTPUT macro option.
*         : 09.08.2012 1.30    Added BSP_CFG_IO_LIB_ENABLE macro.
*         : 16.11.2012 1.40    Added BSP_CFG_USER_LOCKING_ENABLED and BSP_CFG_USER_LOCKING_TYPE macros.
*         : 19.11.2012 1.50    Updated code to use 'BSP_' and 'BSP_CFG_' prefix for macros.
*         : 17.01.2013 1.60    Moved BSP_CFG_IO_LIB_ENABLE macro since it is now used in conjunction with 
*                              BSP_CFG_HEAP_BYTES. Added comments for disabling heap above BSP_CFG_HEAP_BYTES. 
*                              Added BSP_CFG_PARAM_CHECKING_ENABLE macro for configuring global parameter checking.
*         : 06.05.2013 1.70    Added new packages to comments for the BSP_CFG_MCU_PART_PACKAGE macro. Added new macro
*                              (BSP_CFG_MCU_PART_VERSION) for version of the chip being used. Added new options for
*                              memory sizes. Removed BSP_CFG_USER_APP_ENDIAN & BSP_CFG_USER_BOOT_ENDIAN macros. Endian 
*                              is now detected using compiler macros. Added BSP_CFG_USE_CGC_MODULE macro so that users
*                              can use r_cgc_rx module for clock management. Added BSP_CFG_HOCO_FREQUENCY macro for
*                              choosing frequency of the HOCO. Added BSP_CFG_MCU_VCC_MV macro for FIT modules that need
*                              to know Vcc.
*         : 05.31.2013 1.71    Added 'Chip version C' to BSP_CFG_MCU_PART_VERSION macro options.
*         : 25.06.2013 1.80    Added BSP_CFG_USER_STACK_ENABLE macro to enable/disable use of user stack. Removed 
*                              macros defining exception callbacks, undefined interrupt callback, and bus error 
*                              callback. Callbacks are now handled through mcu_interrupts.c.
*         : 31.03.2014 1.90    Added the ability for the user to define two 'warm start' callback functions which when
*                              defined result in a callback from PowerON_Reset_PC() before and/or after initialization
*                              of the C runtime environment. Added r_bsp_config file version number, which must match
*                              the BSP version number or a compiler warning will result. Added ability to specify user
*                              defined functions for redirection of the stdio charget() and/or charput() functions.
***********************************************************************************************************************/

#ifndef R_BSP_CONFIG_REF_HEADER_FILE
#define R_BSP_CONFIG_REF_HEADER_FILE

/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/
/* Enter the product part number for your MCU. This information will be used to obtain information about your MCU such 
   as package and memory size. 
   To help parse this information, the part number will be defined using multiple macros.
   R 5 F 52 10 8 A D FP
   | | | |  |  | | | |  Macro Name              Description
   | | | |  |  | | | |__BSP_CFG_MCU_PART_PACKAGE      = Package type, number of pins, and pin pitch
   | | | |  |  | | |____not used                      = Products with wide temperature range (D:-40-85C G:-40-105C)
   | | | |  |  | |______BSP_CFG_MCU_PART_VERSION      = Version of chip (A or B)
   | | | |  |  |________BSP_CFG_MCU_PART_MEMORY_SIZE  = ROM, RAM, and Data Flash Capacity
   | | | |  |___________BSP_CFG_MCU_PART_GROUP        = Group name  
   | | | |______________BSP_CFG_MCU_PART_SERIES       = Series name
   | | |________________BSP_CFG_MCU_PART_MEMORY_TYPE  = Type of memory (Flash)
   | |__________________not used                      = Renesas MCU
   |____________________not used                      = Renesas semiconductor product. 
   */

/* Package type. Set the macro definition based on values below:
   Character(s) = Value for macro = Package Type/Number of Pins/Pin Pitch
   FP           = 0x0             = LQFP/100/0.50
   FN           = 0x1             = LQFP/80/0.50
   FM           = 0x2             = LQFP/64/0.50
   FK           = 0x3             = LQFP/64/0.80
   LF           = 0x4             = TFLGA/100/0.65
   FB           = 0x5             = LQFP/144/0.50
   FL           = 0x6             = LQFP/48/0.50
   FF           = 0x7             = LQFP/80/0.65
   LK           = 0x8             = TFLGA/145/0.50
   LA           = 0x9             = TFLGA/100/0.50
   LJ           = 0xA             = TFLGA/100/0.65
   LH           = 0xB             = TFLGA/64/0.65
*/
#define BSP_CFG_MCU_PART_PACKAGE        (0x0)

/* Chip version.
   Character(s) = Value for macro = Package Type/Number of Pins/Pin Pitch
   A            = 0xA             = Chip version A
   B            = 0xB             = Chip version B
   C            = 0xC             = Chip version C
*/
#define BSP_CFG_MCU_PART_VERSION        (0xA)

/* ROM, RAM, and Data Flash Capacity. 
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   B            = 0xB             =   1MB/96KB/8KB
   A            = 0xA             = 768KB/96KB/8KB
   8            = 0x8             = 512KB/64KB/8KB
   7            = 0x7             = 384KB/64KB/8KB
   6            = 0x6             = 256KB/32KB/8KB
   5            = 0x5             = 128KB/20KB/8KB
   4            = 0x4             =  96KB/16KB/8KB
   3            = 0x3             =  64KB/12KB/8KB
*/
#define BSP_CFG_MCU_PART_MEMORY_SIZE    (0x8)

/* Group name. 
   Character(s) = Value for macro = Description
   10           = 0x0             = RX210 Group  
*/
#define BSP_CFG_MCU_PART_GROUP          (0x0)

/* Series name. 
   Character(s) = Value for macro = Description
   52           = 0x0             = RX200 Series
*/  
#define BSP_CFG_MCU_PART_SERIES         (0x0)

/* Memory type. 
   Character(s) = Value for macro = Description
   F            = 0x0             = Flash memory version
*/
#define BSP_CFG_MCU_PART_MEMORY_TYPE    (0x0)

/* Whether to use 1 stack or 2. RX MCUs have the ability to use 2 stacks: an interrupt stack and a user stack.
 * When using 2 stacks the user stack will be used during normal user code. When an interrupt occurs the CPU
 * will automatically shift to using the interrupt stack. Having 2 stacks can make it easier to figure out how
 * much stack space to allocate since the user does not have to worry about always having enough room on the
 * user stack for if-and-when an interrupt occurs. Some users will not want 2 stacks though because it is not
 * needed in all applications and can lead to wasted RAM (i.e. space in between stacks that is not used).
 * If only 1 stack is used then the interrupt stack is the one that will be used. If 1 stack is chosen then
 * the user may want to remove the 'SU' section from the linker sections to remove any linker warnings.
 *
 * 0 = Use 1 stack. Disable user stack. User stack size set below will be ignored.
 * 1 = Use 2 stacks. User stack and interrupt stack will both be used.
 */
#define BSP_CFG_USER_STACK_ENABLE       (1)

/* The 'BSP_DECLARE_STACK' macro is checked so that the stack is only declared in one place (resetprg.c). Every time a 
   '#pragma stacksize' is encountered, the stack size is increased. This prevents multiplication of stack size. */
#if defined(BSP_DECLARE_STACK)
    /* If only 1 stack is chosen using BSP_CFG_USER_STACK_ENABLE then no RAM will be allocated for the user stack. */
    #if (BSP_CFG_USER_STACK_ENABLE == 1)
    /* User Stack size in bytes. The Renesas RX toolchain sets the stack size using the #pragma stacksize directive. */
    #pragma stacksize su=0x1000
    #endif

/* Interrupt Stack size in bytes. The Renesas RX toolchain sets the stack size using the #pragma stacksize directive.
 * If the interrupt stack is the only stack being used then the user will likely want to increase the default size
 * below.
 */
#pragma stacksize si=0x400
#endif

/* Heap size in bytes.
   To disable the heap you must follow these steps:
   1) Set this macro (BSP_CFG_HEAP_BYTES) to 0.
   2) Set the macro BSP_CFG_IO_LIB_ENABLE to 0.
   3) Disable stdio from being built into the project library. This is done by going into the Renesas RX Toolchain 
      settings and choosing the Standard Library section. After that choose 'Standard Library' for Category in HEW or 
      choose 'Contents' in E2Studio. This will present a list of modules that can be included. Uncheck the box for
      stdio.h. 
*/
#define BSP_CFG_HEAP_BYTES              (0x400)

/* Initializes C input & output library functions.
   0 = Disable I/O library initialization in resetprg.c. If you are not using stdio then use this value.
   1 = Enable I/O library initialization in resetprg.c. This is default and needed if you are using stdio. */
#define BSP_CFG_IO_LIB_ENABLE           (1)

/* If desired the user may redirect the stdio charget() and/or charput() functions to their own respective functions
   by enabling below and providing and replacing the my_sw_... function names with the names of their own functions. */
#define BSP_CFG_USER_CHARGET_ENABLED    (0)
#define BSP_CFG_USER_CHARGET_FUNCTION     my_sw_charget_function

#define BSP_CFG_USER_CHARPUT_ENABLED    (0)
#define BSP_CFG_USER_CHARPUT_FUNCTION     my_sw_charput_function

/* After reset MCU will operate in Supervisor mode. To switch to User mode, set this macro to '1'. For more information
   on the differences between these 2 modes see the CPU >> Processor Mode section of your MCU's hardware manual.
   0 = Stay in Supervisor mode.
   1 = Switch to User mode.
*/
#define BSP_CFG_RUN_IN_USER_MODE        (0)

/* To get into User Boot Mode the user must control some pins on the MCU and also set some values in ROM. These values
   in ROM are described in the Option-Setting Memory section of the hardware manual. This macro sets these values so 
   that User Boot Mode can be used. The user is still responsible for setting the MCU pins appropriately. If User Boot
   is selected then the user should make sure that they setup a constant data section in their linker named 'UBSETTINGS'
   at address 0xFF7FFFE8. 
   0 = Single-Chip or USB Boot Mode
   1 = User Boot Mode
*/
#define BSP_CFG_USER_BOOT_ENABLE        (0)

/* Set your desired ID code. NOTE, leave at the default (all 0xFF's) if you do not wish to use an ID code. If you set 
   this value and program it into the MCU then you will need to remember the ID code because the debugger will ask for 
   it when trying to connect. Note that the E1/E20 will ignore the ID code when programming the MCU during debugging.
   If you set this value and then forget it then you can clear the ID code by connecting up in serial boot mode using 
   FDT. The ID Code is 16 bytes long. The macro below define the ID Code in 4-byte sections. */
/* Lowest 4-byte section, address 0xFFFFFFA0. From MSB to LSB: Control Code, ID code 1, ID code 2, ID code 3. */
#define BSP_CFG_ID_CODE_LONG_1          (0xFFFFFFFF)
/* 2nd ID Code section, address 0xFFFFFFA4. From MSB to LSB: ID code 4, ID code 5, ID code 6, ID code 7. */
#define BSP_CFG_ID_CODE_LONG_2          (0xFFFFFFFF)
/* 3rd ID Code section, address 0xFFFFFFA8. From MSB to LSB: ID code 8, ID code 9, ID code 10, ID code 11. */
#define BSP_CFG_ID_CODE_LONG_3          (0xFFFFFFFF)
/* 4th ID Code section, address 0xFFFFFFAC. From MSB to LSB: ID code 12, ID code 13, ID code 14, ID code 15. */
#define BSP_CFG_ID_CODE_LONG_4          (0xFFFFFFFF)

/* The r_bsp can be configured to use the r_cgc_rx210 module for clock management. The r_bsp does come with code
   to setup the clocks but it provides no management capabilities and has no ability to change clock settings. If
   the user wishes to change the clocks after reset then they should use the r_cgc_rx210 module.
   0 = Use built-in clock code
   1 = Use r_cgc_rx210 module for clock management
*/
#define BSP_CFG_USE_CGC_MODULE          (0)

/* Clock source select (CKSEL).
   0 = Low Speed On-Chip Oscillator  (LOCO)
   1 = High Speed On-Chip Oscillator (HOCO)
   2 = Main Clock Oscillator  
   3 = Sub-Clock Oscillator
   4 = PLL Circuit
*/ 
#define BSP_CFG_CLOCK_SOURCE            (4)

/* Clock configuration options.
   The input clock frequency is specified and then the system clocks are set by specifying the multipliers used. The
   multiplier settings are used to set the clock registers in resetprg.c. If a 20MHz clock is used and the 
   ICLK is 50MHz, PCLKB is 25MHz, FCLK is 50MHz, PCLKD is 50MHz, and BCLK is 25MHz then the 
   settings would be:

   BSP_CFG_XTAL_HZ = 20000000
   BSP_CFG_PLL_DIV = 2  (no division)
   BSP_CFG_PLL_MUL = 10 (10MHz x 10 = 100MHz)
   BSP_CFG_ICK_DIV =  2      : System Clock (ICLK)        = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_ICK_DIV)  = 50MHz
   BSP_CFG_PCKB_DIV = 4      : Peripheral Clock B (PCLKB) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKB_DIV) = 25MHz
   BSP_CFG_PCKD_DIV = 2      : Peripheral Clock D (PCLKD) = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_PCKB_DIV) = 50MHz
   BSP_CFG_FCK_DIV =  4      : Flash IF Clock (FCLK)      = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_FCK_DIV)  = 25MHz
   BSP_CFG_BCK_DIV =  4      : External Bus Clock (BCK)   = 
                               (((BSP_CFG_XTAL_HZ/BSP_CFG_PLL_DIV) * BSP_CFG_PLL_MUL) / BSP_CFG_BCK_DIV)  = 25MHz
*/
/* XTAL - Input clock frequency in Hz */
#define BSP_CFG_XTAL_HZ                 (20000000)

/* PLL Input Frequency Divider Select (PLIDIV). 
   Available divisors = /1 (no division), /2, /4
*/
#define BSP_CFG_PLL_DIV                 (2)

/* PLL Frequency Multiplication Factor Select (STC). 
   Available multipliers = x8, x10, x12, x16, x20, x24, x25
*/
#define BSP_CFG_PLL_MUL                 (10)

/* System Clock Divider (ICK).
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_ICK_DIV                 (2)

/* Peripheral Module Clock B Divider (PCKB). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKB_DIV                (4)

/* Peripheral Module Clock D Divider (PCKD). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_PCKD_DIV                (2)

/* External Bus Clock Divider (BCK). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_BCK_DIV                 (4)

/* Flash IF Clock Divider (FCK). 
   Available divisors = /1 (no division), /2, /4, /8, /16, /32, /64
*/
#define BSP_CFG_FCK_DIV                 (4)

/* Configure BCLK output pin (only effective when external bus enabled)
   Values 0=no output, 1 = BCK frequency, 2= BCK/2 frequency */
#define BSP_CFG_BCLK_OUTPUT             (0)

/* The HOCO can operate at several different frequencies. Choose which one using the macro below.
   Available frequency settings:
   0 = 32MHz     (default)
   1 = 36.864MHz
   2 = 40MHz
   3 = 50MHz
*/
#define BSP_CFG_HOCO_FREQUENCY          (0)

/* Configure WDT and IWDT settings. 
   OFS0 - Option Function Select Register 0 
   b31:b29 Reserved (set to 1)
   b28     WDTRSTIRQS - WDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
   b27:b26 WDTRPSS - WDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
   b25:b24 WDTRPES - WDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
   b23:b20 WDTCKS - WDT Clock Frequency Division Ratio - (1=/4, 4=/64, 0xF=/128, 6=/512, 7=/2048, 8=/8192)
   b19:b18 WDTTOPS - WDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
   b17     WDTSTRT - WDT Start Mode Select - (0=auto-start after reset, halt after reset)
   b16:b15 Reserved (set to 1)
   b14     IWDTSLCSTP - IWDT Sleep Mode Count Stop Control - (0=can't stop count, 1=stop w/some low power modes)
   b13     Reserved (set to 1)
   b12     IWDTRSTIRQS - IWDT Reset Interrupt Request - What to do on underflow (0=take interrupt, 1=reset MCU)
   b11:b10 IWDTRPSS - IWDT Window Start Position Select - (0=25%, 1=50%, 2=75%, 3=100%,don't use)
   b9:b8   IWDTRPES - IWDT Window End Position Select - (0=75%, 1=50%, 2=25%, 3=0%,don't use)
   b7:b4   IWDTCKS - IWDT Clock Frequency Division Ratio - (0=none, 2=/16, 3 = /32, 4=/64, 0xF=/128, 5=/256)
   b3:b2   IWDTTOPS - IWDT Timeout Period Select - (0=1024 cycles, 1=4096, 2=8192, 3=16384)
   b1      IWDTSTRT - IWDT Start Mode Select - (0=auto-start after reset, halt after reset)
   b0      Reserved (set to 1) */
#define BSP_CFG_OFS0_REG_VALUE  (0xFFFFFFFF) //Disable by default

/* Configure whether voltage detection 0 circuit and HOCO are enabled after reset. 
   OFS1 - Option Function Select Register 1 
   b31:b9 Reserved (set to 1)
   b8     HOCOEN - Enable/disable HOCO oscillation after a reset (0=enable, 1=disable)
   b7:b3  Reserved (set to 1)
   b2     LVDAS - Choose to enable/disable Voltage Detection 0 Circuit after a reset (0=enable, 1=disable)
   b1:b0  VDSEL - Voltage Detection 0 Level Select (0=3.8V, 1=2.8V, 2=1.9V, 3=1.72V) */
#define BSP_CFG_OFS1_REG_VALUE  (0xFFFFFFFF) //Disable by default

/* ROM Code Protection is a feature that can prohibit PROM programmers from reading or writing flash memory.
   There are 3 different options that can be chosen:
   0    = ROM code protection 1 = Both reading and writing to the user area and user boot area are prohibited.
   1    = ROM code protection 2 = Reading from the user area and user boot area are prohibited.
   Else = Protection disabled   = Reading and writing are allowed.
*/
#define BSP_CFG_ROM_CODE_PROTECT_VALUE  (0xFFFFFFFF)

/* This macro lets other modules no if a RTOS is being used.
   0 = RTOS is not used. 
   1 = RTOS is used.
*/
#define BSP_CFG_RTOS_USED               (0)

/* By default modules will use global locks found in mcu_locks.c. If the user is using a RTOS and would rather use its
   locking mechanisms then they can change this macro. 
   NOTE: If '1' is chosen for this macro then the user must also change the next macro 'BSP_CFG_USER_LOCKING_TYPE'.
   0 = Use default locking (non-RTOS)
   1 = Use user defined locking mechanism. 
*/
#define BSP_CFG_USER_LOCKING_ENABLED    (0)

/* If the user decides to use their own locking mechanism with FIT modules then they will need to redefine the typedef
   that is used for the locks. If the user is using a RTOS then they would likely redefine the typedef to be 
   a semaphore/mutex type of their RTOS. Use the macro below to set the type that will be used for the locks. 
   NOTE: If BSP_CFG_USER_LOCKING_ENABLED == 0 then this typedef is ignored.
   NOTE: Do not surround the type with parentheses '(' ')'.
*/
#define BSP_CFG_USER_LOCKING_TYPE       bsp_lock_t

/* If the user decides to use their own locking mechanism with FIT modules then they will need to define the functions
   that will handle the locking and unlocking. These functions should be defined below. 
   If BSP_CFG_USER_LOCKING_ENABLED is != 0:
   R_BSP_HardwareLock(mcu_lock_t hw_index) will call BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION(mcu_lock_t hw_index)
   R_BSP_HardwareUnlock(mcu_lock_t hw_index) will call BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION(mcu_lock_t hw_index)
       NOTE:With these functions the index into the array holding the global hardware locks is passed as the parameter.
   R_BSP_SoftwareLock(BSP_CFG_USER_LOCKING_TYPE * plock) will call 
   BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION(BSP_CFG_USER_LOCKING_TYPE * plock)
   R_BSP_SoftwareUnlock(BSP_CFG_USER_LOCKING_TYPE * plock) will call 
   BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION(BSP_CFG_USER_LOCKING_TYPE * plock)
       NOTE:With these functions the actual address of the lock to use is passed as the parameter.   
   NOTE: These functions must return a boolean. If lock was obtained or released successfully then return true. Else, 
         return false. 
   NOTE: If BSP_CFG_USER_LOCKING_ENABLED == 0 then this typedef is ignored.
   NOTE: Do not surround the type with parentheses '(' ')'.
*/
#define BSP_CFG_USER_LOCKING_HW_LOCK_FUNCTION      my_hw_locking_function
#define BSP_CFG_USER_LOCKING_HW_UNLOCK_FUNCTION    my_hw_unlocking_function
#define BSP_CFG_USER_LOCKING_SW_LOCK_FUNCTION      my_sw_locking_function
#define BSP_CFG_USER_LOCKING_SW_UNLOCK_FUNCTION    my_sw_unlocking_function

/* If the user would like to determine if a warm start reset has occurred, then they may enable one or more of the following
 * callback definitions AND provide a call back function name for the respective callback function (to be defined by the user).
 * Setting BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED = 1 will result in a callback to the user defined my_sw_warmstart_prec_function
 * just prior to the initialization of the C runtime environment by resetprg.
 *
 * Setting BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED = 1 will result in a callback to the user defined my_sw_warmstart_postc_function
 * just after the initialization of the C runtime environment by resetprg.
 */
#define BSP_CFG_USER_WARM_START_CALLBACK_PRE_INITC_ENABLED    (0)
#define BSP_CFG_USER_WARM_START_PRE_C_FUNCTION     my_sw_warmstart_prec_function

#define BSP_CFG_USER_WARM_START_CALLBACK_POST_INITC_ENABLED    (0)
#define BSP_CFG_USER_WARM_START_POST_C_FUNCTION    my_sw_warmstart_postc_function

/* By default FIT modules will check input parameters to be valid. This is helpful during development but some users
   will want to disable this for production code. The reason for this would be to save execution time and code space. 
   This macro is a global setting for enabling or disabling parameter checking. Each FIT module will also have its
   own local macro for this same purpose. By default the local macros will take the global value from here though
   they can be overridden. Therefore, the local setting has priority over this global setting. Disabling parameter
   checking should only used when inputs are known to be good and the increase in speed or decrease in code space is 
   needed. 
   0 = Global setting for parameter checking is disabled. 
   1 = Global setting for parameter checking is enabled (Default).
*/
#define BSP_CFG_PARAM_CHECKING_ENABLE               (1)

/* This macro is used to define the voltage that is supplied to the MCU (Vcc). This macro is defined in millivolts. This
   macro does not actually change anything on the MCU. Some FIT modules need this information so it is defined here. */
#define BSP_CFG_MCU_VCC_MV                          (3300)

#endif /* R_BSP_CONFIG_REF_HEADER_FILE */



