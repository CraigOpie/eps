/******************************************************************************

:::====  :::==== :::==== 	Copyright (C) 2020 Death Star Development, LLC
 :::  === :::     :::  ===	All rights reserved.  This file contains 
 ===  ===  =====  ===  === 	confidential and proprietary information of 
 ===  ===     === ===  === 	Death Star Development, LLC and is protected
 =======  ======  =======  	under U.S. and international copyright and 
other intellectual property laws.  Distribution of this file requires
written consent from Death Star Development, LLC legal team.  Any person or
entity that is found to be re-distributing this material will be subject to 
criminal and civil prosecution.  DO NOT TAKE CREDIT FOR OUR WORK.

*******************************************************************************

Vendor: Death Star Development, LLC
Date Created: 
Version: 0.4.0

Associated Filename:
Associated Document: 
      https://git.deathstardev.com/dsd-projects/eps

*******************************************************************************

Disclaimer:

      This disclaimer is not a license and does not grant any rights to
      the materials distributed herewith. Except as otherwise provided in
      a valid license issued to you by DSD, and to the maximum extent
      permitted by applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE
      "AS IS" AND WITH ALL FAULTS, AND DSD HEREBY DISCLAIMS ALL
      WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY,
      INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
      NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
      (2) DSD shall not be liable (whether in contract or tort,
      including negligence, or under any other theory of liability) for
      any loss or damage of any kind or nature related to, arising under
      or in connection with these materials, including any direct, or
      any indirect, special, incidental, or consequential loss or damage
      (including loss of data, profits, goodwill, or any type of loss or
      damage suffered as a result of any action brought by a third party)
      even if such damage or loss was reasonably foreseeable or DSD
      had been advised of the possibility of the same.

Critical Applications:

      DSD products are not designed or intended to be fail-safe, or
      for use in any application requiring fail-safe performance, such as
      life-support or safety devices or systems, Class III medical
      devices, nuclear facilities, applications related to the deployment
      of airbags, or any other applications that could lead to death,
      personal injury, or severe property or environmental damage
      (individually and collectively, "Critical Applications"). Customer
      assumes the sole risk and liability of any use of DSD products
      in Critical Applications, subject only to applicable laws and
      regulations governing limitations on product liability.

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS
FILE AT ALL TIMES.

*******************************************************************************

Please refer to the documentation at 
https://git.deathstardev.com/dsd-projects/eps
for the following topics:

1. Revision History
2. Overview
3. Software Tools and System Requirements
4. Installation and Operating Instructions
5. Demo Instructions
6. Design Files
7. Other Information

Access is limited to those with specific rights as a Death Star Development
employee, and those individuals with a need to know.

******************************************************************************/

#include <stdint.h>

#define SCB_VTOR_ADDR 0xE000ED08

#define SCB_VTOR *(volatile uint32_t *)SCB_VTOR_ADDR

// These must be defned in linker file
extern unsigned long _etext;
extern unsigned long _srelocate;
extern unsigned long _erelocate;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;


extern int main(void);

typedef void( *const intfunc )( void );

void Reset_Handler(void) __attribute__((__interrupt__));
void Default_Handler(void);


#define NMI_Handler         Default_Handler
#define HardFault_Handler   Default_Handler
#define MemManage_Handler   Default_Handler
#define BusFault_Handler    Default_Handler
#define UsageFault_Handler  Default_Handler
#define MemManage_Handler   Default_Handler
#define SVC_Handler         Default_Handler
#define DebugMon_Handler    Default_Handler
#define PendSV_Handler      Default_Handler
#define SysTick_Handler     Default_Handler


__attribute__ ((section(".vectors")))
void (* const g_pfnVectors[])(void) = {
    (intfunc)((unsigned long)&_estack), /* The stack pointer after relocation */
    Reset_Handler,              /* Reset Handler */
    NMI_Handler,                /* NMI Handler */
    HardFault_Handler,          /* Hard Fault Handler */
    MemManage_Handler,          /* MPU Fault Handler */
    BusFault_Handler,           /* Bus Fault Handler */
    UsageFault_Handler,         /* Usage Fault Handler */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    0,                          /* Reserved */
    SVC_Handler,                /* SVCall Handler */
    DebugMon_Handler,           /* Debug Monitor Handler */
    0,                          /* Reserved */
    PendSV_Handler,             /* PendSV Handler */
    SysTick_Handler             /* SysTick Handler */
};

void Reset_Handler(void)
{
    /* Init Data:
     * - Loads data from addresses defined in linker file into RAM
     * - Zero bss (statically allocated uninitialized variables)
     */
    unsigned long *src, *dst;

    /* copy the data segment into ram */
    src = &_etext;
    dst = &_srelocate;
    if (src != dst)
        while(dst < &_erelocate)
            *(dst++) = *(src++);

    /* zero the bss segment */
    dst = &_sbss;
    while(dst < &_ebss)
        *(dst++) = 0;

    SCB_VTOR = ((uint32_t)g_pfnVectors & (uint32_t)0x1FFFFF80);

    main();
    while(1) {}
}

void Default_Handler(void)
{
    while (1) {}
}

