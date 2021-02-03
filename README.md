Electronic Power Supply
------------------------------------------------------------------------------------
```

Vendor: Death Star Development, LLC
Date Created: August 1st, 2020
Version: 0.4.0

License: CC BY-NC-ND
Associated Filename: All
Associated Document: 
      https://github.com/craigopie/eps

```
------------------------------------------------------------------------------------
```

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

```
------------------------------------------------------------------------------------

# Overview

### Introduction

This repository provides a low cost Electronic Power Supply (EPS) in a PC-104 form factor that can be assembled by low experience individuals with limited soldering equipment.  The design provides the choice of using an Arduino controller or an ATSAM3X8E 32-bit processor.  The ATSAM3X8E provides more reliable I2C communications.  

### Notes

This version does not have a working WatchDog Timer.

### Features

Supports 8.2 Volt Batteries (2S Cell Lithium Based Chemistry).
* (6) Solar Pannel Connections
* (1) USB-C Connection for Programming
* (1) Arduino Nano Every as the Micro-Controller
* (3) 3V3 Switched Power Supplies (2A Max ea.)
* (2) 5V Switched Power Supplies (2A Max ea.)
* (1) 3V3 Bus Power Supply (2A Max)
* (1) 5V Bus Power Supply (2A Max)
* (1) I2C Bus Interface
* (1) Serial (UART) Interface

# Build

### Hardware

Please review the BOM and populate as required by referencing the schematic.  The 5V bus IC requires the supply and enable pins to be shorted together, using a solder bridge, during manufacturing.

### Firmware

Arduino: Be sure to flash the software and test the functionality prior to soldering on to the EPS.
ATSAM3X8E: Please follow the documentation provided for the Arduino Due programming using the Native Port (USB).