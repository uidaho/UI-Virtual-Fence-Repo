#ifndef ARDUINO_H
#define ARDUINO_H
#include <Arduino.h>
#endif
#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H
#include <HardwareSerial.h>
#endif



enum hardwaretype{
    tag,
    beacon
};


class nanotron
{

public:
    nanotron(Stream& serialObj) 
        : serial_port(&serialObj), self_id(read_my_radio_id()) {}
    int getsettingself(int);
    String getsettingself();
    bool setsettingself(int, String);
    int range(String tag_id);
    double read_my_input_voltage();
    String read_my_radio_id();
    int read_my_temperature();
	double read_other_input_voltage(String OtherID);
	
private:
    String self_id;
    String uid;
    hardwaretype hardware;
    Stream* serial_port;

    
};



