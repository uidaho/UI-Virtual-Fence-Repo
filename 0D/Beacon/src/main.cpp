#include <Arduino.h>
#include "libnanotron.hpp"

// put function declarations here:

nanotron my_nanotron;
String OtherID="";
void setup() {
  // put your setup code here, to run once:
  pinMode(30,OUTPUT);
  pinMode(31,OUTPUT);
  pinMode(32,OUTPUT);
  pinMode(33,OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(30,!digitalRead(30));
  my_nanotron.read_other_input_voltage();
  delay(1000);
}
