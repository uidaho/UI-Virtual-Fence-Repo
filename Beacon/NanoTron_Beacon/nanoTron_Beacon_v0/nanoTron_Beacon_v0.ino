#include "beacon_settings.h"
#include <SoftwareSerial.h>

//SoftwareSerial Serial2(NANOTX,NANORX);

void setup() {
  //0.1 Initialize hardware
  pinMode(NANORX, INPUT);
  
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("SNID 000000000001");
  Serial2.println("EDAN 1");
  delay(1);
  Serial2.println("EDNI 1");
  //1.0 Initialize Transmission Recieved interupt
  attachInterrupt(digitalPinToInterrupt(NANORX), txRecieve, RISING);
}

void loop() {
  //0.2 get [all tags] with cooldown <= present time
  int numberOfCoolTags = 0;
  for(int i=0; i<number_of_tags; i++){
    if(all_tags[i].cooldown_timestamp < millis()){
      indexes_of_all_cool_tags[numberOfCoolTags] = i;
      numberOfCoolTags++;
    }
  }
  //0.3 for [tag] in [all cool tags]
  for(int i=0; i<numberOfCoolTags; i++){
    //0.4 increase [tag.communication attempts] by 1
    all_tags[indexes_of_all_cool_tags[i]].communication_attempts++;
    //0.5 if [tag.communication attempts] > [max communication attempts]
    if(all_tags[indexes_of_all_cool_tags[i]].communication_attempts > max_communication_attempts){
      //0.5.1 remove [tag] from [all tags] and go to 0.8
      //Actually queue tags to be removed from the master list. Removing them now will mess up the rest of the loops.
      indexes_of_all_removed_tags[i] = indexes_of_all_cool_tags[i];
    }
    //0.6 if [tag.distance]<=[boundary]&&[tag.distance]>0
    else if (all_tags[indexes_of_all_cool_tags[i]].distance <= boundary && all_tags[indexes_of_all_cool_tags[i]].distance > 0){
      //0.6.1 {encode and queue} warning message
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, warning_message);
      //0.6.2 set [tag.warning flag] true and go to 0.8
      all_tags[indexes_of_all_cool_tags[i]].warning_flag = true;
    }
    //0.7 if [tag.warning flag] == true
    else if(all_tags[indexes_of_all_cool_tags[i]].warning_flag == true){
      //0.7.1 {encode and queue} reset message and go to 0.8
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, reset_message);
      all_tags[indexes_of_all_cool_tags[i]].warning_flag = false;
    }
    //0.8 calculate [tag.cooldown timestamp] based on [tag.distance] & 0.9 {encode and queue} sleep message based on [tag.cooldown timestamp]
    //This math is a bit paper-napkin, but a cow has a 25mph dead sprint which is about 73 ft in 2 sec, 73ft is about 22m, and round up to 25m to give some cushion/make the number prettier
    if(all_tags[indexes_of_all_cool_tags[i]].distance < boundary + 2500){
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, sleep1);
    }
    else if(all_tags[indexes_of_all_cool_tags[i]].distance <= boundary + 5000 && all_tags[indexes_of_all_cool_tags[i]].distance > boundary + 2500){
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, sleep2);      
    }
    else if(all_tags[indexes_of_all_cool_tags[i]].distance <= boundary + 7500 && all_tags[indexes_of_all_cool_tags[i]].distance > boundary + 5000){
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, sleep3);      
    }
    else{
      encodeAndQueue(all_tags[indexes_of_all_cool_tags[i]].encryption_key, sleep4);      
    }
  }
  //0.10 broadcast [Message] & 0.11 clear [Message]
  Serial.print("Broadcasting: ");
  Serial2.print("BDAT 0 ff ");
  for(int i=0; i<256; i++){
    Serial2.print(message[i]);
    Serial.print(message[i]);
    message[i] = 0;
  }
  Serial2.println();
  Serial.println();
  //0.11&1/2 remove tags specified in 0.5.1
  Tag newAllTags[32];
  int removedTagItter = 0;
  for(int i=0; i<32; i++){
    if(i == indexes_of_all_removed_tags[removedTagItter]){
      removedTagItter++;
      number_of_tags--;
    }
    else{
      newAllTags[i-removedTagItter] = all_tags[i];
    }
  }
  Serial.println("Disconected Tags Removed");
  int min_cool = 100;
  for(int i=0; i<=number_of_tags; i++){
    all_tags[i] = newAllTags[i];
    if(all_tags[i].cooldown_timestamp < min_cool && all_tags[i].cooldown_timestamp > -1){
      min_cool = all_tags[i].cooldown_timestamp;
    }
  }
  //0.12 Sleep until atleast one [tag] will have [tag.cooldown timestamp] <=0
  Serial.print("Sleep Duration: ");
  Serial.println(min_cool);
  Sleep(min_cool);
}

//This is the interupt function that we call when we recieve a transmission
void txRecieve(){
  Serial.println("Interupt Triggered");
  //1.1 read transmission
  char c;
  String reading = "";
  while(Serial2.available()){
    c = Serial2.read();
    reading += c;
  }
  //1.2 if transmission is a ranging result
  if(reading.substring(0,3) == "*RRN"){
    //1.2.1 update [tag.distance] for relavant [tag] and go to 1.4
    all_tags[getTag(reading.substring(5,16))].distance = reading.substring(33,38).toInt();  
  }
  //1.3 if transmission is a new tag message
  else if(reading.substring(0,3) == "*DNO"){
    //1.3.1 create [tag] object with transmitted ID and add [tag] to [all tags]
    String newID = reading.substring(4);
    Tag newTag;
    newTag.ID = newID;
    number_of_tags++;
    all_tags[number_of_tags] = newTag;
    
    //1.3.2 direct transmit encryption key message
    Serial2.print("SDAT 0 ");
    Serial2.print(newID);
    Serial2.print(" 01 ");
    if(number_of_tags<10){
      Serial.println("0"+number_of_tags);
    } else {
      Serial.println(number_of_tags);
    }
  }
  //1.4 set [tag.communication attempts] to 0
  //Is it possible/likely for us to recieve transmissions that arent *DNOs or *RRNs?
  all_tags[getTag(reading.substring(5,16))].communication_attempts = 0;
}

//This is the function that CDMA encodes messages and adds them to the global [message].
void encodeAndQueue(int key[32], int new_message[8]){
  //encode the message (change all the 0's to -1's)
  int encoded_message[8];
  for(int i=0; i<8; i++){
    encoded_message[i] = 2*new_message[i]-1;
  }

  //get the kronecker product of the encoded message and the key
  int kronecker_message[256];
  for(int i=0; i<8; i++){
    for(int j=0; j<32; j++){
      kronecker_message[(i*32)+j] = key[j]*encoded_message[i];
    }
  }

  //add the kroneckered message to the global message
  for(int i=0; i<256; i++){
    message[i] = message[i] + kronecker_message[i];
  }

}

//Returns the index in all_tags of tag with specified ID
//This function currently has no error handling, so for now don't call this unless you are certain that the tag will exist.
int getTag(String getID){
  for(int i=0; i<32; i++){
    if(all_tags[i].ID == getID){
      return i;
    }
  }
}

void Sleep(int sleep_time){
  delay(sleep_time);
}

void walshRow(int row, int height, Tag t){
  //mod the row number to determine whether we're reflecting the first or second row of the original
  //This is the first element that is always true. Because the fractal is square rows are reflections of either the first or second row of the seed.
  if(row%2 == 0){
    t.encryption_key[0] = 1;
    t.encryption_key[1] = 1;
  } else {
    t.encryption_key[0] = 1;
    t.encryption_key[1] = 0;
  }
  //Use logical operators to determine the truth value of each bit in sequence and reflect accordingly
  int bitGet = 1;
  int rowHalf = row/2;
  int wRowPos = 2; //index of wRow
  for(int i=0; i<sqrt(height); i++){
    //use bitGet to get the bit of rowHalf
    int rowBit = rowHalf & bitGet;
    if(rowBit == 0){//True reflection
      for(int j=0; j<wRowPos; j++){
        t.encryption_key[wRowPos+j] = t.encryption_key[j];
      }
      wRowPos = wRowPos * 2;
    }
    else{//False reflection
      for(int j=0; j<wRowPos; j++){
        t.encryption_key[wRowPos+j] = abs(t.encryption_key[j]-1);
      }
      wRowPos = wRowPos * 2;
    }
    bitGet = bitGet << 1;
  }
}
