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
int direction = 1;      // 1 for clockwise, -1 for counterclockwise
int stepsRemaining = 0; // Steps left to complete the step cycle
unsigned char stepSequence[] = {
    0x01, // Step 1: BIT0 (P6.0)
    0x02, // Step 2: BIT1 (P6.1)
    0x04, // Step 3: BIT2 (P6.2)
    0x08  // Step 4: BIT3 (P6.3)
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
    P1DIR |= BIT4;
    P5DIR |= BIT3;
    P5DIR |= BIT1;
    P5DIR |= BIT0;
    P1OUT &= ~BIT4;      
    P5OUT &= ~BIT3;
    P5OUT &= ~BIT1;
    P5OUT &= ~BIT0;
    // rows as inputs pulled down internally on P5.4, P1.1, P3.5, 3.1
    P5DIR &= ~BIT4;     // inputs
    P1DIR &= ~BIT1;
    P3DIR &= ~BIT5;
    P3DIR &= ~BIT1;
    P5REN |= BIT4;      // internal resistors
    P5REN |= BIT4;
    P5REN |= BIT4;
    P5REN |= BIT4;
	P5OUT &=~ BIT4;     // pull-downs
	P1OUT &=~ BIT1;
	P3OUT &=~ BIT5;
	P3OUT &=~ BIT1;
    // FUNCTION: function to read keypad input, given keypad struct
    //  readKeypad()

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

    while (true) {
        char val = readKeypad();
    }
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

char readKeypad() {
    // columns on P1.4, P5.3, P5.1, P5.0
    // rows on P5.4, P1.1, P3.5, 3.1

    // check col 1
    P1OUT |= BIT4;
    int row = checkRows();
    if (row==1) {
        return 0x1;
    } else if (row==2) {
        return 0x4;
    } else if (row==3) {
        return 0x7;
    } else if (row==4) {
        return '*';
    }
    P1OUT &= ~BIT4;

    // check col 2
    P5OUT |= BIT3;
    row = checkRows();
    if (row==1) {
        return 0x2;
    } else if (row==2) {
        return 0x5;
    } else if (row==3) {
        return 0x8;
    } else if (row==4) {
        return 0x0;
    }
    P5OUT &= ~BIT3;

    // check col 3
    P5OUT |= BIT1;
    row = checkRows();
    if (row==1) {
        return 0x3;
    } else if (row==2) {
        return 0x6;
    } else if (row==3) {
        return 0x9;
    } else if (row==4) {
        return '#';
    }
    P5OUT &= ~BIT1;

    // check col 4
    P5OUT |= BIT0;
    row = checkRows();
    if (row==1) {
        return 'A';
    } else if (row==2) {
        return 'B';
    } else if (row==3) {
        return 'C';
    } else if (row==4) {
        return 'D';
    }
    P5OUT &= ~BIT0;

    return 'X';
}

int checkRows() {
    // check inputs for rows on P5.4, P1.1, P3.5, 3.1 (rows 1-4)
    int row_val = P5IN & BIT4;
    if (row_val==1) {
        return 1;
    }
    int row_val = P1IN & BIT1;
    if (row_val==1) {
        return 2;
    }
    int row_val = P3IN & BIT5;
    if (row_val==1) {
        return 3;
    }
    int row_val = P3IN & BIT1;
    if (row_val==1) {
        return 4;
    }
    return 0;
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

//-- ISR_<[R, G, B]>
    // turn on/off based on compare/overflow
    // clear flag

//-- ISR_PATTERN
    // step pattern step# forward circularly
    // write current step to output
    // clear flag