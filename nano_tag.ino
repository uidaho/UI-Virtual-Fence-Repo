/*
 * This code is for the nanotron tag designs in use currently. The only service the tag provides is serial reading capability.
 * Created by: Andrew Carefoot
 * Modified: 10 Jun 24
 */

#include <SoftwareSerial.h>
#include "tagpinout.h"

char c;

SoftwareSerial Serial2(NANOUSARTTX,NANOUSARTRX);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(SHOCK,OUTPUT);
  digitalWrite(SHOCK,LOW);
  pinMode(BOOSTEN,OUTPUT);
  digitalWrite(BOOSTEN,LOW);
  pinMode(BUZZER,OUTPUT);
  digitalWrite(BUZZER,LOW);
  Serial2.println("snid 000000000002");
  delay(100);
  Serial.println("gnid");
  Serial2.println("gnid");
  delay(1);
  while(Serial2.available() > 0){
    Serial.write(Serial2.read());
  }
  Serial2.println("edan 1");
}

void loop() {
  // put your main code here, to run repeatedly:
  String message = "";
  while(Serial2.available() > 0){
    c = Serial2.read();
    message += c;
  }
  if(message.substring(0,3) = "*dno"){
    Serial.println("gdat");
    message = "";
  }
  while(Serial2.available() > 0){
    c = Serial2.read();
    message += c;
  }
  Serial.println(message);
}

void shock(){
  digitalWrite(BOOSTEN,HIGH);
  delay(10);
  digitalWrite(SHOCK,HIGH);
  delay(duration);
  digitalWrite(SHOCK,LOW);
  digitalWrite(BOOSTEN,LOW);
}

void beep(){
  digitalWrite(BUZZER,HIGH);
  delay(500);
  digitalWrite(BUZZER,LOW);
}
