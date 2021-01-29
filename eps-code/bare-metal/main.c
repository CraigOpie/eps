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

// LED on B27 (Arduino Due)
#define LED_PIN 27

// Output on B14 (Duet)
//#define LED_PIN 14

// PMC definitions
#define PMC_PCER0 *(volatile uint32_t *)0x400E0610

#define ID_PIOA 11
#define ID_PIOB 12
#define ID_PIOC 13
#define ID_PIOD 14
#define ID_PIOE 15
#define ID_PIOF 16

#define PMC_WPMR *(volatile uint32_t *)0x400E06E4

#define PMC_WPKEY 0x504D43

// PIO definitions

struct gpio {
  // + 0x00
  volatile uint32_t PIO_PER;
  volatile uint32_t PIO_PDR;
  volatile uint32_t PIO_PSR;
  volatile uint32_t res1;
  // + 0x10
  volatile uint32_t PIO_OER;
  volatile uint32_t PIO_ODR;
  volatile uint32_t PIO_OSR;
  volatile uint32_t res2;
  // + 0x20
  volatile uint32_t PIO_IFER;
  volatile uint32_t PIO_IFDR;
  volatile uint32_t PIO_IFSR;
  volatile uint32_t res3;
  // + 0x30
  volatile uint32_t PIO_SODR;
  volatile uint32_t PIO_CODR;
  volatile uint32_t PIO_ODSR;
  volatile uint32_t PIO_PDSR;
  // + 0x40
  volatile uint32_t PIO_IER;
  volatile uint32_t PIO_IDR;
  volatile uint32_t PIO_IMR;
  volatile uint32_t PIO_ISR;
  // + 0x50
  volatile uint32_t PIO_MDER;
  volatile uint32_t PIO_MDDR;
  volatile uint32_t PIO_MDSR;
  volatile uint32_t res4;
  // + 0x60
  volatile uint32_t PIO_PUDR;
  volatile uint32_t PIO_PUER;
  volatile uint32_t PIO_PUSR;
  volatile uint32_t res5;
  // + 0x70
  volatile uint32_t PIO_ABSR;
  volatile uint32_t res6[3];
  // + 0x80
  volatile uint32_t PIO_SCIFSR;
  volatile uint32_t PIO_DIFSR;
  volatile uint32_t PIO_IFDGSR;
  volatile uint32_t PIO_SCDR;
  // + 0x90
  volatile uint32_t res7[4];
  // + 0xA0
  volatile uint32_t PIO_OWER;
  volatile uint32_t PIO_OWDR;
  volatile uint32_t PIO_OWSR;
  volatile uint32_t res8;
  // ...
};

#define PIOA ((struct gpio *)0x400E0E00)
#define PIOB ((struct gpio *)0x400E1000)
#define PIOC ((struct gpio *)0x400E1200)
#define PIOD ((struct gpio *)0x400E1400)
#define PIOE ((struct gpio *)0x400E1600)
#define PIOF ((struct gpio *)0x400E1800)

#define PIOA_WPMR *(volatile uint32_t *)0x400E0EE4
#define PIOB_WPMR *(volatile uint32_t *)0x400E10E4
#define PIOC_WPMR *(volatile uint32_t *)0x400E12E4
#define PIOD_WPMR *(volatile uint32_t *)0x400E14E4
#define PIOE_WPMR *(volatile uint32_t *)0x400E16E4

#define PIO_WPKEY 0x50494F

void HardwareInit (void)
{
  // enable peripheral clock
	//  PMC_WPMR  = PMC_WPKEY << 8 | 0;
  PMC_PCER0 = 1 << ID_PIOB;

  // PIOC
	//  PIOC_WPMR  = PIO_WPKEY << 8 | 0;

  PIOB->PIO_PER = 1 << LED_PIN;
  PIOB->PIO_OER = 1 << LED_PIN;
  PIOB->PIO_OWER = 1 << LED_PIN;
}

void delay (volatile uint32_t time)
{
  while (time--)
    __asm ("nop");
}

void main()
{
  HardwareInit ();

  while (1)
  {
    PIOB->PIO_SODR = 1 << LED_PIN;
    delay (100000);
    PIOB->PIO_CODR = 1 << LED_PIN;
    delay (100000);
  }
}
