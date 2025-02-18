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

//--------------------------------------Main-------------------------------------//
int main(void)
{
    // Stop watchdog timer
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

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
    TB1CTL |= TBCLR;    // reset settings
    TB1CTL |= TBSSEL__SMCLK;
    TB1CTL |= MC__UP;
    TB1CCR0 = 25500;     // period
    TB1CCR1 = 0*100;     // red duty
    TB1CCR2 = 0*100;     // green duty
    TB1CCR3 = 0*100;     // blue duty
    // Enable capture compare
    TB1CCTL0 |= CCIE;
    TB1CCTL1 |= CCIE;
    TB1CCTL2 |= CCIE;
    TB1CCTL3 |= CCIE;
    // Clear IFG
    TB1CCTL0 &= ~CCIFG;
    TB1CCTL1 &= ~CCIFG;
    TB1CCTL2 &= ~CCIFG;
    TB1CCTL3 &= ~CCIFG;
    // 3x: ISR: ISR_<[R, G, B]>
    // FUNCTION: function for updating clock period, given clock and period in [0,255)
    // update<Red, Green, Blue>Duty()

//-- GLOBAL ENABLES
    // enable GPIO
    // enable interrupts
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;

    while (true) {
        // turn off
        updateRedPWM(0x0);
        updateGreenPWM(0x0);
        updateBluePWM(0x0);
        usleep(1e6);
        // turn on red
        updateRedPWM(0xFF);
        usleep(1e6);
        // turn on green
        updateGreenPWM(0xFF);
        usleep(1e6);
        // turn on blue
        updateBluePWM(0xFF);
        usleep(1e6);
    }
}

//------------------------------------Functions-----------------------------------//
void updateRedPWM(unsigned char duty) {
    TB1CCR1 = duty*100;
}

void updateGreenPWM(unsigned char duty) {
    TB1CCR2 = duty*100;
}

void updateBluePWM(unsigned char duty) {
    TB1CCR3 = duty*100;
}

//---------------------------Interupt-Service-Routines---------------------------//

//-- RGB PWM ISR: PERIOD
#pragma vector = TIMER1_B0_VECTOR
__interrupt void ISR_DUTY(void)
{
    // RGB all on
    P1OUT |= BIT5;      
    P1OUT |= BIT6;
    P1OUT |= BIT7;
    TB0CCTL0 &= ~CCIFG;  // clear CCR0 IFG
}

//-- RGB PWM ISR: RED DUTY
#pragma vector = TIMER1_B1_VECTOR
__interrupt void ISR_DUTY(void)
{
    // red off
    P1OUT &= ~BIT5;      
    TB0CCTL1 &= ~CCIFG;  // clear CCR1 IFG
}

//-- RGB PWM ISR: GREEN DUTY
#pragma vector = TIMER1_B2_VECTOR
__interrupt void ISR_DUTY(void)
{
    // green off
    P1OUT &= ~BIT6;      
    TB0CCTL2 &= ~CCIFG;  // clear CCR2 IFG
}

//-- RGB PWM ISR: BLUE DUTY
#pragma vector = TIMER1_B3_VECTOR
__interrupt void ISR_DUTY(void)
{
    // blue off
    P1OUT &= ~BIT7;      
    TB0CCTL3 &= ~CCIFG;  // clear CCR3 IFG
}