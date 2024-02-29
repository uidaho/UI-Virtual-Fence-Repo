#include "beaconpinout.h"
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;
SoftwareSerial ss(TXPin, RXPin);
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  if(beaconInit())
  {
    goodFlash();
  }
  else
  {
    errorFlash();
  }
digitalWrite(GPSPOWER,HIGH);
ss.begin(GPSBaud);
}

// the loop function runs over and over again forever
int loopcount=0;
void loop() {
  
  if((loopcount%5000)==0)
  {
    Serial.println("Running, you can input a command for the nanotron to test, make sure that Both NL & CR is selected");
    Serial.print("GPS year:");
    Serial.println(gps.date.year());
    beaconFlash();
    loopcount=0;
  }

  if (Serial.available()) 
  {       
    char buff[32];
    int i=0;
    while(Serial.available())
    {
      buff[i]=Serial.read();
      i++;
      delay(1);
    }
    for(int j=0;j<i;j++)
    {
      Serial2.print(buff[j]);  // read it and send it out Serial1 (pins 0 & 1)
      Serial.print(buff[j]);
    }

  }

  if (Serial2.available())
  {
    while(Serial2.available())
    {
      Serial.print(Serial2.read());
      delay(1);//stops messages from getting sliced.
    }
    Serial.println();
  }
  
  if (ss.available() > 0)//GPS should almost always be available
  {
      gps.encode(ss.read());
      digitalWrite(LED1,!digitalRead(LED1));
  }


  loopcount++;
  delay(1);

}

bool beaconInit()
{
  bool startupStatus=true;
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(PORTPOWER,OUTPUT);

  pinMode(RELAYIN1,INPUT);
  pinMode(RELAYIN2,INPUT);

  pinMode(RELAYOUT1, OUTPUT);
  pinMode(RELAYOUT2, OUTPUT);

  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);
  digitalWrite(LED5,LOW);
  digitalWrite(RELAYOUT1,LOW);
  digitalWrite(RELAYOUT2,LOW);

  Serial.begin(BEACONBAUD);
  Serial2.begin(NANOBAUD); //Nanotron Communication
  Serial2.println("GNID");
  int timeout=0;
  while(!Serial2.available()&&timeout<10)
  {
    timeout++;
    delay(1);
  }
  if (Serial2.available()) 
  {       
    Serial.print("Nanotron communication detected, got:");
    while(Serial2.available())
    {
      Serial.write(Serial2.read());
      delay(1);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
    }
    Serial.println();
  }
  else
  {
    Serial.println("Nanotron communication failed,  timeout");
    startupStatus=false;//no nanotron communication was received, even after waiting
  }

return startupStatus;
}

void beaconFlash()
{
  digitalWrite(RELAYOUT1,HIGH);
  digitalWrite(LED1,HIGH);
  digitalWrite(LED2,HIGH);
  digitalWrite(LED3,HIGH);
  digitalWrite(LED4,HIGH);
  digitalWrite(LED5,HIGH);
  delay(1000);
  digitalWrite(RELAYOUT1,LOW);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,LOW);
  digitalWrite(LED5,LOW);
  delay(100);
  digitalWrite(RELAYOUT2,HIGH);
  delay(1000);
  digitalWrite(RELAYOUT2,LOW);
}

void errorFlash()
{
  for(int i =0;i<5;i++)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,HIGH);
    delay(250);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,LOW);
    delay(250);
  }

}

void goodFlash()
{
  digitalWrite(LED1,HIGH);
  delay(100);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,HIGH);
  delay(100);
  digitalWrite(LED2,LOW);
  digitalWrite(LED3,HIGH);
  delay(100);
  digitalWrite(LED3,LOW);
  digitalWrite(LED4,HIGH);
  delay(100);
  digitalWrite(LED4,LOW);
  digitalWrite(LED5,HIGH);
  delay(100);
  digitalWrite(LED5,LOW);
  for(int i =0;i<2;i++)
  {
    digitalWrite(LED1,HIGH);
    digitalWrite(LED2,HIGH);
    digitalWrite(LED3,HIGH);
    digitalWrite(LED4,HIGH);
    digitalWrite(LED5,HIGH);
    delay(250);
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    digitalWrite(LED5,LOW);
    delay(250);
  }
}