#include <EEPROM.h>

int pointAddress = 0;
int memPointer = 1;
byte value;
bool reset = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Memory output");

  EEPROM.get(pointAddress, memPointer);
  
  for(int i=0; i<= memPointer; i++){
    value = EEPROM.read(i);

    Serial.print(address);
    Serial.print("\t");
    Serial.println(value, DEC);
    
  }

  if(reset == true){
    EEPROM.put(pointAddress, 0);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
