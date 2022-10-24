#include <msp430.h> 



// ========================================== DEFINES ==========================================

// IO Configuration
#define PIN_LED                 BIT6

// Timing defines
#define INTERRUPTS_SLEEP        2       // Sleep for 2x 426ms
#define LOOKUP_TABLE_LENGTH     100     // Size of the lookup table
#define INTERRUPT_TIME_AWAKE    2000    // Max value for PWM (lower value -> faster lookup table step-through)

// DO NOT CHANGE THESE
#define INTERRUPT_TIME_SLEEP    8000
#define BOOL                    char
#define TRUE                    1
#define FALSE                   0
#define STATE_RISE              0
#define STATE_FALL              1
#define STATE_SLEEP             -1



// ========================================== CONSTANTS ==========================================

const unsigned short LOOKUP_TABLE [] = {
   5, 6, 6, 7, 8, 9, 10, 12, 13, 15, 17, 19, 21, 24, 27, 30, 34, 38, 43, 48, 54, 61, 69, 77, 87, 98, 110, 123, 137, 154, 172, 192, 214, 238, 265, 294, 326, 360, 398, 438, 480, 526, 574, 625, 678, 734, 791, 850, 909, 970, 1030, 1091, 1150, 1209, 1266, 1322, 1375, 1426, 1474, 1520, 1562, 1602, 1640, 1674, 1706, 1735, 1762, 1786, 1808, 1828, 1846, 1863, 1877, 1890, 1902, 1913, 1923, 1931, 1939, 1946, 1952, 1957, 1962, 1966, 1970, 1973, 1976, 1979, 1981, 1983, 1985, 1987, 1988, 1990, 1991, 1992, 1993, 1994, 1994, 1995
};



// ========================================== VARIABLES ==========================================

unsigned long timerInterrupts = 0;
signed char state = STATE_SLEEP;



// ========================================== FUNCTIONS ==========================================

void init();
void init_timer();
void sleep();
void wake_up();



// ========================================== MAIN ==========================================

int main(void) {
    init();                             // Initialize system
    __bis_SR_register(LPM3_bits + GIE); // Enter LPM3 with interrupts enabled
}












// ========================================== INITIALIZATION ==========================================

void init() {

    // Set up the system
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    BCSCTL3 |= LFXT1S_2;        // Use VLOCLK for ACLK
    P1DIR = 0xFF;               // Set everything as output
    P1OUT = 0;                  // All outputs LOW
    P1DIR |= PIN_LED;           // PIN_LED output
    P1SEL |= PIN_LED;           // PIN_LED select for PWM output

    __delay_cycles(100);        // Wait before changing Clock Frequency
    BCSCTL1 = CALBC1_1MHZ;      // Set Frequency Range
    DCOCTL = CALDCO_1MHZ;       // Set Digitally Controlled Oscillator (DCO) to 1MHz (changes MCLK speed as well as SMCLK speed!)
    __delay_cycles(100);        // Wait after changing Clock Frequency

    init_timer();               // Initialize timer
}



// ========================================== INIT TIMER ==========================================

void init_timer() {

    // Sleep timer settings
    TACCR0 = INTERRUPT_TIME_SLEEP;  // Set Sleep time in Capture COmpare Register 0
    TACTL = TASSEL_1 + ID_2 + MC_1; // ACLK, Clock divided by 4, Count up mode
    CCTL0 = CCIE;                   // CCR0 interrupt enabled

    // PWM Settings
    TACCTL1 = OUTMOD_7;             // PWM Mode reset/set
    TACCR1 = 0;                     // disable CCR1 (PWM)
}



// ========================================== SLEEP ISR ==========================================

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A_ISR (void) {

    timerInterrupts += 1;   // Count the occurred interrupts



    switch (state) {        // Statemachine for handling the LED brightness



        // -------------------------- STATE SLEEP --------------------------
        case STATE_SLEEP: {
            if (timerInterrupts >= INTERRUPTS_SLEEP) {

                // > SWITCH STATE to RISE <
                TACCR1 = 0;                             // Reset PWM Capture Compare Register
                TACTL = TASSEL_2 + ID_3 + MC_1;         // Use SMCLK, Clock divided by 8, count up mode, Interrupt enable
                TACCR0 = INTERRUPT_TIME_AWAKE;          // Set Interrupt speed to Awake mode
                timerInterrupts = 0;                    // Reset interrupts
                state = STATE_RISE;                     // Set State
                P1SEL |= PIN_LED;                     	// Enable PWM
                __bic_SR_register_on_exit(LPM3_bits);   // Leave Low Power Mode 3
            }
        } break;



        // -------------------------- STATE RISE --------------------------
        case STATE_RISE: {
            if (timerInterrupts >= LOOKUP_TABLE_LENGTH) {

                // > SWITCH STATE to FALL <
                state = STATE_FALL;                     // Switch State
                timerInterrupts = 0;                    // Reset Interrupts
            } else {
                TACCR1 = LOOKUP_TABLE[timerInterrupts]; // Set next Duty Cycle
            }
        } break;



        // -------------------------- STATE FALL --------------------------
        case STATE_FALL: {
            if (timerInterrupts >= LOOKUP_TABLE_LENGTH-20) {

                // > SWITCH STATE to SLEEP <
                TACCR0 = 0xFFFF;                        // Do not interrupt in interrupt
                P1SEL &= ~PIN_LED;                      // Disable PWM
                TACCR1 = 0;                             // Reset PWM Capture Compare Register
                TACTL = TASSEL_1 + ID_2 + MC_1;         // Use ACLK, Clock divided by 4, count up mode
                TACCR0 = INTERRUPT_TIME_SLEEP;          // Set Interrupt speed to Sleep mode
                state = STATE_SLEEP;                    // Reset State
                timerInterrupts = 0;                    // Reset interrupts
                __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3 w/ interrupt
            } else {
                TACCR1 = LOOKUP_TABLE[LOOKUP_TABLE_LENGTH-1-timerInterrupts];   // Set next Duty Cycle
            }
        } break;
    }
}
