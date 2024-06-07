/*
 * This code has been created as an example of the beacon ranging and perfoming message transmissions.
 * The range function provides ranging functionality, and the update function provides examples of how to use the over the air communictaions from the nanotron radios.
 * Created by: Andrew Carefoot
 * Modified: 7 Jun 24
 */

#include <SoftwareSerial.h>

float distance = 10; //danger distance setting in cm
char c;
String tag_test = "000000000002";
int range_val;

struct tag{
  int current_range;
  String node_id;
  int voltage;
};

struct tag test;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.println("snid 000000000001");
  delay(10);
  Serial.println("gnid"); 
  Serial2.println("gnid");  //Do not broadcast ranging results (Ranging is performed by Beacon)
  delay(10);
  //Serial2.println("edan 1");
  //delay(100);
  //Serial2.println("edni 1");
}

void loop() {
  range_val = range(tag_test);
  Serial.print(range_val);
  Serial.println();
  update_me(&test);
}

int range(String tag_id){
  /*
   * This function provides ranges for the nanotron.
   * Input: nanotron tag ID in string format.
   * Output: Range of tag from the beacon in cm
   */
  String ranging = "rato 0 ";
  ranging += tag_id;
  String reading = "";
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
  int temp = 0;
  temp = buff.toInt();
  //delay(2000);
  return temp;
}

void update_me(struct tag* temp){
  /*
   * This function provides an example update function that may be used in the future.
   * Input: pointer to a tag struct to update the values.
   * Output: nothing all values are changed in memory directly.
   */
  temp->current_range = range(temp->node_id);
  Serial.println();
  /*
   * The sdat command has a special way the message must be formed.
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
  Serial2.println("sdat 1 000000000002 9 081255540254000258 5000"); //this command requests the voltage the nanotron sees
  while(Serial2.available() > 0){
    Serial.write(Serial2.read());
  }
  Serial2.println("sdat 1 000000000002 a 08125554025500034201 5000"); //this command tells the nanotron to change transmission power to 1.
  while(Serial2.available() >0){
    Serial.write(Serial2.read());
  }
  return;
}
