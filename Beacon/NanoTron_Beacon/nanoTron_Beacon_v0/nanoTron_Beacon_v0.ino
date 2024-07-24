void setup() {
  //0.1 Initialize hardware
  //1.0 Initialize Transmission Recieved interupt
}

void loop() {
  //0.2 get [all tags] with cooldown <= present time
  //0.3 for [tag] in [all cool tags]
    //0.4 increase [tag.communication attempts] by 1
    //0.5 if [tag.communication attempts] > [max communication attempts]
      //0.5.1 remove [tag] from [all tags] and go to 0.8
    //0.6 if [tag.distance]<=[boundary]&&[tag.distance]>0
      //0.6.1 {encode and queue} warning message
      //0.6.2 set [tag.warning flag] true and go to 0.8
    //0.7 if [tag.warning flag] == true
      //0.7.1 {encode and queue} reset message and go to 0.8
    //0.8 calculate [tag.cooldown timestamp] based on [tag.distance]
    //0.9 {encode and queue} sleep message based on [tag.cooldown timestamp]
  //0.10 broadcast [Message]
  //0.11 clear [Message]
  //0.12 Sleep until atleast one [tag] will have [tag.cooldown timestamp] <=0

}

//This is the interupt function that we call when we recieve a transmission
void txRecieve(){
  //1.1 read transmission
  //1.2 if transmission is a ranging result
    //1.2.1 update [tag.distance] for relavant [tag] and go to 1.4
  //1.3 if transmission is a new tag message
    //1.3.1 create [tag] object with transmitted ID and add [tag] to [all tags]
    //1.3.2 {encode and queue} encryption key message
  //1.4 set [tag.communication attempts] to 0
}

//This is the function that CDMA encodes messages and adds them to the global [message].
void encodeAndQueue(){

}