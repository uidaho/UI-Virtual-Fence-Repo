#include "libnanotron.hpp"

// put function declarations here:

nanotron my_nanotron;
String OtherID="";
void setup() {
  beacon_init();

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED1,!digitalRead(LED1));//Blink to show I'm alive
  my_nanotron.read_other_input_voltage(OtherID);
  delay(1000);
}
