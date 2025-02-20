//------------------------------------Header-----------------------------------//
// main.c
// P.Buckley & D. Graham  , EELE-367, Project 3
// Nov 30 2024
//
// Clock Calculations
// Effective clock rate = 1MHz / (8 * 4) = 31250 Hz (clock ticks per second)

//----------------------------------Initilization---------------------------------//
#include "intrinsics.h"
#include <msp430fr2355.h>
#include <stdbool.h>

void updateRedPWM(unsigned char);
void updateGreenPWM(unsigned char);
void updateBluePWM(unsigned char);
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
    // Set up ports (outputs and driven low)
    P1DIR |= BIT5;
    P1DIR |= BIT6;
    P1DIR |= BIT7;
    P1OUT &= ~BIT5;      
    P1OUT &= ~BIT6;
    P1OUT &= ~BIT7;
    // global vars to hold R, G, and B pwm values in [0, 255)
    // rPWM, gPWM, bPWM
    // 3x: clock (setup, period, duty, clear/enable flag)
    TB3CTL |= TBCLR;    // reset settings
    TB3CTL |= TBSSEL__SMCLK;
    TB3CTL |= MC__UP;
    TB3CCR0 = 255;        // period
    TB3CCR2 = 1;     // red duty
    TB3CCR3 = 1;     // green duty
    TB3CCR4 = 1;     // blue duty
    // Enable capture compare
    TB3CCTL0 |= CCIE;
    TB3CCTL2 |= CCIE;
    TB3CCTL3 |= CCIE;
    TB3CCTL4 |= CCIE;
    // Clear IFG
    TB3CCTL0 &= ~CCIFG;
    TB3CCTL2 &= ~CCIFG;
    TB3CCTL3 &= ~CCIFG;
    TB3CCTL4 &= ~CCIFG;
    // 3x: ISR: ISR_<[R, G, B]>
    // FUNCTION: function for updating clock period, given clock and period in [0,255)
    // update<Red, Green, Blue>Duty()

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
    __enable_interrupt();
    __bis_SR_register(GIE); // Enable global interrupts


    while (true) {
        // turn on red
        updateRedPWM(0xFF);
        // turn on green
        //updateGreenPWM(0xFF);
        // turn on blue
        //updateBluePWM(0xFF);
    }

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
void updateRedPWM(unsigned char duty) {
    TB3CCR3 = duty;
}

void updateGreenPWM(unsigned char duty) {
    TB3CCR2 = duty;
}

void updateBluePWM(unsigned char duty) {
    TB3CCR4 = duty;
}

//---------------------------Interupt-Service-Routines---------------------------//

#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_PWM_PERIOD(void)
{
    // RGB all on
    P1OUT |= BIT5;      
    P1OUT |= BIT6;
    P1OUT |= BIT7;
    TB3CCTL0 &= ~CCIFG;  // clear CCR0 IFG
}
//-- RGB PWM ISR: RGB DUTIES
#pragma vector = TIMER3_B1_VECTOR
__interrupt void ISR_PWM_RGB(void)
{
    if (TB3IV & 0x2) {
        // red off
        P1OUT &= ~BIT5;      
        TB3CCTL2 &= ~CCIFG;  // clear CCR1 IFG
    }
    if (TB3IV & 0x6) {
        // green off
        P1OUT &= ~BIT6;      
        TB3CCTL3 &= ~CCIFG;  // clear CCR2 IFG
    }
    if (TB3IV & 0x8) {
        // blue off
        P1OUT &= ~BIT7;      
        TB3CCTL4 &= ~CCIFG;  // clear CCR3 IFG
    }
}