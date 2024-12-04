/*
This file provides a pinout for the virtual fence tag usage by the software based on the schematic designs
current hardware uses an ATMEGA 328PB and a nanotron radio for ranging
Last Edited: 7/22/24
By: Andrew Carefoot, Jacob Karl
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
#define NANOUSARTRX 17  //serial communication from nanotron
#define NANOUSARTTX 16  //serial communication to nanotron
#define NANOENABLE 2

//ftdi hardware serial comms
#define FTDIRX 0  //recieving from ftdi comms
#define FTDITX 1  //sending to ftdi

//GPS pins
#define GPSRX 3
#define GPSTX 4

//shock circuit pins
#define SHOCK 30
#define BOOSTEN 31  //shock charge enable
#define BUZZER 32

//baudrate definitions
#define NANOBAUD 115200
#define TAGBAUD 115200

int warnings = 0; //the controling component of tag mechanics. Our behavior changes depending on how manny of these we have.
bool on_network = true; //control variable that detemines whether or not we perform ranging or searching
int sleep_time = 490; //how long we sleep for in mS
int encryption_key[32]; //the 32-bit encryption key we use to decode messages. Empty by default, set when we recieve an encryption message
int transmission_decay = 0; //counts the number of times we listen for a message but don't recieve one. Once we reach a threshold set on_network to false
int timeout = 500; //how long we should wait to recieve a message in mS
