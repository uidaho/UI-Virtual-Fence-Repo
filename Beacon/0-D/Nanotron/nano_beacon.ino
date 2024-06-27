/**
 * @file nano_beacon.ino
 * This code has been created as an example of the beacon ranging and perfoming message transmissions.
 * The range function provides ranging functionality, and the update function provides examples of how to use the over the air communictaions from the nanotron radios.
 * @author Andrew Carefoot
 * @date 10 Jun 24
 */




///danger distance setting in cm
float distance = 1000; 
///???
char c;
///???
String tag_test = "000000000002";
///???
int range_val;

struct tag{
  int current_range;
  String node_id;
  int voltage;
  int beeps;
};

struct tag test;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("snid 000000000001");
  Serial2.println("edan 1");
  delay(1);
  Serial2.println("edni 1");
  test.node_id = tag_test;
}

void loop() {
  update_me(&test);
  delay(5000);
}

  /**
   * This function provides ranges for the nanotron.
   * @param[in] tag_id struct pointer (NEEDS UPDATE)
   * @return  Range of tag from the beacon in cm
   */
int range(String tag_id){

  String ranging = "rato 0 ";
  ranging += tag_id;
  String reading = "";
  Serial.println(tag_id);
  Serial2.println(ranging);
  while (Serial2.available() > 0){ 
     c = Serial2.read();
     reading += c;
  }
  reading += "\n";
  Serial.println(reading);
  delay(10);
  String buff = "";
  buff += reading.substring(3,9);
  int cm_distance = 0;
  cm_distance = buff.toInt();
  return cm_distance;
}
  /**
   * This function provides an example update function that may be used in the future.
   * @param[in,out] temp pointer to a tag struct to update the values.
   */
void update_me(struct tag* temp){

  temp->current_range = range(temp->node_id);
  if(temp->current_range <= distance){
    temp->beeps = beep(temp->node_id,temp->beeps);
  }
  else{
    temp->beeps = 0; //assume cow has left the boundary and reset the beep count.
  }
  /*
   * The sdat command has a special way the message must be formed if sending api commands.
   * First we call for the sdat command, then selecting option 1 allows us to set a timeout which is reflected at the end.
   * the next value is the ID of the tag we are calling.
   * Then the value is the size of the data packet in bytes.
   * The 5th value is the tricky one to shape the data packet the first part: 08125554 specifies that we are doing an over the air transmission.
   * Then the 02 is the version these two parts are always the same!!!
   * following those two we call for the command type followed by a 00 always!
   * The next couple of parts are unique to each command and specified in each commands binary section and they are:
   * the length in 1 byte, the command opcode in 1 byte (CMD block), and if the command has data the data block last.
   * Final piece is the timeout of the call if using option 1 vs 0.
   */
  //put code for other updates here:
  return;
}

  /**
  *This function provides a simple shock command by sending SHOCK in hex to the tag_id input in.
  *@param[in] tag_id nanotron id to be shocked.
  */
void shock(String tag_id){

  String shocking = "sdat 0 ";
  shocking += tag_id;
  shocking += " 05 53484f434b";
  Serial.println(shocking);
  Serial2.println(shocking);
  return;
}

  /**
  *This function provides a simple beep command by sending BEEP in hex to the tag provided if less than 5 beeps have occured.
  *In the event 5 beeps have already occured the sytem will instead send a shock.
  *@param[in] tag_id id to be beeped, number of times beep has occured to resolve if shock should occur.
  *@param[in,out] beeps of beeps that has occured thus far.
  */
int beep(String tag_id, int beeps){

  if(beeps <= 5){
    String beeping = "sdat 0 ";
    beeping += tag_id;
    beeping += " 04 42454550";
    Serial.println(beeping);
    Serial2.println(beeping);
    beeps += 1;
  }
  else{
    shock(tag_id);
    beeps = 0;
  }
  return beeps;
}
