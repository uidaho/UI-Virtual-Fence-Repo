#include <Arduino.h>
#include "libnanotron.hpp"

// put function declarations here:


void setup() {
  // put your setup code here, to run once:
  pinMode(30,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(30,!digitalRead(30));
  delay(1000);
}
