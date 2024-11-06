String message = "";
bool complete = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  
  Serial2.println("SBIV 1000");
  Serial2.println("NCFG 20");
  Serial2.print("RATO 0 000000000B01");

  message.reserve(256);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(complete){
    Serial.println(message);
    message = "";
    complete = false;
  }
  
  while(Serial2.available()){
    char readChar = (char)Serial2.read();
    message += readChar;
    if(readChar == '\n'){
      complete = true;
    }
  }

}

void serialEvent() {
  Serial.println("Serial Event");
}
