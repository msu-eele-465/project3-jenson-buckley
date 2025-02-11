#include <msp430.h>
#include <stdbool.h>

int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

//-- Setup heartbeat LED on P1.0
    // port (direction, initial value)
    // clock (setup, period, duty, clear/enable flag)
    // ISR: ISR_HEARTBEAT

//-- Setup RGB LED on P1.5, P1.6, P1.7
    // global vars to hold R, G, and B pwm values in [0, 255)
    // port (direction, initial value)
    // 3x: clock (setup, period, duty, clear/enable flag)
    // 3x: ISR: ISR_<[R, G, B]>
    // FUNCTION: function for updating clock period, given clock and period in [0,255)

//-- Setup LED array [0,7] on P2.1, P6.0, P6.1, P6.2, P6.3, P6.4, P3.7, P2.4
    // port (direction, initial value)
    // STRUCT: struct to map LED0-7 to ports
    // FUNCTION: function for setting LED array, given LED struct and single entry from LED pattern array

//-- Setup keypad
    // columns as outputs on P1.4, P5.3, P5.1, P5.0 initialized to 0
    // rows as inputs pulled down internally on P5.4, P1.1, P3.5, 3.1
    // STRUCT: struct to map COL0-3 and ROW0-3 to ports
    // FUNCTION: function to read keypad input, given keypad struct

//-- Setup patterns
    // STRUCT: map pattern numbers to arrays holding 8-bits to be displayed at each step
    // global to hold the current pattern array
    // clock (setup, period, duty, clear/enable flag)
    // ISR: ISR_PATTERN to display current pattern based on global and using LED array function
    // (identical to above) FUNCTION: function for updating clock period, given clock and period in [0,255)

//-- GLOBAL ENABLES
    // enable GPIO
    // enable interrupts
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

//-- WHILE TRUE:
    //-- Enter password
        // while statement that is active for only 5 seconds
        // poll keypad, checking if pw is correct (break out)

    //-- Main loop
        // poll keypad
        // update global LED pattern array
        // or
        // break (lock system)
}

//-- Heartbeat LED ISR
    // toggle output
    // clear flag

//-- ISR_<[R, G, B]>
    // turn on/off based on compare/overflow
    // clear flag

//-- ISR_PATTERN
    // step pattern step# forward circularly
    // write current step to output
    // clear flag