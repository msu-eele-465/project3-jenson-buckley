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
#include <time.h>

// Led Variables
int stepIndex = 0;      // Current step index
int stepOldIndex[] = {0, 0, 0, 0, 0, 0, 0, 0};   // last index for each pattern
int prev_pattern = 0;
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

// RGB LED vars
unsigned int pwmCounter = 0;
unsigned int rPWM = 0;
unsigned int gPWM = 0;
unsigned int bPWM = 0;

// keypad
char lastKey = 'X';

//-- Function declarations
void setupLeds();
void setPattern(int);
char readKeypad();
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
    TB3CCR0 = 25500;   // period
    // Enable capture compare
    TB3CCTL0 |= CCIE;
    // Clear IFG
    TB3CCTL0 &= ~CCIFG;
    // FUNCTION: function for updating clock period, given clock and period in [0,255)
    // update<Red, Green, Blue>Duty()

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
    setPattern(0);
    
//-- GLOBAL ENABLES
    // enable GPIO
    // enable interrupts
    // Disable the GPIO power-on default high-impedance mdoe to activate
    // previously configure port settings
    PM5CTL0 &= ~LOCKLPM5;
    __enable_interrupt();       // Enable global interrupt

    // STATE
    // 0    Locked
    // 1    First correct digit entered
    // 2    Second
    // 3    Third
    // 4    Unlocked
    int state = 0;
    // PWM RGB to 0xc4921d
    updateRedPWM(0xC4);
    updateGreenPWM(0x3E);
    updateBluePWM(0x1D);
    while (true) {
        char key_val = readKeypad();
        if (key_val != 'X') {
            if (state == 0) {
                if (key_val=='1') {
                    state = 1;
                    // PWM RGB to 0xc4921d
                    updateRedPWM(0xC4);
                    updateGreenPWM(0x92);
                    updateBluePWM(0x1D);
                    // TODO: save time
                } else {
                    state = 0;
                }
            } else if (state == 1) {
                // if elapsed time > 5 s
                //      -> LOCKED
                //      update PWM color
                if (key_val=='1') {
                    state = 2;
                } else {
                    state = 0;
                    // PWM RGB to 0xc43e1d
                    updateRedPWM(0xC4);
                    updateGreenPWM(0x3E);
                    updateBluePWM(0x1D);
                }
            } else if (state == 2) {
                // if elapsed time > 5 s
                //      -> LOCKED
                //      update PWM color
                if (key_val=='1') {
                    state = 3;
                } else {
                    state = 0;
                    // PWM RGB to 0xc43e1d
                    updateRedPWM(0xC4);
                    updateGreenPWM(0x3E);
                    updateBluePWM(0x1D);
                }
            } else if (state == 3) {
                // if elapsed time > 5 s
                //      -> LOCKED
                //      update PWM color
                if (key_val=='1') {
                    state = 4;
                    // PWM RGB to 0x1da2c4
                    updateRedPWM(0x1D);
                    updateGreenPWM(0xA2);
                    updateBluePWM(0xC4);
                } else {
                    state = 0;
                    // PWM RGB to 0xc43e1d
                    updateRedPWM(0xC4);
                    updateGreenPWM(0x3E);
                    updateBluePWM(0x1D);
                }
            } else if (state == 4) {
                if (key_val=='D') {             // lock
                    state = 0;
                    // PWM RGB to 0xc43e1d
                    updateRedPWM(0xC4);
                    updateGreenPWM(0x3E);
                    updateBluePWM(0x1D);
                    // stop and reset patterns
                    setPattern(0);
                    memset(stepOldIndex, 0, sizeof(stepOldIndex));
                    TB0CCR0 = 511;
                } else if (key_val=='A') {      // decrease base period by 0.25 s
                    if (TB0CCR0 > 127) {
                        TB0CCR0 -= 128;
                    }
                } else if (key_val=='B') {      // increase base period by 0.25 s
                    if (TB0CCR0 < 2432) {
                        TB0CCR0 += 128;
                    }
                } else if (key_val=='0') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 0;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='1') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 1;
                    updateRedPWM(0x00);
                    updateGreenPWM(0xFF);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='2') {      // pattern 2
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 2;
                    updateRedPWM(0x00);
                    updateGreenPWM(0x00);
                    updateBluePWM(0xFF);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='3') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 3;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='4') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 4;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='5') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 5;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='6') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 6;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                } else if (key_val=='7') {      // pattern 0
                    // PWM RGB to 0x<custom>
                    int cur_pattern = 7;
                    updateRedPWM(0xFF);
                    updateGreenPWM(0x00);
                    updateBluePWM(0x00);
                    // save previous state
                    stepOldIndex[prev_pattern] = stepIndex;
                    // update pattern
                    if (prev_pattern != cur_pattern) {
                        stepIndex = stepOldIndex[cur_pattern];
                    } else {
                        stepIndex = 0;
                    }
                    setPattern(cur_pattern);
                    // update prev pattern
                    prev_pattern = cur_pattern;
                }
            } else {
                state = 0;
                // PWM RGB to 0xc43e1d
                updateRedPWM(0xC4);
                updateGreenPWM(0x3E);
                updateBluePWM(0x1D);
            }
        }
    }
}

//------------------------------------Functions-----------------------------------//
void setupLeds() {
    // Configure Leds (P6.0 - P6.4, P2.0 - P2.2)
    P6DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT4;
    P6OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT4);
    P2DIR |= BIT0 | BIT1 | BIT2;
    P2OUT &= ~(BIT0 | BIT1 | BIT2);
    // Setup Timer B0
    TB0CTL = TBSSEL__ACLK | MC__UP | TBCLR | ID__8; // ACLK (32,768), Stop mode, clear timer, divide by 8
    TB0EX0 = TBIDEX__8 ;   // Extra division by 8
    TB0CCR0 = 511;        // Set initial speed to 1 s
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
            seqLength = 6;
        break;
        case 2:
            stepStart = 8;
            seqLength = 8;
        break;
    }
}

char readKeypad() {
    // columns on P1.4, P5.3, P5.1, P5.0
    // rows on P5.4, P1.1, P3.5, 3.1

    char keys[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    char pressed = 'X';

    // check col 1
    P1OUT |= BIT4;
    int row = checkRows();
    if (row!=-1) {
        pressed = keys[row][0];
    } 
    P1OUT &= ~BIT4;

    // check col 2
    P5OUT |= BIT3;
    row = checkRows();
    row = checkRows();
    if (row!=-1) {
        pressed =  keys[row][1];
    } 
    P5OUT &= ~BIT3;

    // check col 3
    P5OUT |= BIT1;
    row = checkRows();
    if (row!=-1) {
        pressed =  keys[row][2];
    }
    P5OUT &= ~BIT1;

    // check col 4
    P5OUT |= BIT0;
    row = checkRows();
    if (row!=-1) {
        pressed =  keys[row][3];
    }
    P5OUT &= ~BIT0;

    if (pressed != lastKey) {
        lastKey = pressed;
        return pressed;
    } else {
        return 'X';
    }
}

int checkRows() {
    // check inputs for rows on P5.4, P1.1, P3.5, 3.1 (rows 1-4)
    if (P5IN & BIT4) {
        return 0;
    } else if (P1IN & BIT1) {
        return 1;
    } else if (P3IN & BIT5) {
        return 2;
    } else if (P3IN & BIT1) {
        return 3;
    } else {
        return -1;
    }
}

void updateRedPWM(unsigned char duty) {
    rPWM = duty*100;
}

void updateGreenPWM(unsigned char duty) {
    gPWM = duty*100;
}

void updateBluePWM(unsigned char duty) {
    bPWM = duty*100;
}

//---------------------------Interupt-Service-Routines---------------------------//
// Timer for Led
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0(void)
{
    P6OUT = (stepSequence[stepIndex + stepStart] & 0b00011111); // Update Led output
    P2OUT = ((stepSequence[stepIndex + stepStart] >>5 ) & 0b00000111); // Update Led output
    stepIndex = (stepIndex + 1) % seqLength; // Update step index
    TB0CCTL0 &= ~CCIFG;  // clear CCR0 IFG
}

// Period for RGB PWM
#pragma vector = TIMER3_B0_VECTOR
__interrupt void ISR_PWM_PERIOD(void)
{
    // RGB all on
    if (pwmCounter > rPWM) {
        P1OUT &= ~BIT5;
    } else {
        P1OUT |= BIT5;
    }
    if (pwmCounter > gPWM) {
        P1OUT &= ~BIT6;
    } else {
        P1OUT |= BIT6;
    }
    if (pwmCounter > bPWM) {
        P1OUT &= ~BIT7;
    } else {
        P1OUT |= BIT7;
    }
    if (pwmCounter > 255) {
        pwmCounter = 0;
    } else {
        pwmCounter += 1;
    }
    TB3CCTL0 &= ~CCIFG;  // clear CCR0 IFG
}