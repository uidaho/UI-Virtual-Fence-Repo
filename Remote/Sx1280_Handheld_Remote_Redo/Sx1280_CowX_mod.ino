//Dev Shrestha, Andrew Carefoot
//Date Modified 6/16/2023
// This is a working Code
//Change MyID to match cow tag before downloading
// Set debug to 0
int MyID = 5;     //This is COW ID. Match before download
bool debug = 1;  //Turn = 1 for berbose output



/////////////////////////////
#include "CowX.h"
#define SERIAL0_BAUD      9600


void setup() {
  PinInitialize();
  
  Serial.begin(SERIAL0_BAUD);
  while (!Serial);//Wait until serial port is ready
  
  if (debug){Serial.print("Pin Initialized");}
  
  SPI.begin(); // No checking necessary
  
  
  while (! LT.begin(NSS, NRESET, RFBUSY, LORA_DEVICE))  {
    Serial.println(F("Sx1280 not found. Check Connections"));
 
    delay(1000);
    }
  if (debug){Serial.println(F("Sx1280 found"));}


  LT.setupLoRa(2445000000, 0, LORA_SF7, LORA_BW_0400, LORA_CR_4_5);      //configure frequency and LoRa settings
  if (debug){Serial.println(F("Radio initialized at: 2.445 GHz, SF07"));}
  
  MsgOut = "C" + String(MyID)+ "," + String(MaxBeeps+2)+"C";
  SendConfirmation(MsgOut);
   
}

void loop() {
 if (ParseMessage() or !Cancelled){  //If there is a new message or not in Cancelled state. Cancelled is also updated in Parsemessage.
    if (Cancelled){
      if (debug){Serial.println(F("Shock cancelled! "));}
           
      MsgOut = "C" + String(MyID) + ",0C";
      SendConfirmation(MsgOut);
      Beeped = 0;
      RecordEvent();
      
      }
    else{
      if(ShockCounter <= MaxShocks and millis() > Timer){
        if (Beeped <= MaxBeeps){  //Need to use >= becasue beep is reset to 0 and increments only after beeping
          Beep();
        }
      }
       else if(TimerShock < millis()){
           Shock();
        }
      }      
    }
}



//LOOP/////////////////////////////////////////

    
 
