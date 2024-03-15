# About

Beacons are essentially just arduino megas with supporting hardware built in.

# Features

The V1/V2 beacons have a few notable features:

- Nanotron radio built in with external antenna
- SX1280 radio integrated using an on board antenna
- 128mBit of flash memory on board
- Audio Buzzer
- GPS friendly header
- on board serial communication

# Setup
## Driver installation

Before beginning, please confirm the proper drivers are installed for your ICSP programmer device. Skip the next section if you are confident yours are ok.

Programmers such as these:

https://www.amazon.com/KeeYees-Downloader-Programmer-Adapter-Microcontroller/dp/B0885RKVMC/ref=sr_1_5?sr=8-5

Are cheap and typically work quite well, though as we have learned recently there is no garuntee they'll always work. Unfortunately they seem to have a higher than average DOA rate, but for their cost it works out. There are signs yours is DOA mentioned later.

Stop gap in documentation for now, this link does a great job getting this running:

https://www.instructables.com/USBASP-Installation-in-Windows-10/


## Burning the bootloader

Before you can do anything useful with the beacon hardware you'll need to burn the bootloader via ICSP. At this stage it is helpful to place the voltage presence jumper. This allows you to determine the presence of proper working voltage, ensuring proper connection of the programmer. 

To burn the bootloader first open the arduino IDE.
Once loaded it is not necesarry to open a sketch.
Go to Tools->Board->AVR->Arduino Mega or Mega2560
Go to Tools->Processor->Mega2560
Go to Tools->Programmer->USBASP

Lastly, go to Tools and click burn boot loader.

This should take 30 seconds to a minute, then the IDE will display the message "Done burning bootloader"

Once you see this you'll be able to program the the USB port, and utilize it for serial communication.

# Usage

After configuring the device you can plug in via usb and upload from the arduino as if the device was an Arduino Mega 2560.

A beaconpoinout.h file is provided to help users get up and running with the device.
