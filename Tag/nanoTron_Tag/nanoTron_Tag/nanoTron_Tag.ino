void setup() {
  //2.1 Initialize hardware
}

void loop() {
  //2.2 if [On Network] == true
    //2.2.1 Listen to radio
    //2.2.2 if recieved message within [Timeout]
      //2.2.2.1 Decrypt message & set [Transmission Decay] to 0
      //2.2.2.2 if message for us (Because CDMA broadcasts messages to all tags we can recieve a message with no content it it for us. Decoding a message like this should result in an empty message.)
        //2.2.2.2.1 set [Sleep Time] based on message
        //2.2.2.2.2 if message is warning
          //2.2.2.2.2.1 Increment [warnings] by 1
          //2.2.2.2.2.2 if [warnings] >= 1 && [warnings <= 10]
            //2.2.2.2.2.2.1 Beep
            //2.2.2.2.2.2.2 if [warnings] > 5
              //2.2.2.2.2.2.2.1 Shock
            //2.2.2.2.2.2.3 go to 2.5
          //2.2.2.2.2.3 go to 2.5
        //2.2.2.2.3 if message is reset
          //2.2.2.3.1 set [warnings] to 0
        //2.2.2.2.4 go to 2.5
      //2.2.2.3 go to 2.5
    //2.2.3 Increment [Transmission Decay] by 1
    //2.2.4 if [Transmission Decay] > 10
      //2.2.4.1 set [On Network] false
    //2.2.5 go to 2.5
  //2.3 Listen to Radio
  //2.4 if any network traffic is detected
    //2.4.1 Transmit addition message
    //2.4.2 Listen to Radio
    //2.4.3 if recieved encryption message within [timeout]
      //2.4.3.1 set [Encryption Key] based on message
      //2.4.3.2 set [On Network] true
  //2.5 Sleep for [sleep time]
}
