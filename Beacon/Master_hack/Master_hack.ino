/*****************************************************************************************************
  Programs for Arduino - Copyright of the author Stuart Robinson - 16/03/20
  Modified by Dev Shrestha - 04/05/24
    Ranging to SX1280 has follwoing sequence
      1. Master sends Setting requst with a code to radio it wants to range
      2. Slave receives the code, and confirms; then set its radio as slave with settings and wait up to 5 seconds to get ranging request
          If none received, slave will go to regular cummunication mode
      3. If confirmation code matches, Master changes its own settings and send range request
         
      4. If successful ranging received, it writes to memroty and switch to slave mode
      5. Slave and Master switches role and ranges again
         Remore radio tries to range up to 5 times if ranging is unsuccessful. then switches to comm mode.
         If ranging successulf Slve Broadcast the result
      5. After successful ranging, Master switches to comm mode and listens to broadcast message
      6  Records the boradcast message
      7. Repeats 1 - 6 with different code for same radio
      8. Once all settings are done, switches to next radio      
 

  This program is supplied as is, it is up to the user of the program to decide if the program is
  suitable for the intended purpose and free from errors.
*******************************************************************************************************/

#include <SPI.h>
#include <EEPROM.h>
#include "Settings.h"

// Set Station ID
uint32_t MyID = 0; 
uint32_t CommanderID = 10; 
String NanoID = "000000000C00";              //Nanotron ID  
uint8_t Stations = 1;                       //Number of Stations to query




//Quick Settings
                         // 0 of Master, 1..Stations for slaves
uint32_t RangingAddress = Stations;          //must match address in recever

bool debug = false;                          //Debug Message ON
bool Tweet = false;                         //Broadcast instead of serial out
bool Record = false;                        //Record to memory 
bool GoRange = true;
    

//UDF definitions

//General
void PinInitialize(); //Initializes pins
void Rmessage();      //Start Ranging message with time 
bool CheckSerial();   // This function returns true if there is a serial messge within a pair of (). The message inside () is writtine to MsgIn
bool ParseMsgxyz(String Msg);    // Parse MessageIn whenCalled For and splits into Who this message for, and the Message Code. If Message for MyID, execute
void ParseCode(uint8_t Code);  //Converts Code into action

void led_Flash(uint16_t flashes, uint16_t delaymS);
void led_code(uint8_t x);
void led_Code2(uint8_t x);

//Nanotron
void NanotronReset();
String NtRange();

//Global variables
char CharIn;
String MsgOut;
String MsgIn;
String SerialIn;
uint8_t MsgCode = 0;
//uint8_t SerialCode = 0;
uint32_t Sender = 0;    //Either sender if from radio to ToID if from Serial Port

uint8_t d = 50; //Standary delay time in ms
uint32_t response_sent;
uint8_t RadioMode = 0; //Listen mode by default

// Ping Variables
uint8_t buff[] = "Hello World";
uint16_t PayloadCRC;
uint8_t TXPacketL;
#define ACKtimeout 300                         //Acknowledge timeout in mS                      
#define TXtimeout 100                          //transmit timeout in mS. If 0 return from transmit function after send.  
#define TXattempts 1                          //number of times to attempt to TX and get an Ack before failing  

//Pong Variables
const uint8_t RXBUFFER_SIZE = 251;              //RX buffer size, set to max payload length of 251, or maximum expected length
uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into
#define ACKdelay 100                            //delay in mS before sending acknowledge                    
#define RXtimeout 300                         //receive timeout in mS.   
uint8_t RXPacketL;                              //stores length of packet received
uint8_t RXPayloadL;                             //stores length of payload received
uint8_t PacketOK;                               //set to > 0 if packetOK
int16_t PacketRSSI;                             //stores RSSI of received packet
uint16_t LocalPayloadCRC;                       //locally calculated CRC of payload
uint16_t RXPayloadCRC;                          //CRC of payload received in packet
uint16_t TransmitterNetworkID;                  //the NetworkID from the transmitted and received packet
///////////////////////////////////////////////////////////////////////////////////// 

#define SHOCK 6
#define BOOSTEN 2  //shock charge enable
#define BUZZER A2
int radius = 300;
bool shocked = false;
int Duration = 40;
int Warnings = 0;
                         
void loop()
{
  
  // 1. Range to each radio in sequence from 1 to Stations 
  if(GoRange){    
    String nanoMessage = NtRange();//Range using Nanotron
    int nanoDistance = nanoMessage.substring(8,14).toInt();

    if(nanoDistance < radius && nanoDistance != 0){
      Serial.println("Shocked");
      actionShock();
      shocked = true;
      led_code(7);
    }
    else if( nanoDistance > radius && shocked == true){
      Serial.println("Reset");
      actionReset();
      shocked = false;
      led_Code2(7);
    }
    Serial.println();

   delay(1000);
  
  
 
    
  }
}

void setup()
{
  Serial.begin(115200);            //setup Serial 
  Serial2.begin(115200);
  //Inilize pins as output or input
  PinInitialize();
  NanotronReset();   //Reset Nanotron radio
  led_code(7);
  delay(1000);
}

void actionShock(){

  if(Warnings > 6){
    return;
  }
   
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);

  if (Warnings > 1){
    digitalWrite(BOOSTEN, HIGH);
    delay(10);
    digitalWrite(SHOCK, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(Duration);                // wait Duration
    digitalWrite(SHOCK, LOW);    // turn the LED off by making the voltage LOW
    digitalWrite(BOOSTEN, LOW);
  }
  Warnings += 1;
  
}

void actionReset(){
  Warnings = 0;
}


//UDFs
//Initializes pins
void PinInitialize(){
  // Set pins as output or input
  pinMode(LED1, OUTPUT);
  pinMode(NANORST, OUTPUT);      
  digitalWrite(NANORST,HIGH);
  if(debug){Serial.println("Pin Initialized.");}

}





void NanotronReset(){
  //Setup Nanotron radios
  if(debug){Serial.println("Starting Nanotron Reset.");}
  Serial.println("BRAR 0"); 
  Serial2.println("BRAR 0");  //Do not broadcast ranging results (Ranging is performed by Beacon)
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  Serial.println("EBID 0"); 
  Serial2.println("EBID 0"); 
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  Serial.println("SNID " + NanoID);
  Serial2.println("SNID " + NanoID);
  delay(d); 
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  Serial.println("GNID");
  Serial2.println("GNID"); 
  delay(d);
    while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  Serial.println("GPIO 0 0 0 0 2");
  Serial2.println("GPIO 0 0 0 0 2"); 
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }

  Serial.println("GPIO 1 0 0 0 2");
  Serial2.println("GPIO 1 0 0 0 2"); 
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  Serial.println("GPIO 2 0 0 0 2");
  Serial2.println("GPIO 2 0 0 0 2"); 
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }

  Serial.println("GPIO 3 0 0 0 2");
  Serial2.println("GPIO 3 0 0 0 2"); 
  delay(d);
  while (Serial2.available() > 0){ 
     Serial.write(Serial2.read());
  }
  
  //Serial2.println("SSET"); //Save to permanent address 
  
  if(debug){Serial.println("Nanotron Reset done.");}
}

String NtRange(){
  //Create Line 2 = "N,0,range in cm, RSSI,Temperature, Battery"
  //if(debug){Serial.println("Starting N-message.");}
  MsgOut = "N,";
  MsgOut += MyID;
  MsgOut += ",";
  MsgOut += RangingAddress;
  MsgOut += ",";
  String Rto =  "RATO 0 000000000B0" + String(RangingAddress);
  Serial2.println(Rto); //Returns "=Code,Distance in cm,RSSI \n\r"
  if(debug){Serial.println("Ranging to: " + Rto);}
  delay(d);

  while (Serial2.available() > 0)
  { 
    CharIn = Serial2.read();
    if(debug){Serial.print(CharIn);}
    if (CharIn != '=' &&  CharIn != '\r' && CharIn != '\n' ){MsgOut += CharIn;  }    //Strip away =,/n and/r
  }
  
  //Get temperature and battery
  Serial2.println("GMYT"); //Returns "= Temperature in °C \n\r"
  delay(d);
  MsgOut += ",";
  while (Serial2.available() > 0){ 
    CharIn = Serial2.read();
    if (CharIn != '=' &&  CharIn != '\r' && CharIn != '\n' ){MsgOut += CharIn;  } 
  }
  Serial2.println("GBAT"); // Returns "= Battery voltage like 33 for 3.3V \n\r"
  delay(d);
  MsgOut += ",";
  while (Serial2.available() > 0){ 
    CharIn = Serial2.read();
    if (CharIn != '=' ){MsgOut += CharIn;  }     
  }
   return MsgOut;
  //if(debug){Serial.println("N-message done.");}
}


void led_code(uint8_t x){
  digitalWrite(LED1,bitRead(x,0));

  /*
  Code 0 = Idle
  1 = Sx1280 in COMM Mode
  2 = Sx1280 Ranging Master
  3 = Sx1280 Ranging Slave 
  4 = Flash busy
  5 = Sx1280 Busy
  
  */

}
void led_Code2(uint8_t x){
  digitalWrite(LED1,bitRead(x,0));

}
