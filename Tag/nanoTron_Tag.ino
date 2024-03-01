#include <SoftwareSerial.h>
#include <SPI.h>

String self; //This is where our nanoTron ID goes

int Warnings = 0;
int Duration = 40;
byte SIntensity =1;

SoftwareSerial Serial3(15,14);

#define SHOCK 6
#define CHARGE 2
#define PSS A0

void setup() {
  Serial.begin(115200);
  
  Serial3.begin(115200);
  Serial3.print("NCFG 1ff\r\n");  // Set *RRN response to include RSSI data
  Serial3.print("SBIV 1000\r\n");  //Set host to blink every 1 second
  Serial3.print("EDNI 1\r\n"); // Activate *DNI to be able to read messagaes

  

  bool detected = false;
  while(detected = false){
    String nanoMessage = readNanoTron();
    if(getDNI(nanoMessage) == "10"+self+"000001"){
      Serial3.print("FNIN 0A 11"+self+"000011");
      detected = true;
    }
  }

}

void loop() {
  String nanoMessage = readNanoTron();
  if(getDNI(nanoMessage) == "10"+self+"111111"){
    Shock();
  }  
  else if(getDNI(nanoMessage) == "10"+self+"000000"){
    Reset();
  }
}

String sFill(String s, int len, char fill){
  String filled = s;
  for(int i=0; i<len-s.length(); i++){
    filled = fill+filled;
  }
  return filled;
}

String readNanoTron(){
  String nanoRead = "";
  while (Serial3.available()) {
    int bad=0;
    char c = Serial3.read();
    if (c == '*') {
      while (!Serial3.available()) {
         //Wait for a Response
      }
      char c1 = Serial3.read();
      for(int i=1; i<80; i++){
        while (!Serial3.available()) {
         //Wait for a Response
        }
        c = Serial3.read();               //Keep Reading in Node ID 
        if (c=='*'){
          bad = 1;  //variable to check for overlapping *RRN commands
          break;
        } 
        
          //stop filling out IDBlink at new line - not needed
        if (c=='\n') {
          nanoRead = nanoRead+ String(c);
          int track=i;
            //Serial.println(track);  //print length of serial command
          break;                          
        }
        nanoRead = nanoRead+ String(c);
      }
      if (bad==1) {    //Exit while loop if overlapping *RRN commands
        nanoRead = "overlap";
      }
    }
  }
  return nanoRead;
}

String getDNI(String readIn){
  String DNI = "";
  int readInSize = sizeof(readIn) / sizeof(char);

  for(int i=readInSize; i>0; i--){
    if(readIn.charAt(i) == ';'){
      String DNI = readIn.substring(i);
      break;
    }
  }
  return DNI;
}

void Shock(){

  if(Warnings > 6){
    return;
  }
   
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);

  if (Warnings > 1){
    //Set Potentiometer
    byte PotR = 10 - 2*SIntensity;
    word PotVal = getPot(0);
    while (PotVal!= PotR){
      setPot(0, PotR);
      if (true){
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
  }

  Warnings += 1;
}

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

void Reset(){
  Warnings = 0;
}
