// This program replaces Dev's original code for the v2.0 controller design

#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>
#include <SX128XLT.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);//https://www.arduino.cc/reference/en/libraries/liquidcrystal-i2c/
SX128XLT LT; 

bool debug = 0;


#include "Vfence.h"
#define SERIAL0_BAUD      9600


void setup() {
  PinInitialize();
  lcd.init(); 
  LCDUpdate(0);

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
  attachInterrupt(digitalPinToInterrupt(CancelButton),CancelFun,LOW);
  attachInterrupt(digitalPinToInterrupt(ShockButton), ResumeFun, LOW);
}
//LOOP/////////////////////////////////////////
void loop() {
  ResumePressed = false;
  Active = !digitalRead(ShockButton) && (Editing < 1);
  if (Active){
    MsgOut = "B" + String(AID)+ "," + String (EXP) +"B";
    Broadcast(MsgOut);
    digitalWrite(RedLED,HIGH);
    while (Active){
      if (CancelPressed){
        CancelShock();
      }
      if (ResumePressed){
         MsgOut = "B" + String(AID)+ "," + String (EXP) +"B";
         Broadcast(MsgOut);
         Paused = false;
         ResumePressed = false;
         if(debug){
          Serial.println("Resumed");
         }
      }
      if (ParseMessage()){  //ParseMessage returns true only if valid message is received from matching AID
        if (BeepCounter==0){
          lcd.setCursor(11, 1);
          lcd.print("C");
          Active = false;
          CancelPressed=false;
          digitalWrite(RedLED,LOW);
        } //Only cancelled feedback (0) or shocked feedback (MaxBeeps+1) turns off Active state
        else if(BeepCounter>0 && BeepCounter <= MaxBeeps){
          lcd.setCursor(11, 1);
          lcd.print(BeepCounter);
        }
        else if(BeepCounter==MaxBeeps+1){
          lcd.setCursor(11, 1);
          ShockCounter++;
          lcd.print("S "+ String(ShockCounter));
          Active = false;
          CancelPressed=false;
          digitalWrite(RedLED,LOW);
        }
        else{}
      }
     
    }
  }
  else{
    if (CancelPressed and Editing <1){ //Case to enter editing mode
      Editing = 1; //Editing AID (field1)
      CancelPressed = false;
      EditSettings();
    }

  }
  
}
////////////////////////////////////

////////////////////////////////////
void CancelShock(){
  int i = 0;
  while(i < 5000){  //runs the cancel command for roughly 1 minute or until a response from the appropriate tag is seen.
    MsgOut = "B" + String(AID)+",0B";
    Broadcast(MsgOut);
    i++;
    if(TagCancel() == true){
      break;
    }
  }
  CancelPressed = false;
  if(Paused == true){
     Paused = false;
     Active = false;
     digitalWrite(GreenLED, HIGH);
     digitalWrite(RedLED, LOW);
  }
  else if(Paused == false){
     Paused = true;
  }
}
////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
 bool ParseMessage(){

  // Will return true only if there is a valid message for this cow, else it will return false.
  // Nothing to do if it returns false, If returned true, 

  //Read buffer
  RXPacketL = LT.receiveIRQ(RXBUFFER, RXBUFFER_SIZE, 500, WAIT_RX); //wait for a packet to arrive with 500ms timeout. More time allows to  cancel message is receive with better likelihood
  
  //Is mesage length > 0?
  bool NoErr = (RXPacketL > 0);
  
  //In no error then Check if RSSI > -100 else return false
  if (NoErr){
    if (debug){
      Serial.print(F("\nMessagex: "));
      LT.printASCIIPacket(RXBUFFER, RXPacketL);
      Serial.print(F(",Length,"));
      Serial.println(RXPacketL);
      Serial.print(F(",RSSI,"));
      Serial.println(LT.readPacketRSSI());
    }
    NoErr = NoErr and  LT.readPacketRSSI() >= -100; //Only entertain message with RSSI > -100 dB
  }
  else{
    //if (debug){Serial.println("No message");}
    return false;
  } 
  
  //If still no error then check if sentence structure is enclosed in <> and RSSI is > -100 dB
  
  if (NoErr){
    if (debug){
      Serial.print(F("First and Last Characters:"));
      Serial.print((char)RXBUFFER[0]);
      Serial.print(",");
      Serial.println((char)RXBUFFER[RXPacketL-1]);
      }
    NoErr = NoErr and  (char)RXBUFFER[0] == 'C' and (char)RXBUFFER[RXPacketL-1] == 'C';
  }
  else{
    if (debug){Serial.println("Message with RSSI < -100dB");}
    return false;
  } 

  //PacketSNR = LT.readPacketSNR();                //read the received packets SNR value
  // If still no error, Parse message and see if it was for this cow, then updpate and return true, else return false
  
  
  if (NoErr){
    if (debug){Serial.println("No Err in message");}
    ////The following code to extract message could be more efficient.
    byte index;
    MsgIn = "";
    //Expected Buffer C AID, Code C So discard the outer Code
    
    for (index = 1; index < RXPacketL-1; index++){ 
      MsgIn += (char)RXBUFFER[index];
    }
    
    if (debug){
      Serial.print("MsgIn:");
      Serial.println(MsgIn);
    }
    // The message <AID,CODE>
    int c1 = MsgIn.indexOf(',');       //Find first comma position
    if (debug){Serial.println("CowID,Code");}
    int CowID = MsgIn.substring(0, c1).toInt(); //Substring counts from 0. NOTE: Strangely returns character up to position of c1-1. C1 is not included 
    int Code = MsgIn.substring(c1+1).toInt();
  
    if (debug){
      Serial.println(CowID);
      Serial.println(Code);
      //Serial.println(CowID);
    }
      
    if(CowID == AID ){
      BeepCounter = Code; 
      return true;
    }
    else{
      if (debug){Serial.println("Message is not for this cow");}
      return false;
      }
    //}
    
  }
  else{
    if (debug){Serial.println("Message has wrong structure");}
    return false;
  } 
  
}
/////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////
void Broadcast(String Msg){
  TXPacketL = Msg.length()+1; 
  byte buff[TXPacketL];
  Msg.toCharArray(buff,TXPacketL);
  
  if(LT.transmitIRQ(buff, TXPacketL-1, 1000, TXpower, WAIT_TX)){ //will return packet length sent if OK, otherwise 0 if transmit error
    //BlinkLED2(100,1);
    if (debug){
    Serial.print("Sent: ");
    Serial.println(Msg);
    }
    
  }
  
}
//////////////

//////////////
bool TagCancel(){
  RXPacketL = LT.receiveIRQ(RXBUFFER, RXBUFFER_SIZE, 500, WAIT_RX);
  bool NoErr = (RXPacketL > 0);
  if(NoErr){
    NoErr = NoErr and LT.readPacketRSSI() >= -100;
  }
  else{return false;}
  if(NoErr){
    byte index;
    MsgIn = "";
    for (index = 1; index < RXPacketL-1; index++){ 
      MsgIn += (char)RXBUFFER[index];
    }
    int c1 = MsgIn.indexOf(',');
    int CowID = MsgIn.substring(0, c1).toInt();
    int Code = MsgIn.substring(c1+1).toInt();
    if (debug){
      Serial.println(CowID);
      Serial.println(Code);
    }
    if(CowID == AID && Code == 0){
      return true;
    }
    else{return false;}
  }
  else{return false;}
}
