/*
This file provides a pinout for the virtual fence tag usage by the software based on the schematic designs
current hardware uses an ATMEGA 328PB and a nanotron radio for ranging
Edited: 5 Jun 24
By: Andrew Carefoot
*/
//onboard led
#define LED1 A0

//flash SPI comms
#define FLASHCS 5
#define FLASHMISO 12
#define FLASHMOSI 11
#define FLASHSCK 13

//nanotron communication pins
#define NANORST 10
#define NANOUSARTRX 7  //serial communication from nanotron
#define NANOUSARTTX 8  //serial communication to nanotron
#define NANOENABLE A3

//ftdi hardware serial comms
#define FTDIRX 0  //recieving from ftdi comms
#define FTDITX 1  //sending to ftdi

//GPS pins
#define GPSRX 3
#define GPSTX 4

//shock circuit pins
#define SHOCK 6
#define BOOSTEN 2  //shock charge enable
#define BUZZER A2

//baudrate definitions
#define NANOBAUD 115200
#define TAGBAUD 115200
