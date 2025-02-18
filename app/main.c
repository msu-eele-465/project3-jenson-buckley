//------------------------------------Header-----------------------------------//
// main.c
// P.Buckley & D. Graham  , EELE-367, Project 3
// Nov 30 2024
//
// Clock Calculations
// Effective clock rate = 1MHz / (8 * 4) = 31250 Hz (clock ticks per second)

//----------------------------------Initilization---------------------------------//
#include <msp430.h>
#include <stdbool.h>

// Led Variables
int stepIndex = 0;      // Current step index
int stepStart = 0;      // Start of the selected pattern
int seqLength = 1;      // Length of sleected sequence

unsigned char stepSequence[] = {
                // Pattern 0
                0b0,
                0b0,
                // Pattern 1
                0b00011000,
                0b00100100,
                0b01000010,
                0b10000001,
                0b01000010,
                0b00100100,
                // Pattern 2
                0b1,
                0b11,
                0b111,
                0b1111,
                0b11111,
                0b111111,
                0b1111111,
                0b11111111
            };

//--------------------------------------Main-------------------------------------//
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
    setupLeds();
    setPattern(2);
//-- GLOBAL ENABLES
    // enable GPIO
    // enable interrupts
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    __enable_interrupt();       // Enable global interrupt

    while(true){}

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

//------------------------------------Functions-----------------------------------//
void setupLeds() {
    // Configure Leds (P6.0 - P6.4, P2.0 - P2.2)
    P6DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4;
    P6OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4);
    P2DIR |= BIT0 | BIT1 | BIT2;
    P2OUT &= ~(BIT0 | BIT1 | BIT2);
    // Setup Timer B0
    TB0CTL = TBSSEL__SMCLK | MC__UP | TBCLR | ID__8; // SMCLK (1Mhz), Stop mode, clear timer, divide by 8
    TB0EX0 = TBIDEX__4 ;   // Extra division by 4
    TB0CCR0 = 153;  // Set initial speed
    TB0CCTL0 |= CCIE;      // Enable compare interrupt
}

void setPattern(int a) {
    switch (a) {
        case 0:
            stepStart = 0;
            seqLength = 2;
        break;
        case 1:
            stepStart = 2;
            seqLength = 5;
        break;
        case 2:
            stepStart = 8;
            seqLength = 8;
        break;
    }
}

//---------------------------Interupt-Service-Routines---------------------------//
// Timer for Led
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    stepIndex = (stepIndex + 1) % seqLength; // Update step index
    P6OUT = (stepSequence[stepIndex + stepStart] & 0b00011111); // Update Led output
    P2OUT = ((stepSequence[stepIndex + stepStart] >>5 ) & 0b00000111); // Update Led output

    TB0CCTL0 &= ~CCIFG;     // Clear interrupt flag
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