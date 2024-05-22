//Included Libraries
#include <avr/wdt.h> 
#include <avr/interrupt.h>      
#include <avr/sleep.h>          
#include <avr/power.h> 


#define sleepMode (SLEEP_MODE_PWR_DOWN)
/*Sleep Modes
SLEEP_MODE_IDLE
Power Saving: Minimal.
Functionality: The CPU is stopped, but the system clock and peripherals (such as Timer/Counters, USART, etc.) continue to operate.
Use Case: When you need the peripherals to keep running but want to save some power by stopping the CPU.

SLEEP_MODE_ADC
Power Saving: Low.
Functionality: The CPU is stopped, and the ADC Noise Reduction mode is enabled. This allows for more accurate ADC conversions by reducing noise.
Use Case: When you need to perform ADC conversions with minimal noise.

SLEEP_MODE_PWR_SAVE
Power Saving: High.
Functionality: The CPU is stopped, the system clock is stopped, but the asynchronous timer (Timer/Counter2 in asynchronous operation) continues to run.
Use Case: When you need to wake up periodically using an asynchronous timer but want to save significant power.

SLEEP_MODE_STANDBY
Power Saving: Very high.
Functionality: Similar to Power-down mode, but the system clock is kept running, allowing faster wake-up time.
Use Case: When you need to wake up quickly while still saving significant power.

SLEEP_MODE_PWR_DOWN
Power Saving: Maximum.
Functionality: The entire microcontroller, including the system clock, is stopped. Only external interrupts or the Watchdog Timer can wake up the microcontroller.
Use Case: When you want to achieve the lowest power consumption and can afford to wake up only on external events.
*/

//Prototypes
void Sleep(int MinutesToSleep); //sets device to sleep in mode specified above
void wdtInitialize(); //configures the Watchdog Timer (WDT) to generate an interrupt after approximately 8 seconds if it is not reset within that period

//Interrupts
ISR (WDT_vect){
 //We don't actually do anything other than reset the WDT here. We just use this to wake up. 
 wdt_reset();
}

void setup(){
    wdtInitialize();
}

void loop() {
    
}

void wdtInitialize()
{
    //disable interrupts
    cli();

    //reset status register
    MCUSR = 0; 

    // using an OR leaves most bits unchanged while setting WDCE and WDE for config mode. See datasheet for details on those.
    WDTCSR |= 0b00011000; 

    //WDIE - enable interrupt, WDE - disable reset on interrupt. the right half sets the delay to 8 seconds.
    WDTCSR =  0b01000000 | 0b100001; 

    //enable interrupts
    sei();

}//wdt init

void Sleep(int MinutesToSleep) {
    // Enable global interrupts
    sei();
    
    // Calculate the total number of 8-second intervals for the desired sleep duration
    int totalIntervals = (MinutesToSleep * 60) / 8;
    
    // Set the sleep mode to Power Down
    set_sleep_mode(sleepMode);

    // Sleep for the calculated number of intervals
    for (int i = 0; i < totalIntervals; ++i)
    {
        // Enter sleep mode
        sleep_mode();
        
        // Disable sleep mode after waking up
        sleep_disable();
    }
    
    // Disable global interrupts
    cli();
    
    // Re-enable all power domains
    power_all_enable();
    
    // Disable the ADC to save power if not needed
    power_adc_disable();

}//DeepSleep
