#ifndef Cowx_h
#define Cowx_h
//*************************************************************
// PIN settings
//*************************************************************
//
#include <SPI.h>
#include <SX128XLT.h> 
 
//SPI pins
#define NSS 10					// SS pin for SX1280 (SX1280 E28-2G4M12S) Radoo
#define PSS A0					// SS pin for Potentiometer MCP41X2-104
#define FSS A3					// SS pin for Flash memory

// Other Radio Pins
#define RFBUSY 7				//Radio Rfbusy pin (Stays high if there is any error in chip including solding shorts)
#define NRESET 9				//Rest pin 
#define DIO1 5
#define DIO2 -1                 //not used 
#define DIO3 -1                 //not used
#define RX_EN -1                //pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1                //pin for TX enable, used on some SX1280 devices, set to -1 if not used 
#define NMOSI 11				//Transmitt from Radio
#define NMISO 12				//Receive from Radio

//Potentiometer Pins

#define PMOSI 11				//Write Potentiometer value
#define PMISO 12				//Read Potentiometer value


// Flash Pins
#define FMOSI 11				//Write to flash memory
#define FMISO 12				//Read from flash memory

// GPS Pins
#define GBE A0					//Boost Enable
#define GRx 3					//Arduino to GPS
#define GTx 4					//GPS to Arduino

//Other pins
#define SHOCK 6					//Controls IRL 540 switch
#define CHARGE 2
#define BUZZER A2				//Controls Buzzer pin high
#define LED1 A7					//Controls LED 1 (need a jumper)
#define LED2 A1					//Controls LED 2 
#define LED3 8					//Controls LED 3
#define LED4 A6					//Controls LED 4 (need a jumper)

//Radio settings from Stuart's code
#define LORA_DEVICE DEVICE_SX1280               //we need to define the device we are using
#define RXBUFFER_SIZE 255                       //RX buffer size
uint32_t RXpacketCount;
uint32_t errors;

uint8_t RXBUFFER[RXBUFFER_SIZE];                //create the buffer that received packets are copied into

uint8_t RXPacketL;                              //stores length of packet received
int16_t PacketRSSI;                             //stores RSSI of received packet
int8_t  PacketSNR;                              //stores signal to noise ratio (SNR) of received packet

#define TXpower 10                              //LoRa transmit power in dBm
uint8_t TXPacketL;
/////////////////////////////////////
// Constants
int Duration = 40;    //milli seconds
byte SIntensity = 1;  //Shiock Intensity (1,2 or 3)
byte MaxBeeps = 5;    //Shock after this many beeps
int MaxShocks = 999;   //No more than this many schocks
int  Freq = 800;      //Buzzer Frequency
int  Duty = 1000;     //Buzzer Duration
int  Duty2 = 100;      //Buzzer duration for the speed change
SX128XLT LT;    //LT is SX1280 object

// Useful Variables that program changes internally
int EXP = 0;    // Experiment #
int Beeped = 0; //Beep Counter
bool Cancelled = true;
int ShockCounter = 1;

//Program Values
String MsgIn = "";
String MsgOut = "";
byte PotR = 0;
unsigned long Timer  = 0;
unsigned long TimerShock = 0;



void setPot(byte m, byte n)
{
  n = max(1,n); // To prevent from too low resistance, which will burn out the POT
  m = m<<4; 
  // Accoding to MCP41X2 manual, Memory address is first 4 bits, so must be shifted 4 bits to left. 
  // Next two bits is 00 to write command so no change required
  // Next bit is ignored
  // Last bit is 1 for value = 256. Normally 0 for wiper position from 0 to 255
  digitalWrite(PSS, LOW); //Activate the POT  
  //SPI.transfer(b); 
  SPI.transfer(m);  // send address and command byte
  SPI.transfer(n); // send value (0~255)
  digitalWrite(PSS, HIGH); //Deactivate the POT 

}

word getPot(byte m)
{
digitalWrite(PSS, LOW);
  m = m<<4;
  m = m +12;
  //Accoding to MCP41X2 manual, Memory address is first 4 bits, so must be shifted 4 bits to left. 
  // Next two bits is 11 to read. 1100b = 12d.
  // Anything after 11 is ignored, the chip will read the and send the register value after 7th bit
  
  byte x1 = SPI.transfer(m); 
  // send command byte to write. 
  //x1 = 1111 11 (First six bits are always 1), 7th bit is 1 if valid address or 0 if error, 
  // 8th bits is the MSB of the 9 bit value
  
  byte x2 = SPI.transfer(0x00);//the tranfer value 0x00 is ignored as as the command is read
  //x2 is the actual value if last bit of x1 is 0, otherwise data = 256
  word x = x2;//<<7;
  //x = x+x2;
  digitalWrite(PSS, HIGH);
  return x;

}

void PinInitialize(){
//First thing first
  //Drive shock pin low to avoid shocking cow my mistake
  pinMode(SHOCK, OUTPUT);
  digitalWrite(SHOCK, LOW); //First thing to do is disble shock pin
  //Set other pins
  
  pinMode(FSS, OUTPUT);
  pinMode(NSS, OUTPUT); 
  pinMode(PSS, OUTPUT);
  //setup pin as output for indicator LED
  digitalWrite(FSS, HIGH);
  digitalWrite(NSS, HIGH);
  digitalWrite(PSS, HIGH);  
  pinMode(NRESET, OUTPUT);

}




/////////////////////////////////////////////////////////////////////////////////////////////
void SendConfirmation(String Msg){
    TXPacketL = Msg.length() + 1;
    byte buff[TXPacketL];
    Msg.toCharArray(buff,TXPacketL);
    // Transmit back
    //TXPacketL = sizeof(buff); 
    delay(1000); //Delay Needed for the Beacon as it has just sent a message and may not be ready to receive.
    if(LT.transmitIRQ(buff, TXPacketL-1, 500, TXpower, WAIT_TX))   //will return packet length sent if OK, otherwise 0 if transmit error
    {
     if (debug){
      Serial.print(F("Sent: "));
      Serial.println(Msg);}
     
    }
 }
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////

 void RecordEvent(){}
/////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////
 bool ParseMessage(){

   // Will return true only if there is a valid message for this cow, else it will return false.
   // Nothing to do if it returns false
  
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
      NoErr = NoErr and  LT.readPacketRSSI() >= -100;
      }
     else{
      //if (debug){Serial.println("No message");}
      return false;
     } 
    //In no error then check if sentence structure is valid else return false
    
    if (NoErr){
      if (debug){
        Serial.print(F("First and Last Characters:"));
        Serial.print((char)RXBUFFER[0]);
        Serial.print(",");
        Serial.println((char)RXBUFFER[RXPacketL-1]);
      }
      NoErr = NoErr and  (char)RXBUFFER[0] == 'B' and (char)RXBUFFER[RXPacketL-1] == 'B'; //Only listen for beacon messages
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
      //Expected Buffer (Duration in ms, SIntensity, CowID, ChkSum) So discard the outer bracket
      
      for (index = 1; index < RXPacketL-1; index++){
        MsgIn += (char)RXBUFFER[index];
      }
      
      if (debug){
        Serial.print("MsgIn:");
        Serial.println(MsgIn);
        }
      // The message
      int c1 = MsgIn.indexOf(',');       //Find first comma position
      //int c2 = MsgIn.indexOf(',', c1+1); //Find second comma position
      //int c3 = MsgIn.indexOf(',', c2+1); //Find third comma position 
      if (debug){Serial.println("CowID,EXP");}
      int CowID = MsgIn.substring(0, c1).toInt(); //Substring counts from 0. NOTE: Strangely returns character up to position of c1-1. C1 is not included
      //int lStrength = MsgIn.substring(c1+1, c2).toInt();
      //int lTreatment = MsgIn.substring(c2+1, c3).toInt();      
      int lTreatment = MsgIn.substring(c1+1).toInt();
    
       if (debug){
        Serial.println(CowID);
        Serial.println(lTreatment);
                
       }
       
       if(CowID == MyID ){
          
          if(lTreatment == 0 ){ 
            Cancelled = true;
          
          }
          else if(lTreatment > 0 and lTreatment < 4){ //EXP is between 1 and 3
            Cancelled = false;
            EXP = lTreatment; 
          }
          
         return true;
        }
      else{
        if (debug){Serial.println("Message is not for this cow");}
        return false;
       }
      //}
      
  }else{
      if (debug){Serial.println("Message has wrong structure");}
      return false;
     } 
  
 }
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
 void Beep(){  //Duration in ms
   if(EXP == 1){
      tone(BUZZER, Freq, Duty);
      Beeped++;
      MsgOut = "C" + String(MyID)+ "," + String(Beeped) +"C";
      SendConfirmation(MsgOut);
      RecordEvent();
      if (debug){Serial.println(MsgOut);}
      Timer = millis()+2000;  //placed to prevent a beep loop
   }
   else if(EXP == 2){  //This code block is written inefficiently on purpose to maintain timing constraints.
      for(unsigned long interval = 400; interval > 280; interval -= 20){
         tone(BUZZER, Freq, Duty2);
         delay(interval);
      }
      ParseMessage();
      if(!Cancelled){
         for(unsigned long interval = 280; interval > 160; interval -= 20){
            tone(BUZZER, Freq, Duty2);
            delay(interval);
         }
         ParseMessage();
      }
      if(!Cancelled){
         for(unsigned long interval = 160; interval > 0; interval -= 20){
            tone(BUZZER, Freq, Duty2);
            delay(interval);
         }
      }
      Beeped++;
      MsgOut = "C" + String(MyID)+ "," + String(Beeped) +"C";
      SendConfirmation(MsgOut);
      RecordEvent();
      if (debug){Serial.println(MsgOut);}
      Timer = millis()+500;  //placed to allow time for cancel
   }
   else if(EXP == 3){
      for(int frequency = 400; frequency <= 600; frequency += 20){
         tone(BUZZER, frequency, Duty2);
         delay(Duty2);
      }
      ParseMessage();
      if(!Cancelled){
         for(int frequency = 600; frequency <= 800; frequency += 20){
            tone(BUZZER, frequency, Duty2);
            delay(Duty2);
         }
         ParseMessage();
      }
      if(!Cancelled){
         for(int frequency = 800; frequency <= 1000; frequency += 20){
            tone(BUZZER, frequency, Duty2);
            delay(Duty2);
         }
      }
      Beeped++;
      MsgOut = "C" + String(MyID)+ "," + String(Beeped) +"C";
      SendConfirmation(MsgOut);
      RecordEvent();
      if (debug){Serial.println(MsgOut);}
      Timer = millis()+500;  //placed to allow time for cancel
   }
}
/////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
void Shock(){
  //Set Potentiometer
  byte PotR = 10 - 2*SIntensity;
  word PotVal = getPot(0);
  while (PotVal!= PotR){
  setPot(0, PotR);
  if (debug){
      Serial.print(F("Set Pot: "));
      Serial.println(PotR);
      Serial.print(F("Get Pot: "));
      Serial.println(PotVal);
      delay(1000);
  }
    delay(10);
    PotVal = getPot(0);
  }
  digitalWrite(CHARGE, HIGH);
  delay(10);
  digitalWrite(SHOCK, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(Duration);                // wait Duration
  digitalWrite(SHOCK, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(CHARGE, LOW);
  Timer = millis()+5000;
  TimerShock = millis()+10000;   // This is here so the board doesnt get caught in a shock loop
  //Beeped++;
  //MsgOut = "C" + String(MyID)+ "," + String(Beeped)+ "C";
  //SendConfirmation(MsgOut);
  if (debug){
       Serial.println(MsgOut);
   }
  RecordEvent();
  ShockCounter++;
  if(Beeped >= MaxBeeps){
     Beeped++;
     MsgOut = "C" + String(MyID)+ "," + String(Beeped)+ "C";
     SendConfirmation(MsgOut);
     Beeped = 0;
     Cancelled = true;
  }    
 }  
/////////////////////////////////////////////////////////////////////////////////////////////

#endif
