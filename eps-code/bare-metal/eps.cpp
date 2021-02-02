/***********************************************************************************************************************************************************

:::====  :::==== :::====     Copyright (C) 2020 Death Star Development, LLC
 :::  === :::     :::  ===    All rights reserved.  This file contains
 ===  ===  =====  ===  ===     confidential and proprietary information of
 ===  ===     === ===  ===     Death Star Development, LLC and is protected
 =======  ======  =======      under U.S. and international copyright and
other intellectual property laws.  Distribution of this file requires
written consent from Death Star Development, LLC legal team.  Any person or
entity that is found to be re-distributing this material will be subject to
criminal and civil prosecution.  DO NOT TAKE CREDIT FOR OUR WORK.

**************************************************************************************************************************************************************

Vendor: Death Star Development, LLC
Date Created:
Version: 0.4.0

Associated Filename:
Associated Document:
      https://git.deathstardev.com/dsd-projects/eps

**************************************************************************************************************************************************************

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

**************************************************************************************************************************************************************

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

**************************************************************************************************************************************************************/

#include "eps.h"

/***********************************************************************************************************************************************************
 *   Private GPIO High:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::gpioHigh(int _group, int _pin)
{
    _group->PIO_SODR = 1 << _pin;
    delay(10);
}

/***********************************************************************************************************************************************************
 *   Private GPIO Low:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::gpioLow(int _group, int _pin)
{
    _group->PIO_CODR = 1 << _pin;
    delay(10);
}

/***********************************************************************************************************************************************************
 *   Private Board Status:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::boardStatus()
{
  byte buffer_[2];
  buffer_[1] = this->board_status;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}

/***********************************************************************************************************************************************************
 *   Private Get Last Error:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getLastError()
{
  byte buffer_[2];
  buffer_[1] = this->last_error;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}

/***********************************************************************************************************************************************************
 *   Private Get Board Version:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getVersion()
{
  byte buffer_[4];
  buffer_[3] = 0x00;
  buffer_[2] = 0x00;
  buffer_[1] = 0x04;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 4);
}

/***********************************************************************************************************************************************************
 *   Private Get Telemetry:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getTelemetry()
{
  // This is effectively just temperature using SPI
  byte buffer_[2];
  
  /********************************************************************************************************************************************************
   *   IIDIODE_OUT - BCR Output Current (14.662757 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x84))
  {
    buffer_[1] = 0x10;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   VIDIODE_OUT - BCR Output Voltage (0.008993157 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x80))
  {
    buffer_[1] = 0xF3;
    buffer_[0] = 0x06;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   I3V3_DRW - 3V3 Current (0.001327547 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x05))
  {
    buffer_[1] = 0xE2;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   I5V_DRW - 5V Current (0.001327547 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x05))
  {
    buffer_[1] = 0x79;
    buffer_[0] = 0x01;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   IPCM12V - 12V Bus Current (0.00207 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x34))
  {
    buffer_[1] = 0xA9;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   VPCM12V - 12V Bus Voltage (0.01349 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x30))
  {
    buffer_[1] = 0x7A;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   IPCMBATV - BAT Bus Current (0.005237 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x24))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************************************************************************************************
   *   VPCMBATV - BAT Bus Voltage (0.008978 x ADC Count)
   *********************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x20))
  {
    buffer_[1] = 0x81;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /*************************************************************************************************************************************************************
   *   IPCM5V - 5V Bus Current (0.005237 x ADC Count)
   ************************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x14))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /*************************************************************************************************************************************************************
   *   VPCM5V - 5V Bus Voltage (0.005865 x ADC Count)
   ************************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x10))
  {
    buffer_[1] = 0x55;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /*************************************************************************************************************************************************************
   *   IPCM3V3 - 3V3 Bus Current (0.005237 x ADC Count)
   ************************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x04))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /*************************************************************************************************************************************************************
   *   VPCM3V3 - 3V3 Bus Voltage (0.004311 x ADC Count)
   ************************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x00))
  {
    buffer_[1] = 0xFE;
    buffer_[0] = 0x02;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /*************************************************************************************************************************************************************
   *   TBRD - Temperature Data (0.372434 x ADC Count)
   ************************************************************************************************************************************************************/
  if ((this->i2c_data[1] == 0xE3) && (this->i2c_data[2] == 0x08))
  {
    buffer_[1] = 0x16;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
}

/***********************************************************************************************************************************************************
 *   Private Get Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getWatchdog()
{
  byte buffer_[2];
  buffer_[1] = (this->watchdog_limit / 1000.0 / 60.0);
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}

/***********************************************************************************************************************************************************
 *   Private Set Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::setWatchdog()
{
  this->watchdog_limit = (unsigned long)((int)this->i2c_data[1] * 60.0 * 1000.0);
}

/***********************************************************************************************************************************************************
 *   Private Reset Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::resetWatchdog()
{
  this->gpioLow(WDT_RESET);
  this->gpioHigh(WDT_RESET);
  this->gpioLow(WDT_RESET);
  this->gpioHigh(WDT_RESET);
}

/***********************************************************************************************************************************************************
 *   Private Set All PDMs On:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::allPDMsOn()
{
  this->gpioHigh(SW1_ENABLE);
  this->gpioHigh(SW2_ENABLE);
  this->gpioHigh(SW3_ENABLE);
  this->gpioHigh(SW4_ENABLE);
  this->gpioHigh(SW5_ENABLE);
  this->gpioHigh(VBAT_ENABLE);
}

/***********************************************************************************************************************************************************
 *   Private Set All PDMs Off:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::allPDMsOff()
{
  this->gpioLow(SW1_ENABLE);
  this->gpioLow(SW2_ENABLE);
  this->gpioLow(SW3_ENABLE);
  this->gpioLow(SW4_ENABLE);
  this->gpioLow(SW5_ENABLE);
  this->gpioLow(VBAT_ENABLE);
}

/***********************************************************************************************************************************************************
 *   Private Get Actual PDM Status:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getPDMsActual()
{
  if (!this->i2c_request)
  {
    this->pdm_status = (digitalRead(SW1_ENABLE)) ? (this->pdm_status | 0x01) : (this->pdm_status & 0xFE);
    this->pdm_status = (digitalRead(SW2_ENABLE)) ? (this->pdm_status | 0x02) : (this->pdm_status & 0xFD);
    this->pdm_status = (digitalRead(SW3_ENABLE)) ? (this->pdm_status | 0x04) : (this->pdm_status & 0xFB);
    this->pdm_status = (digitalRead(SW4_ENABLE)) ? (this->pdm_status | 0x08) : (this->pdm_status & 0xF7);
    this->pdm_status = (digitalRead(SW5_ENABLE)) ? (this->pdm_status | 0x10) : (this->pdm_status & 0xEF);
    this->pdm_status = (digitalRead(VBAT_ENABLE)) ? (this->pdm_status | 0x20) : (this->pdm_status & 0xDF);
  }
  else
  {
    byte buffer_[2];
    buffer_[1] = this->pdm_status;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
}

/***********************************************************************************************************************************************************
 *   Private Get Expected PDMs:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::getPDMsExpected()
{
  byte buffer_[2];
  buffer_[1] = this->pdm_expected;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}

/***********************************************************************************************************************************************************
 *   Private Set PDM On:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::setPDMOn()
{
  switch(this->i2c_data[1])
  {
    case  1:
          this->gpioHigh(SW1_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x01;
          break;
    case  2:
          this->gpioHigh(SW2_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x02;
          break;
    case  3:
          this->gpioHigh(SW3_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x04;
          break;
    case  4:
          this->gpioHigh(SW4_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x08;
          break;
    case  5:
          this->gpioHigh(SW5_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x10;
          break;
    case  6:
          this->gpioHigh(VBAT_ENABLE);
          this->pdm_expected = this->pdm_expected | 0x20;
          break;
    default:
          // FAULT
          break;
  }
}

/***********************************************************************************************************************************************************
 *   Private Set PDM Off:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::setPDMOff()
{
  switch(this->i2c_data[1])
  {
    case  1:
          this->gpioLow(SW1_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xfe;
          break;
    case  2:
          this->gpioLow(SW2_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xfd;
          break;
    case  3:
          this->gpioLow(SW3_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xfb;
          break;
    case  4:
          this->gpioLow(SW4_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xf7;
          break;
    case  5:
          this->gpioLow(SW5_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xef;
          break;
    case  6:
          this->gpioLow(VBAT_ENABLE);
          this->pdm_expected = this->pdm_expected & 0xdf;
          break;
    default:
          // FAULT
          break;
  }
}

/***********************************************************************************************************************************************************
 *   Private Make the EPS Reset Using Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::epsReset()
{
  this->reset_eps = true;
}

/***********************************************************************************************************************************************************
 *   Public Constructor:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   Pointer
 ************************************************************************************************************************************************************/
DSD_EPS::DSD_EPS()
{
    /*****************************************************************************************************************************************************
     *   Establish Default Status to Keep the EPS ON
     ******************************************************************************************************************************************************/
    this->reset_eps = false;
    
    /*****************************************************************************************************************************************************
     *   Establish Default Pin Modes
     ******************************************************************************************************************************************************/
    pinMode(WDT_RESET, OUTPUT);
    pinMode(I2C_ENABLE, OUTPUT);
    pinMode(SW1_ENABLE, OUTPUT);
    pinMode(SW2_ENABLE, OUTPUT);
    pinMode(SW3_ENABLE, OUTPUT);
    pinMode(SW4_ENABLE, OUTPUT);
    pinMode(SW5_ENABLE, OUTPUT);
    pinMode(VBAT_ENABLE, OUTPUT);
    
    /*****************************************************************************************************************************************************
     *   Establish Default Pin Values
     ******************************************************************************************************************************************************/
    this->gpioHigh(WDT_RESET);
    this->gpioLow(I2C_ENABLE);
    this->gpioLow(SW1_ENABLE);
    this->gpioLow(SW2_ENABLE);
    this->gpioLow(SW3_ENABLE);
    this->gpioLow(SW4_ENABLE);
    this->gpioLow(SW5_ENABLE);
    this->gpioLow(VBAT_ENABLE);
    
    /*****************************************************************************************************************************************************
     *   Reset the Watchdog Timer
     ******************************************************************************************************************************************************/
    this->resetWatchdog();
    this->watchdog_limit = (4.0 * 60.0 * 1000.0);
    this->watchdog_last_reset = millis();
    this->watchdog_internal = millis();
    
    /*****************************************************************************************************************************************************
     *   Place the EPS I2C Circuit on the CSKB Bus
     ******************************************************************************************************************************************************/
    this->i2c_receive = false;
    this->i2c_request = false;
    this->gpioHigh(I2C_ENABLE);
}

/***********************************************************************************************************************************************************
 *   Public Deconstructor:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   Null Pointer
 ************************************************************************************************************************************************************/
DSD_EPS::~DSD_EPS()
{
    // No deconstructor required for this application
}

/***********************************************************************************************************************************************************
 *   Public Delay:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::delay(volatile uint32_t time)
{
    while (time--)
        __asm ("nop");
}

/***********************************************************************************************************************************************************
 *   Public Read the I2C Bus:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::readI2C()
{
  switch(this->i2c_data[0])
  {
    case  BOARD_STATUS:
          this->boardStatus();
          break;
    case  GET_LAST_ERROR:
          this->getLastError();
          break;
    case  GET_VERSION:
          this->getVersion();
          break;
    case  GET_TELEMETRY:
          this->getTelemetry();
          break;
    case  GET_WATCHDOG:
          this->getWatchdog();
          break;
    case  SET_WATCHDOG:
          this->setWatchdog();
    case  RESET_WATCHDOG:
          this->resetWatchdog();
          this->watchdog_last_reset = millis();
          break;
    case  ALL_PDMS_ON:
          this->allPDMsOn();
          break;
    case  ALL_PDMS_OFF:
          this->allPDMsOff();
          break;
    case  GET_PDMS_ACTUAL:
          this->getPDMsActual();
          break;
    case  GET_PDMS_EXPECTED:
          this->getPDMsExpected();
          break;
    case  SET_PDM_ON:
          this->setPDMOn();
          break;
    case  SET_PDM_OFF:
          this->setPDMOff();
          break;
    case  EPS_RESET:
          this->epsReset();
          break;
    default:
          this->board_status = this->board_status | 0x01;
          this->last_error = 0x13;
          break;
  }
  
  if (this->i2c_request)
  {
    this->i2c_request = false;
  }
}

/***********************************************************************************************************************************************************
 *   Public Read the UART Connection:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::readUART()
{
  // Future implementation using UART for command and control
}

/***********************************************************************************************************************************************************
 *   Public Watchdog:
 *      Description:  Checks status of watchdog elapsed time vs. the
 *                    watchdog maximum time and returns if the value
 *                    has been exceeded
 *      Parameters:   Current system time in milliseconds
 *      Return Val:   None
 ************************************************************************************************************************************************************/
void DSD_EPS::watchdog()
{
  while (!this->reset_eps)
  {
    unsigned long now = millis() - this->watchdog_last_reset;
    if ((now >= this->watchdog_limit) && (now < 2*this->watchdog_limit))
    {
      this->board_status = this->board_status | 0x02;
      this->last_error = 0x13;
    }
    else
    {
      if ((millis() - this->watchdog_internal) > 500)
      {
        this->resetWatchdog();
        UART.println("Watchdog Reset");
        this->watchdog_internal = millis();
        this->getPDMsActual();
      }
    }
    if (this->i2c_receive)
    {
      this->readI2C();
      this->i2c_receive = false;
      for (int i = 0; i < this->bytes; i++)
      {
        if (i < MAX_I2C_BYTES)
        {
          this->i2c_data[i] = 0;
        }
      }
    }
  }
}
