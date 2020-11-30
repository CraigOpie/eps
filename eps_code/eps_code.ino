/********************************************************************
 *   
 *   DSD EPS Software
 *   
 *******************************************************************   
 *******************************************************************
 *   Purpose
 *   
 *   Sketch to operate DSD EPS                      
 *                                                                  
 *   Program editors:                                               
 *     Craig Opie                                                   
 *                                                                  
 *******************************************************************/
/********************************************************************
 *   Editing history
 *   
 *   08/04/20 - Version 0.1.0 - Craig:
 *   Initial Draft.
 *   - Standard object code and methods
 *              
 *******************************************************************/
/********************************************************************
 *   Hardware compatibility
 *   
 *   DSD EPS v0.4.0
 *   Arduino Nano Every
 *   
 *******************************************************************/
/********************************************************************
 *   IO assignments
 *   
 *   pin 1 --> UART Tx - CSKB H1.49
 *   pin 2 <-- UART Rx - CSKB H1.51
 *   pin 3 <-- Reset
 *   pin 4 --- GND
 *   pin 5 --> I2C enable
 *   pin 6 --> VBAT enable
 *   pin 7 --> SW1 enable
 *   pin 8 --> SW2 enable
 *   pin 9 --> SW3 enable
 *   pin 10 --> SW4 enable
 *   pin 11 --> SPI SS to U11
 *   pin 12 --> SW5 enable
 *   pin 13 --> WDT reset
 *   pin 14 <-- SPI DIN from U11
 *   pin 15 --> SPI DOUT to U11
 *   pin 16 --> SPI SCLK to U11
 *   pin 17 --> +3V3
 *   pin 18 --- AREF
 *   pin 19 <-- AIN3
 *   pin 20 <-- AIN2
 *   pin 21 <-- AIN1
 *   pin 22 <-- AIN0
 *   pin 23 <-> I2C SDA - CSKB H1.41
 *   pin 24 <-> I2C SCL - CSKB H1.43
 *   pin 25 <-- AIN4
 *   pin 26 <-- AIN5
 *   pin 27 --> +5V
 *   pin 28 --- GND
 *   pin 29 <-- Reset
 *   pin 30 <-- VIN
 *   
 *******************************************************************/


/********************************************************************
 *   Includes
 *******************************************************************/
#include <Wire.h>


/********************************************************************
 *   Definitions
 *******************************************************************/
#define I2C_ADDRESS                           0x2b
#define BOARD_STATUS                          0x01
#define GET_LAST_ERROR                        0x03
#define GET_VERSION                           0x04
#define GET_TELEMETRY                         0x10
#define GET_WATCHDOG                          0x20
#define SET_WATCHDOG                          0x21
#define RESET_WATCHDOG                        0x22
#define ALL_PDMS_ON                           0x40
#define ALL_PDMS_OFF                          0x41
#define GET_PDMS_ACTUAL                       0x42
#define GET_PDMS_EXPECTED                     0x43
#define SET_PDM_ON                            0x50
#define SET_PDM_OFF                           0x51
#define EPS_RESET                             0x80

#define UART                                  Serial
#define I2C                                   Wire
#define I2C_ENABLE                            2
#define VBAT_ENABLE                           3
#define SW1_ENABLE                            4
#define SW2_ENABLE                            5
#define SW3_ENABLE                            6
#define SW4_ENABLE                            7
#define SW5_ENABLE                            9
#define WDT_RESET                             10

#define MAX_I2C_BYTES                         16


/********************************************************************
 *   Setup Object Links/Pointers
 *******************************************************************/
class DSD_EPS
{
  private:
    unsigned long watchdog_limit;
    unsigned long watchdog_last_reset;
    unsigned long watchdog_internal;
    bool reset_eps;
    int pdm_expected;
    int pdm_status;
    int board_status;
    int last_error;
    void gpioHigh(int);
    void gpioLow(int);
    void boardStatus();
    void getLastError();
    void getVersion();
    void getTelemetry();
    void getWatchdog();
    void setWatchdog();
    void resetWatchdog();
    void allPDMsOn();
    void allPDMsOff();
    void getPDMsActual();
    void getPDMsExpected();
    void setPDMOn();
    void setPDMOff();
    void epsReset();

  public:
    bool i2c_receive;
    bool i2c_request;
    byte i2c_data[MAX_I2C_BYTES];
    int bytes;
    DSD_EPS();
    ~DSD_EPS();
    void init();
    void readI2C();
    void readUART();
    void watchdog();
 
};


/********************************************************************
 *   Private GPIO High:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::gpioHigh(int _pin)
{
  digitalWrite(_pin, HIGH);
  delay(1);
}


/********************************************************************
 *   Private GPIO Low:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::gpioLow(int _pin)
{
  digitalWrite(_pin, LOW);
  delay(1);
}


/********************************************************************
 *   Private Board Status:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::boardStatus()
{
  byte buffer_[2];
  buffer_[1] = this->board_status;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}


/********************************************************************
 *   Private Get Last Error:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::getLastError()
{
  byte buffer_[2];
  buffer_[1] = this->last_error;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}


/********************************************************************
 *   Private Get Board Version:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::getVersion()
{
  byte buffer_[4];
  buffer_[3] = 0x00;
  buffer_[2] = 0x00;
  buffer_[1] = 0x03;
  buffer_[0] = 0x01;
  delay(1);
  I2C.write(buffer_, 4);
}


/********************************************************************
 *   Private Get Telemetry:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::getTelemetry()
{
  // This is effectively just temperature using SPI
  byte buffer_[2];
  
  /********************************************************************
   *   IIDIODE_OUT - BCR Output Current (14.662757 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x84))
  {
    buffer_[1] = 0x10;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   VIDIODE_OUT - BCR Output Voltage (0.008993157 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x80))
  {
    buffer_[1] = 0xF3;
    buffer_[0] = 0x06;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   I3V3_DRW - 3V3 Current (0.001327547 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x05))
  {
    buffer_[1] = 0xE2;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   I5V_DRW - 5V Current (0.001327547 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x05))
  {
    buffer_[1] = 0x79;
    buffer_[0] = 0x01;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   IPCM12V - 12V Bus Current (0.00207 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x34))
  {
    buffer_[1] = 0xA9;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   VPCM12V - 12V Bus Voltage (0.01349 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x30))
  {
    buffer_[1] = 0x7A;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   IPCMBATV - BAT Bus Current (0.005237 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x24))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   VPCMBATV - BAT Bus Voltage (0.008978 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x20))
  {
    buffer_[1] = 0x81;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   IPCM5V - 5V Bus Current (0.005237 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x14))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   VPCM5V - 5V Bus Voltage (0.005865 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x10))
  {
    buffer_[1] = 0x55;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   IPCM3V3 - 3V3 Bus Current (0.005237 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x04))
  {
    buffer_[1] = 0x26;
    buffer_[0] = 0x00;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   VPCM3V3 - 3V3 Bus Voltage (0.004311 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE2) && (this->i2c_data[2] == 0x00))
  {
    buffer_[1] = 0xFE;
    buffer_[0] = 0x02;
    delay(1);
    I2C.write(buffer_, 2);
  }
  
  /********************************************************************
   *   TBRD - Temperature Data (0.372434 x ADC Count)
   *******************************************************************/
  if ((this->i2c_data[1] == 0xE3) && (this->i2c_data[2] == 0x08))
  {
    buffer_[1] = 0x16;
    buffer_[0] = 0x03;
    delay(1);
    I2C.write(buffer_, 2);
  }
}


/********************************************************************
 *   Private Get Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::getWatchdog()
{
  byte buffer_[2];
  buffer_[1] = (this->watchdog_limit / 1000.0 / 60.0);
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}


/********************************************************************
 *   Private Set Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::setWatchdog()
{
  this->watchdog_limit = (unsigned long)((int)this->i2c_data[1] * 60.0 * 1000.0);
}


/********************************************************************
 *   Private Reset Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::resetWatchdog()
{
  this->gpioLow(WDT_RESET);
  this->gpioHigh(WDT_RESET);
  this->gpioLow(WDT_RESET);
  this->gpioHigh(WDT_RESET);
}


/********************************************************************
 *   Private Set All PDMs On:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::allPDMsOn()
{
  this->gpioHigh(SW1_ENABLE);
  this->gpioHigh(SW2_ENABLE);
  this->gpioHigh(SW3_ENABLE);
  this->gpioHigh(SW4_ENABLE);
  this->gpioHigh(SW5_ENABLE);
  this->gpioHigh(VBAT_ENABLE);
}


/********************************************************************
 *   Private Set All PDMs Off:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::allPDMsOff()
{
  this->gpioLow(SW1_ENABLE);
  this->gpioLow(SW2_ENABLE);
  this->gpioLow(SW3_ENABLE);
  this->gpioLow(SW4_ENABLE);
  this->gpioLow(SW5_ENABLE);
  this->gpioLow(VBAT_ENABLE);
}


/********************************************************************
 *   Private Get Actual PDM Status:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
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


/********************************************************************
 *   Private Get Expected PDMs:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::getPDMsExpected()
{
  byte buffer_[2];
  buffer_[1] = this->pdm_expected;
  buffer_[0] = 0x00;
  delay(1);
  I2C.write(buffer_, 2);
}


/********************************************************************
 *   Private Set PDM On:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
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


/********************************************************************
 *   Private Set PDM Off:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
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


/********************************************************************
 *   Private Make the EPS Reset Using Watchdog:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::epsReset()
{
  this->reset_eps = true;
}


/********************************************************************
 *   Public Constructor:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   Pointer
 *******************************************************************/
DSD_EPS::DSD_EPS()
{
  // No constructor because arduinos can't handle it
}


/********************************************************************
 *   Public Deconstructor:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   Null Pointer
 *******************************************************************/
DSD_EPS::~DSD_EPS()
{
  // No deconstructor required for this application
}

/********************************************************************
 *   Public Initializer:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::init()
{
  /********************************************************************
   *   Establish Default Status to Keep the EPS ON
   *******************************************************************/
  this->reset_eps = false;
  
  /********************************************************************
   *   Establish Default Pin Modes
   *******************************************************************/
  pinMode(WDT_RESET, OUTPUT);
  pinMode(I2C_ENABLE, OUTPUT);
  pinMode(SW1_ENABLE, OUTPUT);
  pinMode(SW2_ENABLE, OUTPUT);
  pinMode(SW3_ENABLE, OUTPUT);
  pinMode(SW4_ENABLE, OUTPUT);
  pinMode(SW5_ENABLE, OUTPUT);
  pinMode(VBAT_ENABLE, OUTPUT);
  
  /********************************************************************
   *   Establish Default Pin Values
   *******************************************************************/
  this->gpioHigh(WDT_RESET);
  this->gpioLow(I2C_ENABLE);
  this->gpioLow(SW1_ENABLE);
  this->gpioLow(SW2_ENABLE);
  this->gpioLow(SW3_ENABLE);
  this->gpioLow(SW4_ENABLE);
  this->gpioLow(SW5_ENABLE);
  this->gpioLow(VBAT_ENABLE);
  
  /********************************************************************
   *   Reset the Watchdog Timer
   *******************************************************************/
  this->resetWatchdog();
  this->watchdog_limit = (4.0 * 60.0 * 1000.0);
  this->watchdog_last_reset = millis();
  this->watchdog_internal = millis();
  
  /********************************************************************
   *   Place the EPS I2C Circuit on the CSKB Bus
   *******************************************************************/
  this->i2c_receive = false;
  this->i2c_request = false;
  this->gpioHigh(I2C_ENABLE);
  
}


/********************************************************************
 *   Public Read the I2C Bus:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
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


/********************************************************************
 *   Public Read the UART Connection:
 *      Description:  See ICD for API
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void DSD_EPS::readUART()
{
  // Future implementation using UART for command and control
}


/********************************************************************
 *   Public Watchdog:
 *      Description:  Checks status of watchdog elapsed time vs. the
 *                    watchdog maximum time and returns if the value
 *                    has been exceeded
 *      Parameters:   Current system time in milliseconds
 *      Return Val:   None
 *******************************************************************/
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
 

/********************************************************************
 *   Setup Object Links/Pointers
 *******************************************************************/
DSD_EPS eps = DSD_EPS();


/********************************************************************
 *   Main Function
 *******************************************************************/
void setup() 
{
  /********************************************************************
   *   Start Up Communication Interfaces
   *******************************************************************/
  UART.begin(9600);
  I2C.begin(I2C_ADDRESS);
  delay(100);

  /********************************************************************
   *   Establish Default Pin Modes
   *******************************************************************/
  pinMode(WDT_RESET, OUTPUT);
  pinMode(I2C_ENABLE, OUTPUT);
  pinMode(SW1_ENABLE, OUTPUT);
  pinMode(SW2_ENABLE, OUTPUT);
  pinMode(SW3_ENABLE, OUTPUT);
  pinMode(SW4_ENABLE, OUTPUT);
  pinMode(SW5_ENABLE, OUTPUT);
  pinMode(VBAT_ENABLE, OUTPUT);
  
  /********************************************************************
   *   Initialize EPS
   *******************************************************************/
  eps.init();
  
  /********************************************************************
   *   Check for Communication on I2C
   *******************************************************************/
  I2C.onReceive(receiveEvent);
  I2C.onRequest(requestEvent);
}

/********************************************************************
 *   Public Receive Event Trigger:
 *      Description:  Triggers I2C Read
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void receiveEvent(int bytes) 
{ 
  eps.bytes = bytes;
  for (int i = 0; i < bytes; i++)
  {
    if (i < MAX_I2C_BYTES)
    {
      eps.i2c_data[i] = I2C.read();
    }
    else
    {
      I2C.read();
    }
  }

  /********************************************************************
   *   Catch Functions That Don't Provide Talk Back
   *******************************************************************/
  switch(eps.i2c_data[0])
  {
    case  SET_WATCHDOG:
    case  RESET_WATCHDOG:
    case  ALL_PDMS_ON:
    case  ALL_PDMS_OFF:
    case  SET_PDM_ON:
    case  SET_PDM_OFF:
    case  EPS_RESET:
          eps.i2c_receive = true;
          break;
  }
}

/********************************************************************
 *   Public Request Event Trigger:
 *      Description:  Triggers I2C Read
 *      Parameters:   None
 *      Return Val:   None
 *******************************************************************/
void requestEvent() 
{ 
  eps.i2c_request = true;
  eps.readI2C();
}

void loop() 
{
  while (true)
  {
    /********************************************************************
     *   Monitor the Watchdog
     *******************************************************************/
    eps.watchdog();
  }
}
