#include <Arduino.h>

void beacon_init(){
    pinMode(LED1,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
    pinMode(LED4,OUTPUT);
    pinMode(LED5,OUTPUT);
    pinMode(SX1280LED,OUTPUT);

    pinMode(CC_OUT1,OUTPUT);
    pinMode(CC_OUT2,OUTPUT);

    pinMode(CC_IN1,INPUT);
    pinMode(CC_IN2,INPUT);

    pinMode(BUZZER,OUTPUT);
    pinMode(HEADER_POWER,OUTPUT);

    pinMode(NANO_MOD_EN,OUTPUT);


}