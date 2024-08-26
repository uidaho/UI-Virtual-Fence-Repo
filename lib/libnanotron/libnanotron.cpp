/*
libnanotron
Library to wrap nanotron functionality.

*/
#include "libnanotron.hpp"


nanotron::nanobeacon(){
    ///Constructor for the nanobeacon using its default ID.
    ID = read_my_radio_id();
    uid=ID;
    return;
}

nanotron::nanobeacon(String selfid){
    ///Constructor for the nanobeacon if setting a new ID.
    uid=selfid;
    ID = read_my_radio_id();
    return;
}

/// @brief This function provides ranges for the nanotron.
/// @param tag_id The desired tag to range with
/// @return The range of the tag from the beacon in cm
int nanotron::range(String tag_id){
  String ranging = "rato 0 ";
  ranging += tag_id;
  String reading = "";
  Serial2.println(ranging);
  //TODO: does this fail if serial is not available?
  while (Serial2.available() > 0){ 
     char c = Serial2.read();
     reading += c;
  }
  String buff = "";
  buff += reading.substring(3,9);
  //TODO: should this be int? maybe double?
  int temp = 0;
  temp = buff.toInt();
  return temp;
}

/// @brief Gets node voltage from nanotron ADC and processes into a double per nanotron documentation.
/// @return a double representing node voltage.
double nanotron::read_my_input_voltage(){
  Serial2.println("GBAT");
  String reading="";
  //TODO: does this fail if serial is not available?
  while (Serial2.available() > 0){ 
     char c = Serial2.read();
     reading += c;
  }
  //TODO: double check that this unsigned int gets processed properly
  double node_voltage = reading.toInt();
  node_voltage=node_voltage/10;
  return node_voltage;

}

double nanotron::read_other_input_voltage(String OtherID){
  output = "rato 0 " + OtherID;
  Serial2.println(output);
  while(Serial2.available()){
      ichar = Serial2.read();
      value += ichar;
  }
  temporary = value.substring(3,9);
  query = temporary.toInt();
  distance = query;
  break;
  return node_voltage;

}

String nanotron::read_my_radio_id(){
  int query = 0;
  String value = "";
  Serial2.println("gnid");
    while(Serial2.available()){
        value +=  Serial2.read();
    }
    ID = value;
    return ID
}

String nanotron::read_other_radio_id(String OtherID){
  String value = "";
  String temporary = "";
  String command = "sdat 0 " + OtherID + " 9 081255540254000100";
  Serial2.println(command);
  while(Serial2.available()){
    value += Serial2.read();
  }
  //Serial.println(value);
  temporary = value.substring(4,8);
  if(temporary == "*AIR"){
    temporary = value.substring(31,43);
    //Serial.println(temporary);
  }

  return temporary;
}

int nanotron::read_my_temperature(){
  String value = "";
  Serial2.println("gmyt");
  while(Serial2.available()){
    value += Serial2.read();
  }
  return value.toInt();//to double? 
}
int nanotron::read_other_temperature(String OtherID){
  output = "sdat 0 " + ID + " 9 081255540254000158";
  Serial2.println(output);
  while(Serial2.available()){
      ichar = Serial2.read();
      value += ichar;
  }
  temporary = value.substring(4,8);
  if(temporary == "*AIR"){
      temporary = value.substring(31,34);
  }
  else{
      temperature = 0;
      return temperature;
  }
  query = temporary.toInt();
  temperature = query;
  break;
}





