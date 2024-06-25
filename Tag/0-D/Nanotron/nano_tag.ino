/*
 * This code is for the nanotron tag designs in use currently. The only service the tag provides is serial reading capability.
 * Created by: Andrew Carefoot
 * Modified: 18 Jun 24
 */

#include <SoftwareSerial.h>
#include "tagpinout.h"

char c;
int duration = 500;
String message = "";

SoftwareSerial Serial2(12,11);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);
  Serial2.begin(19200);
  pinMode(SHOCK,OUTPUT);
  digitalWrite(SHOCK,LOW);
  pinMode(BOOSTEN,OUTPUT);
  digitalWrite(BOOSTEN,LOW);
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
  Serial2.println("snid 000000000002");
  Serial.println("gnid");
  Serial2.println("gnid");
  while(Serial2.available() > 0){
    Serial.write(Serial2.read());
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  message = "";
  while(Serial2.available() > 0){
    c = Serial2.read();
    Serial.print(c);
    message += c;
    delay(1);
  }
  if(message.substring(0,17)== "*DNO:000000000001"){
    Serial.println("gdat");
    Serial2.println("gdat");
    delay(500);
    message = "";
    while(Serial2.available() > 0){
      c = Serial2.read();
      message += c;
      delay(1);
    }
    Serial.println(message.substring(16,24));
    if(message.substring(16,24) == "42454550"){
      beep();
    }
    else if(message.substring(16,26) == "53484F434B"){
      shock();
    }
    else{
      Serial.println("nothing");
    }
  }
}

void shock(){
  Serial.println("shocking");
  digitalWrite(BOOSTEN,HIGH);
  delay(10);
  digitalWrite(SHOCK,HIGH);
  delay(duration);
  digitalWrite(SHOCK,LOW);
  digitalWrite(BOOSTEN,LOW);
}

void beep(){
  Serial.println("beeping");
  digitalWrite(BUZZER,HIGH);
  delay(duration);
  digitalWrite(BUZZER,LOW);
}
