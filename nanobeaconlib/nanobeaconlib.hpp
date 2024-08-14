#include <Arduino.h>
#include <HardwareSerial.h>

class nanobeacon
{
private:
    
public:
    nanobeacon();
    nanobeacon(String);
    int getsettingself(int);
    String getsettingself();
    bool setsettingself(int, String);
    String ID;
    int temperature;
    int batt;
};

class nanotag
{
public:
    nanotag(String);
    int getsettingother(int);
    String getsettingother();
    String ID;
    int temperature;
    int batt;
    int distance;
};
