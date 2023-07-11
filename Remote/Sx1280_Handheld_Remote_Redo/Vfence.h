#ifndef V_Settings_h
#define V_Settings_h
//*************************************************************
// PIN settings
//*************************************************************
//
 
//SPI pins
#define NSS 10					// SS pin for SX1280 (SX1280 E28-2G4M12S) Radio

// Other Radio Pins
#define RFBUSY 7				//Radio Rfbusy pin (Stays high if there is any error in chip including soldering shorts)
#define NRESET 9				//Reset pin 
#define DIO1 5
#define DIO2 -1         //not used 
#define DIO3 -1         //not used
#define RX_EN -1        //pin for RX enable, used on some SX1280 devices, set to -1 if not used
#define TX_EN -1        //pin for TX enable, used on some SX1280 devices, set to -1 if not used 
#define NMOSI 11				//Transmit from Radio
#define NMISO 12				//Receive from Radio

//Digital Potentiometer Pins

#define PMOSI 11				//Write Potentiometer value
#define PMISO 12				//Read Potentiometer value


// Flash Pins
#define FMOSI 11				//Write to flash memory
#define FMISO 12				//Read from flash memory

//Remote Controller Pins

//I2C LCD A4 SDA and A5 SCL
#define BlueLED 4        //Edit LED
#define RedLED 6         //In progress LED
#define GreenLED 8       //Ready LED
#define ShockButton 3    //Button to start expirement (interrupts are pins 2 & 3)
#define CancelButton 2   //Button for cancelling and editing

//D-pad Buttons
#define UpButton A0
#define RightButton A1
#define DownButton A2
#define LeftButton A3

// Variable declarations
byte AID = 1;
byte EXP = 1;
int MaxBeeps = 5;
int BeepCounter = MaxBeeps+2;
int SDuration = 20;
byte SIntensity = 1;
int ShockCounter = 0;

String getInterval(int x){
  if (x == 1){return "2.0";}
  else if (x == 2){return "3.5";}
  else {return "5.0";}
}

//Memory Variables
unsigned long Last_Address = 0;
unsigned long counter = 0;

//Temporary Varaibles
String MsgIn = "";
String MsgOut = "";
String cmd = "";
byte Editing = 0;
bool Active = false;  //True is shock button is pressed until it is cancelled or 5 shocks have occured
bool CancelPressed = false;
bool Paused = false;
bool ResumePressed = false;


long debouncing_time = 15; //Debouncing Time in Milliseconds
volatile unsigned long last_micros;

byte PotR = 0;
int NewPotR = 0;



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

void PinInitialize(){
  //Trun off Radio
  pinMode(NSS, OUTPUT); 
  pinMode(NRESET, OUTPUT);
  digitalWrite(NSS, HIGH);
  
  pinMode(RedLED, OUTPUT); 
  digitalWrite(RedLED, LOW);
  pinMode(GreenLED, OUTPUT); 
  digitalWrite(GreenLED, HIGH);
  pinMode(BlueLED, OUTPUT);
  digitalWrite(BlueLED, LOW);

  pinMode(UpButton, INPUT_PULLUP);
  pinMode(RightButton, INPUT_PULLUP);
  pinMode(DownButton, INPUT_PULLUP);
  pinMode(LeftButton, INPUT_PULLUP);
  
  pinMode(CancelButton, INPUT_PULLUP); 
  pinMode(ShockButton, INPUT_PULLUP); 
  if (EEPROM.read(0) == 0xBE ){
    EEPROM.get(1,AID);
    EEPROM.get(2,EXP);
  }
  
}

void CancelFun(){ //Interrupt Handler
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200) {
   CancelPressed = true;
  }
  last_interrupt_time = interrupt_time;
}

void ResumeFun(){
   static unsigned long last_interrupt_time = 0;
   unsigned long interrupt_time = millis();
   // If interrupts come faster than 200ms, assume it's a bounce and ignore
   if (interrupt_time - last_interrupt_time > 200) {
      ResumePressed = true;
   }
   last_interrupt_time = interrupt_time;
}
//Update LCD
void LCDUpdate(byte x){
  if (x==0){
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AID: ");
  lcd.setCursor(5, 0);
  lcd.print(AID);
  lcd.setCursor(8, 0);
  lcd.print("EXP: ");
  lcd.setCursor(13, 0);
  lcd.print(EXP);
  lcd.setCursor(0, 1);
  lcd.print(MaxBeeps);
  lcd.print(",");
  lcd.setCursor(2, 1);
  lcd.print(SDuration);
  lcd.print(",");
  lcd.print(SIntensity);
  lcd.print(",");
  lcd.setCursor(7, 1);
  lcd.print(getInterval(EXP));
  }
  if (x==1){
    lcd.setCursor(5, 0);
    lcd.print("  ");
    lcd.setCursor(5, 0);
    lcd.print(AID);
  }  
  if (x==2){
    lcd.setCursor(13, 0);
    lcd.print(EXP);
    lcd.setCursor(7, 1);
    lcd.print(getInterval(EXP));
  }
  
}
//MArk the next update AID or EXP with = sign
void LCDMark(byte x){
  if (x==1){
    lcd.setCursor(4, 0);
    lcd.print("=");
    lcd.setCursor(12, 0);
    lcd.print(" ");
  }
  else if(x==2){
    lcd.setCursor(4, 0);
    lcd.print(" ");
    lcd.setCursor(12, 0);
    lcd.print("=");  
  }
  else{
    lcd.setCursor(4, 0);
    lcd.print(" ");
    lcd.setCursor(12, 0);
    lcd.print(" ");
  }  
  
}

void Increment(byte x){
  if (x==1){ AID++; }
  else if(x==2){EXP = min(EXP + 1,3);} 
  LCDUpdate(x);
}
void Decrement(byte x){
  if (x==1){AID = max(AID-1,1);} 
  else if(x==2){EXP=max(EXP - 1,1);}
  LCDUpdate(x);
}


void EditSettings(){
  if(debug){Serial.println("Inside Editing");}
  LCDMark(Editing);
  digitalWrite(GreenLED,LOW);
  if(debug) {Serial.println("Green off");}
  digitalWrite(BlueLED,HIGH); //Entering editing mode
  if(debug){Serial.println("Blue on");}
  unsigned int long last_pressed_right = 0;
  unsigned int long last_pressed_left = 0;
  unsigned int long last_pressed_up = 0;
  unsigned int long last_pressed_down = 0;

  while (CancelPressed == false){
    if(digitalRead(RightButton) == LOW){
      //JoyRight = false;
      if(debug){
        Serial.println("right:");
      }
      int long pressed_right = millis();
      if(pressed_right - last_pressed_right > 200){
      Editing = min(2,Editing+1);
      LCDMark(Editing);
      delay(100);
      }
      last_pressed_right = pressed_right;
    } // Maximum editing is 2
    else if(digitalRead(LeftButton) == LOW){
      //JoyLeft = false;
      if(debug){
        Serial.println("Left:");
      }
      int long pressed_left = millis();
      if(pressed_left - last_pressed_left > 200){
      Editing =max(1,Editing-1);
      LCDMark(Editing);
      delay(100);
      }
      last_pressed_left = pressed_left;
    } // Minimum editing is 1
    else if(digitalRead(UpButton) == LOW) {
      //JoyUp = false;
      if(debug){
        Serial.println("up:");
      }
      int long pressed_up = millis();
      if(pressed_up - last_pressed_up > 200){
      Increment(Editing);
      delay(100);
      }
      last_pressed_up = pressed_up;
    } //Increment the setting based on Editing
    else if(digitalRead(DownButton) == LOW) {
      //JoyDown = false;
      if(debug){
        Serial.println("down:");
      }
      int long pressed_down = millis();
      if(pressed_down - last_pressed_down > 200){
      Decrement(Editing);
      delay(100);
      }
    }
    else{}
  }
  digitalWrite(BlueLED,LOW); //Exiting editing mode
  if(debug){Serial.println("Blue off");}
  delay(50);
  CancelPressed = false;
  Editing = 0;
  LCDMark(Editing);

  EEPROM.write(0,0xBE);
  EEPROM.write(1,AID);
  EEPROM.write(2,EXP);
  digitalWrite(GreenLED,HIGH);
  if(debug){Serial.println("Green on");}
}
#endif
