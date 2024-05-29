#include <SoftwareSerial.h>
#include <SPI.h>

String self = "000000000201"; //This is where our nanoTron ID goes

int Warnings = 0;
int Duration = 40;
byte SIntensity =1;

//SoftwareSerial Serial2(15,14);

#define SHOCK 6
#define CHARGE 2
#define PSS A0

#define LED1 30 //defined as output in beaconinit
#define LED2 31//defined as output in beaconinit
#define LED3 32//defined as output in beaconinit
#define LED4 33//defined as output in beaconinit
#define LED5 34//defined as output in beaconinit


void setup() {
  Serial.begin(115200);
  
  Serial2.begin(115200);
  Serial2.print("SFAC\r\n"); //reset nanotron
  Serial2.print("SNID "+ self +"\r\n"); //set nanotron ID
  Serial2.print("GNID\r\n");
  Serial2.print("EBID 0\r\n"); // enable blink
  Serial2.print("NCFG 8\r\n");  // Set *RRN response to include RSSI data
  //Serial2.print("SBIV 1000\r\n");  //Set host to blink every 1 second
  Serial2.print("EDNI 1\r\n"); // Activate *DNI to be able to read messagaes
  //Serial2.print("BRAR 0\r\n"); //disable ranging broadcast
  
  int timeout=0;
  while(!Serial2.available()&&timeout<10)  {
    timeout++;
    delay(1);
  }
  if (Serial2.available()){       
    while(Serial2.available()){
      Serial.write(Serial2.read());
      delay(1);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
    }
    Serial.println();
  }
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  bool detected = true;
  while(detected == false){
    String nanoMessage = readNanoTron();
    if(nanoMessage != ""){
      Serial.println(getMessage(getDNI(nanoMessage)));
    }
    String detectMessage = "10"+self+"000001\r";
    if(getMessage(getDNI(nanoMessage)) == detectMessage){
      Serial.println("detected");
      Serial2.print("FNIN 0A 11"+self+"000011\r\n");
      detected = true;
    }
  }

  Serial2.println("EBID 1");
  Serial2.println("EDNI 1");
}

void loop() {
  Serial2.println("RATO 0 000000000200");
  //Serial2.println("EDNI 1");
  String nanoRead = readNanoTron();
  if(nanoRead != ""){
  Serial.println(nanoRead);
  }
  String nanoMessage = getMessage(getDNI(nanoRead));
 /* if(nanoMessage != ""){
  Serial.println(nanoMessage);
  }*/
  if(nanoMessage == "10"+self+"111111\r"){
    Serial.println("Shocking");
    Shock();
  }  
  else if(nanoMessage == "10"+self+"000000\r"){
    Serial.println("Resetting");
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

String readNanoTron2(){
  
  String nanoRead = "";
  int timeout=0;
  while(!Serial2.available()&&timeout<10)  {
    timeout++;
    delay(1);
  }
  if (Serial2.available()){       
    while(Serial2.available()){
      char c = Serial2.read();
      nanoRead = nanoRead + c;
      //Serial.write(c);
      delay(10);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
    }
    Serial.println();
  }

  return nanoRead;
}

String readNanoTron(){
  String nanoRead = "";
  while (Serial2.available()) {
    int bad=0;
    char c = Serial2.read();
    if (c == '*') {
      while (!Serial2.available()) {
         //Wait for a Response
      }
      char c1 = Serial2.read();
      nanoRead = nanoRead+ String(c)+String(c1);
      for(int i=1; i<80; i++){
        while (!Serial2.available()) {
         //Wait for a Response
        }
        c = Serial2.read();               //Keep Reading in Node ID 
        Serial.print(c);
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

//A split function sure would be nice. Oh well...
String getDNI(String readIn){
  String DNI = "";
  int readInSize = readIn.length();
  int DNIstart = -1;

  for(int i=0; i<readInSize; i++){
    //Serial.print(readIn.charAt(i));
    if(readIn.charAt(i) == ':'){
      String check = readIn.substring(i-3, i);
      if(check == "DNI"){
        DNIstart = i+1;
      }
    }
    if(readIn.charAt(i) == '*' && DNIstart != -1){
      DNI = readIn.substring(DNIstart, i-1);
    }
    
  }
  return DNI;
}

String getMessage(String readIn){
  String message = "";
  int readInSize = readIn.length();
  int messageStart = -1;

  for(int i=readInSize; i>0; i--){
    //Serial.print(readIn.charAt(i));
    if(readIn.charAt(i) == ','){
      message = readIn.substring(i+1, readInSize);
      break;
    }
    
  }
  return message;
}

void Shock(){

  if(Warnings > 6){
    digitalWrite(LED5, HIGH);
    delay(200);
    digitalWrite(LED5, LOW);
    return;
  }
   
  digitalWrite(A2, HIGH);
  delay(200);
  digitalWrite(A2, LOW);
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED1, LOW);

  if (Warnings > 1){
    
    digitalWrite(LED2, HIGH);
    delay(200);
    digitalWrite(LED2, LOW);
    //Set Potentiometer
    /*
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
    */
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
  digitalWrite(LED1, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED5, HIGH);
  delay(200);
  digitalWrite(LED1, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED5, LOW);
  Warnings = 0;
}
