void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial3.print("NCFG 1ff\r\n");  // Set *RRN response to include RSSI data
  Serial3.print("SBIV 1000\r\n");  //Set host to blink every 1 second
  Serial3.print("EDNI 1\r\n"); // Activate *DNI to be able to read messagaes

  bool detected = false;
  while(detected = false){
    Serial3.print(
    String nanoMessage = readNanoTron();
    if(getDNI(nanoMessage) == 
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}

String sFill(String s, int len, char fill){
  String filled = s;
  for(int i=0; i<len-s.length(); i++){
    filled = fill+filled;
  }
  return filled;
}

String readNanoTron(){
  String nanoRead = "";
  while (Serial3.available()) {
    int bad=0;
    char c = Serial3.read();
    if (c == '*') {
      while (!Serial3.available()) {
         //Wait for a Response
      }
      char c1 = Serial3.read();
      for(int i=1; i<80; i++){
        while (!Serial3.available()) {
         //Wait for a Response
        }
        c = Serial3.read();               //Keep Reading in Node ID 
        if (c=='*'){
          bad = 1;  //variable to check for overlapping *RRN commands
          break;
        } 
        
          //stop filling out IDBlink at new line - not needed
        if (c=='\n') {
          nanoRead = nanoRead+ String(c);
          int track=i;
            //Serial.println(track);  //print length of serial command
          break;                          
        }
        nanoRead = nanoRead+ String(c);
      }
      if (bad==1) {    //Exit while loop if overlapping *RRN commands
        nanoRead = "overlap";
      }
    }
  }
  return nanoRead;
}

String getDNI(String readIn){
  String DNI = "";
  int readInSize = sizeof(readIn) / sizeof(char);

  for(int i=readInSize; i>0; i--){
    if(readIn.charAt(i) == ';'){
      String DNI = readIn.substring(i);
      break;
    }
  }
  return DNI;
}
