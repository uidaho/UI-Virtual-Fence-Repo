struct Point {
  float x, y;
};

struct Line {
  Point p1, p2;
};

float distance_master = -1;
float radius = 10.0;

int tagIDs[][2] = {{3,0},{6,0},{7,0},{8,0},{9,0},{10,0}}; //element 0 is id, element 1 is shock status (0 for not shocked, 1 for shocked)
int tagIDsLength =  sizeof(tagIDs) / sizeof(int);
int tagItter = 0;

int blinkRate = 1000;

void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
  Serial3.print("NCFG 1ff\r\n");  // Set *RRN response to include RSSI data
  Serial3.print("SBIV "+String(blinkRate)+"\r\n");  //Set host to blink every 1 second
  Serial3.print("EDNI 1\r\n"); // Activate *DNI to be able to read messagaes

  for(int i=0; i<tagIDsLength; i++){
    int checks = 0;
    while(checked == false){
      String c1 = "FNIN 0A 10";
      String target = sFill(String(tagIDs[i][0]), 12, '0');
      String message = "000001";
      Serial3.print(c1+target+message+"\r\n");
      String inMessage = readNanoTron();
      inMessage = getDNI(inMessage);
      if(inMessage == "11"+target+"000011"){
        checked = true;
      }
      else if ( checks == 10){
        checks = 0;
        Serial.println("Unable to contact tag" + target);
        checked = true;
      }
      else{
        checks++;
      }
    }
  }

}

void loop() {
  //check distance
  // The exact mechanisms for this may be subject ot change. It's possible that, as the nanoTron
  //allows for this, we could check all tags simultaneously. As it stands now we'll check whichever
  //tag we are concerned with this cycle and discard the ranging data for the rest.
  bool ranged = false;
  float distance;
  while(ranged == false){  
    String message = readNanoTron();
    if(message.substring()==sFill(String(tagIDs[tagItter][0]), 12, '0')){
      ranged = true;
      distance = message.substring();
    }
  }
  //send command
  if(distance < distance_master){
    String c1 = "FNIN 0A 10";
    String target = sFill(String(tagIDs[i][0]), 12, '0');
    String message = "111111";
    Serial3.print(c1+target+message+"\r\n");  

    tagIDs[tagItter][1] = 1;   
    
  }
  else if(distance >= distance_master && tagIDs[tagItter][1] == 1){
    
    String c1 = "FNIN 0A 10";
    String target = sFill(String(tagIDs[i][0]), 12, '0');
    String message = "000000";
    Serial3.print(c1+target+message+"\r\n");  

    tagIDs[tagItter][1] = 0; 
  }
  
  delay(blinkRate); //this guarantees that we have blinked atleast once so we don't 
  //potentially overwrite our message before it gets read
  //wait for response -- This adds an enourmous ammount of latency to the system may 
  //simply not do this
  if(tagItter == tagIDsLength){
    tagItter = 0;
  }
  else{
    tagItter++;
  }

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
