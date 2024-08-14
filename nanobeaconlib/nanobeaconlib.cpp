#include "nanobeaconlib.hpp"

nanobeacon::nanobeacon()
{
    int setting = 0;
    ID = getsettingself(setting);
}

nanobeacon::nanobeacon(String selfid){
    int setting = 0;
    setsettingself(setting,selfid);
}

int nanobeacon::getsettingself(int setting){
    /**
     * Function provides a wrapper to get information about one of the nanotron sensors.
     * input: an integer correlated to a setting:
     *      0: gets radio ID
     *      1: gets the temperature of the radio
     *      2: gets the voltage of the radio
     * Output:returns the response value of the nanotron in an integer.
     */
    int query = 0;
    char ichar;
    String value = "";

    switch (setting)
    {
    case 0:
        Serial2.println("gnid");
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        ID = value;
        query = 1;
        break;
    case 1:
        Serial2.println("gmyt");
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        query = value.toInt();
        temperature = query;
        break;
    case 2:
        Serial2.println("gbat");
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        query = value.toInt();
        batt = query;
        break;
    default:
        break;
    }
    return query;
}

String nanobeacon::getsettingself(){
    /**
     * Function provides a wrapper to get all data from the nanotron at once.
     * Input: Void
     * Output: String in form: ID, Temperature, Voltage
     */
    String values = "";
    String value_conversion;
    int valuen = 0;
    int valuesetting;
    
    values = ID + ", ";
    valuesetting = 1;
    valuen = getsettingself(valuesetting);
    value_conversion = String(valuen);
    values = values + value_conversion + ", ";
    value_conversion = "";
    valuesetting = 2;
    valuen = getsettingself(valuesetting);
    value_conversion = String(valuen);
    values += value_conversion;
    return values;
}

bool nanobeacon::setsettingself(int setting, String option){
    /**
     * Function provides a wrapper to edit a common setting on the nanotron devices.
     * Input: int to select setting, String for value to set(must be a string for ID)
     *      0: save settings, void
     *      1: set node ID to value other than default ex: 00000000001
     *      2: sets the nanotron chirp notifications on for value 1 and off for value 0
     *      3: sets the nanotron chirp on for value 1 and off for value 0
     *      4: sets the how often the chirp should occur 50-65000 ms options
     *      5: sets the transmission power of the device 0-63
     * Output: boolean value of true means change was success
     */
    String output = "";
    bool query = 0;
    String value = "";

    switch (setting)
    {
    case 0:
        Serial2.println("sset");
        query = 1;
        break;
    case 1:
        output = "snid " + option;
        Serial2.println(output);
        ID = option;
        query = 1;
        break;
    case 2:
        output = "eidn " + option;
        Serial2.println(option);
        query = 1;
    case 3:
        output = "ebid " + option;
        Serial2.println(option);
        query = 1;
    case 4:
        output = "sbiv " + option;
        Serial2.println(output);
        query = 1;
    case 5:
        output = "stxp " + option;
        Serial2.println(option);
        query = 1;
    default:
        break;
    }
    return query;
}

int nanotag::getsettingother(int setting){
    /**
     * Function provides a wrapper to get information from other nanotrons over the air if a value of 0 is ever returned an error has occured.
     * input: an integer correlated to a setting:
     *      0: gets radio ID
     *      1: gets the temperature of the radio
     *      2: gets the voltage of the radio
     *      3: gets the range of the radio
     * Output:returns the response value of the nanotron in an integer.
     */
    String output = "";
    int query = 0;
    String value = "";
    char ichar;
    String temporary = "";

    switch (setting)
    {
    case 0:
        output = "sdat 0 " + ID + " 9 081255540254000100";
        Serial2.println(output);
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        Serial.println(value);
        temporary = value.substring(4,8);
        if(temporary == "*AIR"){
            temporary = value.substring(31,43);
            Serial.println(temporary);
        }
        else {
            query = 0;
            return query;
        }
        query = 1;
        break;
    case 1:
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
    case 2:
        output = "sdat 0 " + ID + " 9 081255540254000159";
        Serial2.println(output);
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        temporary = value.substring(4,8);
        if(temporary == "*AIR"){
            temporary = value.substring(31,35);
        }
        else{
            batt = 0;
            return batt;
        }
        query = temporary.toInt();
        batt = query;
        break;
    case 3:
        output = "rato 0 " + ID;
        Serial2.println(output);
        while(Serial2.available()){
            ichar = Serial2.read();
            value += ichar;
        }
        temporary = value.substring(3,9);
        query = temporary.toInt();
        distance = query;
        break;
    default:
        break;
    }
    return query;
}

String nanotag::getsettingother(){
    /**
     * Function provides a wrapper to get all information a nanotron tag at once.
     * Input: String with tag to get information from
     * Output: String in form: ID, Temperature, Voltage, Range
     * #Need to add RSSI to this still
     */
    String values = "";
    String value_conversion;
    int valuen = 0;
    int valuesetting;
    
    values = ID + ", ";
    valuesetting = 1;
    valuen = getsettingother(valuesetting);
    value_conversion = String(valuen);
    values = values + value_conversion + ", ";
    value_conversion = "";
    valuesetting = 2;
    valuen = getsettingother(valuesetting);
    value_conversion = String(valuen);
    values = values + value_conversion + ", ";
    valuesetting = 3;
    valuen = getsettingother(valuesetting);
    value_conversion = String(valuen);
    values += value_conversion;
    return values;
}
        