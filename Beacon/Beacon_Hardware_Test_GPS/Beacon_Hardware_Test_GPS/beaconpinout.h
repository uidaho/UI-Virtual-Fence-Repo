#define LED1 30 //defined as output in beaconinit
#define LED2 31//defined as output in beaconinit
#define LED3 32//defined as output in beaconinit
#define LED4 33//defined as output in beaconinit
#define LED5 34//defined as output in beaconinit

#define SX1280LED 8
#define SX1280CS 10
#define SX1280RST 9
#define SX1280BUSY 7
#define SX1280DIO1 5

#define FLASHCS 21

#define PORTPOWER 22//defined as output in beaconinit
#define PORTCOM1 3
#define PORTCOM2 4

#define BUZZER 20

#define RELAYIN1 18//defined as input in beaconinit
#define RELAYIN2 19//defined as input in beaconinit

#define RELAYOUT1 14//defined as output in beaconinit
#define RELAYOUT2 15//defined as output in beaconinit

#define NANOTX 17
#define NANORX 16
#define NANOBAUD 115200

#define BEACONBAUD 115200

#define ANALOGAUX1 A8
#define ANALOGAUX2 A9
#define ANALOGAUX3 A10
#define ANALOGAUX4 A11
#define ANALOGAUX5 A12

//Copied from old GPS firmware:
#define LED_PIN (5)
#define RXPin (4)
#define TXPin (3)


#define GPSPOWER PORTPOWER
#define SDCHIPSELECT (8)
#define GREENLED (LED1)
#define REDLED (LED2)
#define GPSBaud 9600