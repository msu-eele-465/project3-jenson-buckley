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

// Led Array Variables
int stepIndex = 0;      // Current step index
int direction = 1;      // 1 for clockwise, -1 for counterclockwise
int stepsRemaining = 0; // Steps left to complete the step cycle
unsigned char stepSequence[] = {
    0x01, // Step 1: BIT0 (P6.0)
    0x02, // Step 2: BIT1 (P6.1)
    0x04, // Step 3: BIT2 (P6.2)
    0x08  // Step 4: BIT3 (P6.3)
};

// RGB LED variables
unsigned char rPWM = 0x0;
unsigned char gPWM = 0x0;
unsigned char bPWM = 0x0;

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
    TB1CTL |= TBCLR;    // reset settings
    TB1CTL |= TBSSEL__SMCLK;
    TB1CTL |= MC__UP;
    TB1CCR0 = 25500;        // period
    TB1CCR1 = rPWM*100;     // red duty
    TB1CCR2 = gPWM*100;     // green duty
    TB1CCR3 = bPWM*100;     // blue duty
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

//------------------------------------Functions-----------------------------------//
void setupMotor() {
    // Configure Stepper Motor Outputs (P6.0 - P6.3)
    P6DIR |= BIT0 | BIT1 | BIT2 | BIT3;
    P6OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    // Setup Timer B0
    TB0CTL = TBSSEL__SMCLK | MC__STOP | TBCLR | ID__8; // SMCLK (1Mhz), Stop mode, clear timer, divide by 8
    TB0EX0 = TBIDEX__4 ;   // Extra division by 4
    TB0CCR0 = 153;  // Set initial speed
    TB0CCTL0 = CCIE;      // Enable compare interrupt
    // Code to move motor
    //if (stepsRemaining == 0) {  // Start a new rotation only if no rotation in progress
    //    direction = 1;          // Clockwise
    //    stepsRemaining = 739;   // 36% of a rotation
    //    TB0CCR0 = 212;          // Set speed to do 36% of a rotation in 5sec
    //    TB0CTL |= MC__UP;       // Start timer
    //}
}

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
// Timer for Motor
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    if (stepsRemaining > 0)
    {
        stepIndex = (stepIndex + 4 + direction) % 4; // Update step index
        P6OUT = (P6OUT & 0xF0) | stepSequence[stepIndex]; // Update motor output
        stepsRemaining--;   // Decrement steps remaining
    }
    else
    {
        TB0CTL &= ~MC__UP;  // Stop timer when all steps are completed
        ADC_PressureReading();
    }

    TB0CCTL0 &= ~CCIFG;     // Clear interrupt flag
}

//-- Heartbeat LED ISR
    // toggle output
    // clear flag

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


//-- ISR_PATTERN
    // step pattern step# forward circularly
    // write current step to output
    // clear flag