/*
 * This code is for the nanotron tag designs in use currently. The only service the tag provides is serial reading capability.
 * Created by: Andrew Carefoot
 * Modified:7 Jun 24
 */

#include <SoftwareSerial.h>


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("snid 000000000002");
  delay(100);
  Serial.println("gnid");
  Serial2.println("gnid");
  delay(100);
  while(Serial2.available() > 0){
    Serial.write(Serial2.read());
  }
  Serial2.println("edan 1");
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial2.available() > 0){
    Serial.write(Serial2.read());
  }
  delay(50);
}
