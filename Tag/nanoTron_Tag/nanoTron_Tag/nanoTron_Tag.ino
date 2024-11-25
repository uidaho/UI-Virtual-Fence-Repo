#include "tag_settings.h"
#include <SoftwareSerial.h>

//SoftwareSerial Serial2(0,1);

void setup() {
  //2.1 Initialize hardware
  pinMode(SHOCK, OUTPUT);
  digitalWrite(SHOCK, LOW);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  pinMode(NANOENABLE, OUTPUT);
  digitalWrite(NANOENABLE, HIGH);

  Serial.begin(TAGBAUD);
  Serial2.begin(NANOBAUD);
  Serial2.println("EDAN 1");
  delay(1);
  Serial2.println("EDNI 1");
  Serial2.println("EBID 1");

  //Set self settings
  //To change the ID of a tag change the last characters of the snid, and
  //change the first argument of walshRow to match.
  Serial2.println("SNID 100000000001");
  walshRow(1, 32);

}

void loop() {
  //2.2 if [On Network] == true
  //if (on_network == true) {
  if (true) {
    Serial.println("On Network");
    //2.2.1 Listen to radio
    String message = radioListen();
    Serial.println(message);
    //2.2.2 if recieved message within [Timeout]
    if (message != "") {
      Serial.println("Message Recieved");
      //2.2.2.1 Decrypt message & set [Transmission Decay] to 0
      String messageActual = decrypt(message);
      //2.2.2.2 if message for us (Because CDMA broadcasts messages to all tags we can recieve a message with no content in it for us. Decoding a message like this should result in an empty message.)
      if (messageActual != "0000") { //this is what a message with no content looks like
        //2.2.2.2.1 set [Sleep Time] based on message
        if (messageActual.substring(0, 1) == "00") {
          sleep_time = 2000;
        }
        else if (messageActual.substring(0, 1) == "01") {
          sleep_time = 4000;
        }
        else if (messageActual.substring(0, 1) == "10") {
          sleep_time = 6000;
        }
        else if (messageActual.substring(0, 1) == "11"){
          sleep_time = 10000;
        }
        //2.2.2.2.2 if message is warning
        if (messageActual.substring(2, 3) == "11") {
          //2.2.2.2.2.1 Increment [warnings] by 1
          warnings++;
          //2.2.2.2.2.2 if [warnings] >= 1 && [warnings <= 10]
          if (warnings >= 1 && warnings <= 10) {
            //2.2.2.2.2.2.1 Beep
            Beep();
            //2.2.2.2.2.2.2 if [warnings] > 5
            if (warnings > 5) {
              //2.2.2.2.2.2.2.1 Shock
              Shock();
            }
            //2.2.2.2.2.2.3 go to 2.5
          }
          //2.2.2.2.2.3 go to 2.5
        }
        //2.2.2.2.3 if message is reset
        else if (messageActual.substring(2, 3) == "10") {
          //2.2.2.3.1 set [warnings] to 0
          warnings = 0;
        }
        //2.2.2.2.4 go to 2.5
      }
      //2.2.2.3 go to 2.5
    }
    else {
      //2.2.3 Increment [Transmission Decay] by 1
      transmission_decay++;
      Serial.println(transmission_decay);
      //2.2.4 if [Transmission Decay] > 10
      /*if (transmission_decay > 10) {
        //2.2.4.1 set [On Network] false
        on_network = false;
      }*/
    }
    //2.2.5 go to 2.5
  }
  else {
    /*Serial.println("Off Network");
      //2.3 Listen to Radio
      String message = radioListen();
      Serial.println(message);
      //2.4 if any network traffic is detected
      if (message != "") {
      Serial.println("Message Recieved");
      //2.4.1 Transmit addition message
      Serial2.print("SDAT 0 ");
      Serial2.print(message.substring(5, 16));
      Serial2.println(" 01 00");

      Serial.print("SDAT 0 ");
      Serial.print(message.substring(5, 16));
      Serial.println(" 01 00");
      //2.4.2 Listen to Radio
      String message = radioListen();
      //2.4.3 if recieved encryption message within [timeout]
      if (message != "") {
        //2.4.3.1 set [Encryption Key] based on message
        int encryption_index = message.substring(22, 24).toInt();
        walshRow(encryption_index, 32);
        //2.4.3.2 set [On Network] true
        on_network = true;
      }
      }*/
  }
  //2.5 Sleep for [sleep time]
  //Sleep(sleep_time);
}

String radioListen(){
  int timeout = 0;
  int buff = 0;
  String nanoRead = "";
  while(!Serial2.available()&&timeout<10)  {
    Serial.print(timeout);
    timeout++;
    delay(1);
  }
  if (Serial2.available()){       
    while(Serial2.available() && buff <= 256){
      Serial.write(Serial2.read());
      nanoRead = nanoRead+Serial2.read();
      delay(1);  //a small delay helps the entire message come through before moving on. Otherwise messages got cut in half
      buff++;
    }
    Serial.println();
  }
  return nanoRead;
}


/*String radioListen() {
  char c;
  String reading = "";
  int listenStart = millis();
  int buff = 0;
  while (Serial2.available() || buff <= 256) {
    Serial.write(Serial2.read());
    reading += char(Serial2.read());
    delay(1);
    if (c == '\n') {
      return reading;
    }
    buff ++;
  }
  return reading;
}*/

String decrypt(String message) {
  int messageInt[128];
  int patternedMessage[128];
  int decodedMessage[8];
  for (int i = 0; i < message.length(); i++) {
    messageInt[i] = message.charAt(i) - '0'; // the (- '0') is supposed to convert a character to an integer
  }
  int keyItter = 0;
  for (int i = 0; i < 256; i++) {
    if (keyItter >= 32) {
      keyItter = 0;
    }
    patternedMessage[i] = messageInt[i] * encryption_key[keyItter];
    keyItter++;
  }
  for (int i = 0; i < 8; i++) {
    int messageChunk = 0;
    for (int j = 0; j < 32; j++) {
      messageChunk = messageChunk + patternedMessage[(i * 32) + j];
    }
    decodedMessage[i] = messageChunk;
  }
  String messageFinal = "";
  for (int i = 0; i < 8; i++) {
    if (decodedMessage[i] > 0) {
      messageFinal = messageFinal + "1";
    }
    else {
      messageFinal = messageFinal + "0";
    }
  }
  return messageFinal;
}

void Beep() {
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
}

void Shock() {
  digitalWrite(BOOSTEN, HIGH);
  delay(10);
  digitalWrite(SHOCK, HIGH);
  delay(40);
  digitalWrite(SHOCK, LOW);
  digitalWrite(BOOSTEN, LOW);
}

void Sleep(int sleep_time) {
  delay(sleep_time);
}

void walshRow(int row, int height) {
  //mod the row number to determine whether we're reflecting the first or second row of the original
  //This is the first element that is always true. Because the fractal is square rows are reflections of either the first or second row of the seed.
  if (row % 2 == 0) {
    encryption_key[0] = 1;
    encryption_key[1] = 1;
  } else {
    encryption_key[0] = 1;
    encryption_key[1] = 0;
  }
  //Use logical operators to determine the truth value of each bit in sequence and reflect accordingly
  int bitGet = 1;
  int rowHalf = row / 2;
  int wRowPos = 2; //index of wRow
  for (int i = 0; i < sqrt(height); i++) {
    //use bitGet to get the bit of rowHalf
    int rowBit = rowHalf & bitGet;
    if (rowBit == 0) { //True reflection
      for (int j = 0; j < wRowPos; j++) {
        encryption_key[wRowPos + j] = encryption_key[j];
      }
      wRowPos = wRowPos * 2;
    }
    else { //False reflection
      for (int j = 0; j < wRowPos; j++) {
        encryption_key[wRowPos + j] = abs(encryption_key[j] - 1);
      }
      wRowPos = wRowPos * 2;
    }
    bitGet = bitGet << 1;
  }
}
