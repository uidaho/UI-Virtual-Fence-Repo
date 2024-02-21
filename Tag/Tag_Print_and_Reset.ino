#include <EEPROM.h>

int pointAddress = 0;
int memPointer = 1;
long value;
bool reset = true;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Memory output");

  EEPROM.get(pointAddress, memPointer);
  
  for(int i=0; i<= memPointer; i += 4){
    long four = EEPROM.read(i);
    long three = EEPROM.read(i + 1);
    long two = EEPROM.read(i + 2);
    long one = EEPROM.read(i + 3);
  
    value = ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
    

    Serial.print(memPointer);
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
