#ifndef ARDUINO_H
#define ARDUINO_H
#include <Arduino.h>
#endif
#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H
#include <HardwareSerial.h>
#endif

class nanobeacon
{
private:
    String ID;
    int temperature;
    int batt;
public:
    nanobeacon();
    nanobeacon(String);
    int getsettingself(int);
    String getsettingself();
    bool setsettingself(int, String);
    bool temptoocool(int);
    bool temptoohot(int);
};

class nanotag
{
private:
    String ID;
    int temperature;
    int batt;
    int distance;
public:
    nanotag(String);
    int getsettingother(int);
    String getsettingother();
    bool closerthandistance(int);
    bool furtherthandistance(int);
    bool temptoocool(int);
    bool temptoohot(int);
};
