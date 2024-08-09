#include "beacon_settings.h"

void setup() {
  //0.1 Initialize hardware
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
    //0.8 calculate [tag.cooldown timestamp] based on [tag.distance]
    //0.9 {encode and queue} sleep message based on [tag.cooldown timestamp]
  }
  //0.10 broadcast [Message] & 0.11 clear [Message]
  Serial2.print("BDAT 0 ff ");
  for(int i=0; i<256; i++){
    Serial2.print(message.array[i]);
    message.array[i] = 0;
  }
  Serial2.println();
  //0.12 Sleep until atleast one [tag] will have [tag.cooldown timestamp] <=0

}

//This is the interupt function that we call when we recieve a transmission
void txRecieve(){
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
}

//This is the function that CDMA encodes messages and adds them to the global [message].
void encodeAndQueue(int key[32], int new_message[8]){

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
