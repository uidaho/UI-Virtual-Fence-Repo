#include <Arduino.h>
#include <HardwareSerial.h>

class nanobeacon
{
private:
    
public:
    nanobeacon();
    nanobeacon(String);
    String getsettingself();
    int getsettingself(int);
    bool setsettingself(int, String);
    String ID;
    int temperature;
    int batt;
};

class nanotag
{
public:
    nanotag(String);
    int getsettingother(int, String);
    String ID;
    int temperature;
    int batt;
};

//int getsettingself(int);
//bool setsettingself(int, int);
//int getsettingother(int, String);