/*
libnanotron
Library to wrap nanotron functionality.

Author: Mat Shryock, Andrew Carefoot
*/
#include "libnanotron.hpp"


nanotron::nanotron(Stream& serialObj) 
    : serial_port(&serialObj), self_id(read_my_radio_id()) {}

nanotron::nanotron(String selfid,Stream& serialObj){
    ///Constructor for the nanobeacon if setting a new ID.
    uid=selfid;
    self_id = read_my_radio_id();
    serial_port = &serialObj;
    return;
}

/// @brief This function provides ranges for the nanotron.
/// @param tag_id The desired tag to range with
/// @return The range of the tag from the beacon in cm
int nanotron::range(String tag_id){
  String ranging = "rato 0 ";
  ranging += tag_id;
  String reading = "";
  serial_port->println(ranging);
  //TODO: does this fail if serial is not available?
  while (serial_port->available() > 0){ 
     char c = serial_port->read();
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
  serial_port->println("GBAT");
  String reading="";
  //TODO: does this fail if serial is not available?
  while (serial_port->available() > 0){ 
     char c = serial_port->read();
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
  serial_port->println(output);
  while(serial_port->available()){
      value += serial_port->read();
  }
  String temporary = value.substring(3,9);

  return temporary.toInt();

}

/// @brief Asks nanotron module for its ID
/// @return a string with the ID stored
String nanotron::read_my_radio_id(){
  String value = "";
  serial_port->println("gnid");
    while(serial_port->available()){
        value +=  serial_port->read();
    }
  return value;
}




