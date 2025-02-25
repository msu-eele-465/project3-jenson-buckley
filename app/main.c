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

// Led Variables
int stepIndex = 0;      // Current step index
int stepStart = 0;      // Start of the selected pattern
int seqLength = 1;      // Length of sleected sequence
int basePeriod = 128;
int patternMultiplier = 1;

unsigned char stepSequence[] = {
                // Pattern 0
                0b10101010,
                0b10101010,
                // Pattern 1
                0b10101010,
                0b01010101,
                // Pattern 3
                0b00011000,
                0b00100100,
                0b01000010,
                0b10000001,
                0b01000010,
                0b00100100,
                // Pattern 5
                0b00000001,
                0b00000010,
                0b00000100,
                0b00001000,
                0b00010000,
                0b00100000,
                0b01000000,
                0b10000000,
                // Pattern 6
                0b01111111,
                0b10111111,
                0b11011111,
                0b11101111,
                0b11110111,
                0b11111011,
                0b11111101,
                0b11111110,
                // Pattern 7
                0b1,
                0b11,
                0b111,
                0b1111,
                0b11111,
                0b111111,
                0b1111111,
                0b11111111
                // Pattern 3

            };
char readKeypad();
int checkRows();

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
    TB3CCR2 = 0;     // red duty
    TB3CCR3 = 0;     // green duty
    TB3CCR4 = 0;     // blue duty
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
    P1REN |= BIT1;
    P3REN |= BIT5;
    P3REN |= BIT1;
	P5OUT &=~ BIT4;     // pull-downs
	P1OUT &=~ BIT1;
	P3OUT &=~ BIT5;
	P3OUT &=~ BIT1;
    // FUNCTION: function to read keypad input
    //  readKeypad()

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
    TB0CCR0 = basePeriod;  // Set initial speed
    TB0CCTL0 |= CCIE;      // Enable compare interrupt
}

void setPattern(int a) {
    switch (a) {
        case 0:
            stepStart = 0;
            seqLength = 2;
            patternMultiplier = 4;
        break;
        case 1:
            stepStart = 2;
            seqLength = 5;
            patternMultiplier = 4;

        break;
        case 3:
            stepStart = 8;
            seqLength = 8;
            patternMultiplier = 2

        break;
        case 5:
            stepStart = 8;
            seqLength = 8;
            patternMultiplier = 6

        break;
        
        case 6:
            stepStart = 8;
            seqLength = 8;
            patternMultiplier = 2
        
        break;

        case 7:
            stepStart = 8;
            seqLength = 8;
            patternMultiplier = 4

        break;

    }

    TB0CCR0 = basePeriod * patternMultiplier;
    
}

//---------------------------Interupt-Service-Routines---------------------------//
// Timer for Led
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    stepIndex = (stepIndex + 1) % seqLength; // Update step index
    P6OUT = (stepSequence[stepIndex + stepStart] & 0b00011111); // Update Led output
    P2OUT = ((stepSequence[stepIndex + stepStart] >>5 ) & 0b00000111); // Update Led output

    while (true) {
        char val = readKeypad();
        if (val == 0x1) {
            char last_read = val;
        }
    }
}

//------------------------------------Functions-----------------------------------//
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
    if (P5IN & BIT4) {
        return 1;
    } else if (P1IN & BIT1) {
        return 2;
    } else if (P3IN & BIT5) {
        return 3;
    } else if (P3IN & BIT1) {
        return 4;
    } else {
        return 0;
    }
}

void updateRedPWM(unsigned char duty) {
    TB3CCR2 = duty;
}

void updateGreenPWM(unsigned char duty) {
    TB3CCR3 = duty;
}

void updateBluePWM(unsigned char duty) {
    TB3CCR4 = duty;
}

//---------------------------Interupt-Service-Routines---------------------------//

#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_PWM_PERIOD(void)
{
    // RGB all on
    if (TB3CCR2 != 0x0) {
        P1OUT |= BIT5;  
    }    
    if (TB3CCR3 != 0x0) {
        P1OUT |= BIT6;  
    }    
    if (TB3CCR4 != 0x0) {
        P1OUT |= BIT7;  
    }    
    TB3CCTL0 &= ~CCIFG;  // clear CCR0 IFG
}
//-- RGB PWM ISR: RGB DUTIES
#pragma vector = TIMER3_B1_VECTOR
__interrupt void ISR_PWM_RGB(void)
{
    if ((TB3IV & 0x2) & (TB3CCR2 != 0xFF)) {
        // red off
        P1OUT &= ~BIT5;      
        TB3CCTL2 &= ~CCIFG;  // clear CCR2 IFG
    }
    if ((TB3IV & 0x6) & (TB3CCR3 != 0xFF)) {
        // green off
        P1OUT &= ~BIT6;      
        TB3CCTL3 &= ~CCIFG;  // clear CCR3 IFG
    }
    if ((TB3IV & 0x8) & (TB3CCR4 != 0xFF)) {
        // blue off
        P1OUT &= ~BIT7;      
        TB3CCTL4 &= ~CCIFG;  // clear CCR4 IFG
    }
}
