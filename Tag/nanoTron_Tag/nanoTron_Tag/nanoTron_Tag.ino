#include "tag_settings.h"

void setup() {
  //2.1 Initialize hardware
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("EDAN 1");
  delay(1);
  Serial2.println("EDNI 1");
  Serial2.println("EBID 1");
}

void loop() {
  //2.2 if [On Network] == true
  if(on_network == true){
    //2.2.1 Listen to radio
    String message = radioListen();
    //2.2.2 if recieved message within [Timeout]
    if(message != ""){
      //2.2.2.1 Decrypt message & set [Transmission Decay] to 0
      String messageActual = decrypt(message);
      //2.2.2.2 if message for us (Because CDMA broadcasts messages to all tags we can recieve a message with no content in it for us. Decoding a message like this should result in an empty message.)
        if(messageActual != "00000000"){//this is what a message with no content looks like
        //2.2.2.2.1 set [Sleep Time] based on message
        if(messageActual.substring(0,3) == "1001"){
          sleep_time = 2000;
        }
        else if(messageActual.substring(0,3) == "1010"){
          sleep_time = 4000;
        }
        else if(messageActual.substring(0,3) == "1100"){
          sleep_time = 6000;
        }
        //2.2.2.2.2 if message is warning
        if(messageActual.substring(4,7) == "1111"){
          //2.2.2.2.2.1 Increment [warnings] by 1
          warnings++;
          //2.2.2.2.2.2 if [warnings] >= 1 && [warnings <= 10]
          if(warnings >= 1 && warnings <=10){
            //2.2.2.2.2.2.1 Beep
            Beep();
            //2.2.2.2.2.2.2 if [warnings] > 5
            if(warnings > 5){
              //2.2.2.2.2.2.2.1 Shock
              Shock();
            }
            //2.2.2.2.2.2.3 go to 2.5
          }
          //2.2.2.2.2.3 go to 2.5
        }
        //2.2.2.2.3 if message is reset
        else if(messageActual.substring(4,7) == "1000"){
          //2.2.2.3.1 set [warnings] to 0
          warnings = 0;
        }
        //2.2.2.2.4 go to 2.5
        }
      //2.2.2.3 go to 2.5
    }
    else{
    //2.2.3 Increment [Transmission Decay] by 1
      transmission_decay++;
    //2.2.4 if [Transmission Decay] > 10
      if(transmission_decay > 10){
      //2.2.4.1 set [On Network] false
        on_network = false;
      }
    }
    //2.2.5 go to 2.5
  }
  else{
  //2.3 Listen to Radio
  String message = radioListen();
  //2.4 if any network traffic is detected
    if(message != ""){
    //2.4.1 Transmit addition message
    Serial2.print("SDAT 0 ");
    Serial2.print(message.substring(5,16));
    Serial2.println(" 01 00");
    //2.4.2 Listen to Radio
    String message = radioListen();
    //2.4.3 if recieved encryption message within [timeout]
    if(message != ""){
      //2.4.3.1 set [Encryption Key] based on message
      int encryption_index = message.substring(22,24).toInt();
      for(int i=0; i<32; i++){
        encryption_key[i] = encryption_key_table[encryption_index][i];
      }
      //2.4.3.2 set [On Network] true
      on_network = true;
    }
    }
  }
  //2.5 Sleep for [sleep time]
  Sleep(sleep_time);
}

String radioListen(){
  char c;
  String reading = "";
  int listenStart = millis();
  while(millis() < listenStart+timeout){
    while(Serial2.available()){
      c = Serial2.read();
      reading += c;
    }
  }

  return reading;
}

String decrypt(String message){
  int messageInt[256];
  int patternedMessage[256];
  int decodedMessage[8];
  for(int i=0; i<message.length(); i++){
    messageInt[i] = message.charAt(i) - '0'; // the (- '0') is supposed to convert a character to an integer
  }
  int keyItter = 0;
  for(int i=0; i<256; i++){
    if(keyItter >= 32){
      keyItter = 0;
    }
    patternedMessage[i] = messageInt[i]*encryption_key[keyItter];
    keyItter++;
  }
  for(int i=0; i<8; i++){
    int messageChunk = 0;
    for(int j=0; j<32; j++){
      messageChunk = messageChunk + patternedMessage[(i*32)+j];
    }
    decodedMessage[i] = messageChunk;
  }
  String messageFinal = "";
  for(int i=0; i<8; i++){
    if(decodedMessage[i] > 0){
      messageFinal = messageFinal + "1";
    }
    else{
      messageFinal = messageFinal + "0";
    }
  }
  return messageFinal;
}

void Beep(){
  
}

void Shock(){
  
}

void Sleep(int t){
  
}
