/*
libnanotron
Library to wrap nanotron functionality.

*/
#include "libnanotron.hpp"

/// @brief This function provides ranges for the nanotron.
/// @param tag_id The desired tag to range with
/// @return The range of the tag from the beacon in cm
int range(String tag_id){
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
double read_my_input_voltage(){
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







