#include "liblowpower.hpp"

int init_low_power_328p(){
    cli();//disable interrupts during set-up
    MCUSR = 0; //reset status register
    //turn on WDT and set to 8 seconds
    wdt_init();
    disable_adc();
    sei();//enable interrupts
}

/// @brief Powers down all possible peripherals and goes to the deepest possible sleep state. Wakes up with WDT
/// @param minutes_to_sleep 
void deep_sleep_328p(int minutes_to_sleep){
    sei();//enable interrupts - we sleep forever without them
    for(int sec=0;seconds<60*minutes_to_sleep;sec+=8)
    {
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();//This should enable sleep, invoke sleep, then disable sleep
    }
    cli();//disable interrupts
}

void disable_adc(){
    // Disable the ADC by setting the ADEN bit (bit 7)  of the
    // ADCSRA register to zero.
    ADCSRA = ADCSRA & B01111111;

    // Disable the analog comparator by setting the ACD bit
    // (bit 7) of the ACSR register to one.
    ACSR = B10000000;

    // Disable digital input buffers on all analog input pins
    // by setting bits 0-5 of the DIDR0 register to one.
    DIDR0 = DIDR0 | B00111111;
}

void wdt_init(){
    WDTCSR |= 0b00011000; // using an OR leaves most bits unchanged while setting WDCE and WDE for config mode. See datasheet for details on those.
    WDTCSR =  0b01000000 | 0b100001; //WDIE - enable interrupt, WDE - disable reset on interrupt. the right half sets the delay to 8 seconds.
}