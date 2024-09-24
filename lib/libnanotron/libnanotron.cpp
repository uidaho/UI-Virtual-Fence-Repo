/*
libnanotron
Library to wrap nanotron functionality.

*/
#include "libnanotron.hpp"


nanotron::nanotron(){
    ///Constructor for the nanobeacon using its default ID.
    ID = read_my_radio_id();
    uid=ID;
    return;
}

nanotron::nanotron(String selfid){
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
  String output = "rato 0 " + OtherID;
  String value;
  Serial2.println(output);
  while(Serial2.available()){
      value += Serial2.read();
  }
  String temporary = value.substring(3,9);

  return temporary.toInt();

}

String nanotron::read_my_radio_id(){
  int query = 0;
  String value = "";
  Serial2.println("gnid");
    while(Serial2.available()){
        value +=  Serial2.read();
    }
    ID = value;
    return ID;
}




