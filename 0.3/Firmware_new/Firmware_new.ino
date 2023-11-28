/*
  Omamori Firmware v1.1

  Author: Felix Mark
  Date:   23.10.2022
*/

// ==========================================================================================================================
// Includes
// ==========================================================================================================================
#include <avr/interrupt.h> // Used for enabling interrupts (especially by RTC)
#include <avr/sleep.h>     // Used for sleeping

// ==========================================================================================================================
// Defines
// ==========================================================================================================================
#define PIN_SUPERCAP_MEASUREMENT PIN_A1 // MEASURE SUPERCAP
#define PIN_BUTTON PIN_A2               // BUTTON
#define PIN_LED1 PIN_A3                 // TOP LED
#define PIN_LED2 PIN_A4                 // MID LED
#define PIN_LED3 PIN_A5                 // BOT LED

#define NUM_SIGMOID_VALUES 105 // amount
#define BLINK_DELAY 50         // ms
#define ANIMATION_DELAY 5      // ms
#define SLEEP_SECONDS 15       // s
#define ANIMATION_OCCURS 10    // play animation every nth time

#define ADC_VALUE_BEFORE_BROWNOUT 1265 // ADC output @ 1.7V
#define ADC_VALUE_1_3_CHARGED 2208     // ADC output @ 3.0V
#define ADC_VALUE_2_3_CHARGED 3151     // ADC output @ 4.2V

// ==========================================================================================================================
// Variables
// ==========================================================================================================================
int soc = 0;              // Store SOC measurement (State of charge)
int no_animation_cnt = 0; // count wake-ups
int operation_mode = 1;   // Track operation mode

// ==========================================================================================================================
// Constants
// ==========================================================================================================================
static uint8_t SIGMOID[NUM_SIGMOID_VALUES] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 21, 23, 25, 27, 30, 33, 36, 39,
    42, 46, 50, 54, 58, 63, 68, 73, 79, 84, 90, 96, 102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176,
    181, 186, 191, 196, 200, 205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246,
    247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 255,
    255, 255, 255, 255, 255};

// ==========================================================================================================================
// Function declarations
// ==========================================================================================================================
void play_led_wave(uint8_t mode);

// ==========================================================================================================================
// Setup
// ==========================================================================================================================
void setup()
{
    // Disable interrupts before changing them
    cli();

    // Set all pins to low power mode
    for (uint8_t pin = 0; pin < 8; pin++)
    {
        *((uint8_t *)&PORTA + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp; // Enable all pullups on port A
        *((uint8_t *)&PORTB + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp; // Enable all pullups on port B
        *((uint8_t *)&PORTC + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp; // Enable all pullups on port C
        (&PORTA.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;         // Disable interrupt and digital input buffer on port A
        (&PORTB.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;         // Disable interrupt and digital input buffer on port B
        (&PORTC.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;         // Disable interrupt and digital input buffer on port C
    }
    pinMode(PIN_SUPERCAP_MEASUREMENT, INPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);

    // Wait for all register to be synchronized
    while (RTC.STATUS > 0)
        ;

    // RTC configuration
    RTC.PER = 1024 * SLEEP_SECONDS;                                               // n secs between wakes.
    RTC.INTCTRL = 0 << RTC_CMP_bp | 1 << RTC_OVF_bp;                              // Overflow interrupt
    RTC.CTRLA = RTC_PRESCALER_DIV1_gc | 1 << RTC_RTCEN_bp | 1 << RTC_RUNSTDBY_bp; // No Prescaler + Enable RTC + Run in standby
    RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;                                             // 32KHz divided by 32, i.e run at 1.024kHz

    // Enable interrupts
    sei();
}

// ==========================================================================================================================
// Loop
// ==========================================================================================================================
void loop()
{
    // Turn on ADC and wait for 10 ms
    ADC0_CTRLA |= ADC_ENABLE_bm;
    delay(10);

    // On button press change mode
    if (analogRead(PIN_BUTTON) <= 10)
    {
        operation_mode = (operation_mode + 1) % 3;
        blink_led(operation_mode + 1);
        delay(1000);
    }

    // Check operating mode and blink accordingly
    if (operation_mode > 0)
    {
        read_soc();
        if (operation_mode == 1)
        {

            blink_led_pretty_according_to_soc();
        }
        else if (operation_mode == 2)
        {
            if (no_animation_cnt + 1 >= ANIMATION_OCCURS)
            {
                no_animation_cnt = 0;
                if (soc <= ADC_VALUE_1_3_CHARGED)
                    play_led_wave(0);
                else if (soc <= ADC_VALUE_2_3_CHARGED)
                    play_led_wave(1);
                else
                    play_led_wave(2);
            }
            else
            {
                no_animation_cnt += 1;
                blink_led_pretty_according_to_soc();
            }
        }
    }

    // Go to sleep
    ADC0_CTRLA &= ~ADC_ENABLE_bm;       // Turn off ADC
    set_sleep_mode(SLEEP_MODE_STANDBY); // Set standby sleep mode
    sleep_enable();                     // Enable sleeping
    sleep_cpu();                        // Sleep until woken up by ISR
}

// ==========================================================================================================================
// ISR (Interrupt Service Routine)
// ==========================================================================================================================
ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_OVF_bm;
}

// ==========================================================================================================================
// Functions
// ==========================================================================================================================
void play_led_wave(uint8_t mode = 0)
{
    int delayed = 0, delayed2 = 0;
    int a = 0, b = 0, c = 0;
    for (int i = 0; i < NUM_SIGMOID_VALUES * 2 + 140; i += 1)
    {
        a = i;
        b = delayed;
        c = delayed2;
        if (a >= NUM_SIGMOID_VALUES * 2 - 1)
        {
            a = 0;
        }
        else if (a >= NUM_SIGMOID_VALUES)
        {
            a = NUM_SIGMOID_VALUES - 1 - (a % NUM_SIGMOID_VALUES);
        }
        if (a < 0)
        {
            a = 0;
        }
        if (b >= NUM_SIGMOID_VALUES * 2 - 1)
        {
            b = 0;
        }
        else if (b >= NUM_SIGMOID_VALUES)
        {
            b = NUM_SIGMOID_VALUES - 1 - (b % NUM_SIGMOID_VALUES);
        }
        if (b < 0)
        {
            b = 0;
        }
        if (c >= NUM_SIGMOID_VALUES * 2 - 1)
        {
            c = 0;
        }
        else if (c >= NUM_SIGMOID_VALUES)
        {
            c = NUM_SIGMOID_VALUES - 1 - (c % NUM_SIGMOID_VALUES);
        }
        if (c < 0)
        {
            c = 0;
        }
        if (mode == 0)
        { // Down
            set_leds_analog(SIGMOID[a], SIGMOID[b], SIGMOID[c]);
        }
        else if (mode == 1)
        { // Up
            set_leds_analog(SIGMOID[c], SIGMOID[b], SIGMOID[a]);
        }
        else
        { // Center
            set_leds_analog(SIGMOID[b], SIGMOID[a] + SIGMOID[c], SIGMOID[b]);
        }
        delay(ANIMATION_DELAY);

        if (i >= 70)
        {
            delayed++;
        }
        if (i >= 140)
        {
            delayed2++;
        }
    }
}

void blink_led_pretty_according_to_soc()
{
    if (soc <= ADC_VALUE_1_3_CHARGED)
    {
        blink_led_pretty(3);
    }
    else if (soc <= ADC_VALUE_2_3_CHARGED)
    {
        blink_led_pretty(2);
    }
    else
    {
        blink_led_pretty(1);
    }
}

void set_leds_analog(int led1, int led2, int led3)
{
    analogWrite(PIN_LED1, led1);
    analogWrite(PIN_LED2, led2);
    analogWrite(PIN_LED3, led3);
}

void set_leds_digital(int high_low1, int high_low2, int high_low3)
{
    digitalWrite(PIN_LED1, high_low1);
    digitalWrite(PIN_LED2, high_low2);
    digitalWrite(PIN_LED3, high_low3);
}

void blink_led(uint8_t which_one)
{
    // Turn LED on, wait for BLINK_DELAY, turn LED off
    set_leds_digital(
        which_one == 1 || which_one == 0,
        which_one == 2 || which_one == 0,
        which_one == 3 || which_one == 0);
    delay(BLINK_DELAY);
    set_leds_digital(LOW, LOW, LOW);
}

void blink_led_pretty(uint8_t which_one)
{
    // Use sigmoid function to turn an led on and off
    int a = 0;
    for (int i = 0; i < NUM_SIGMOID_VALUES * 2; i += 1)
    {
        a = i;
        if (a >= NUM_SIGMOID_VALUES)
        {
            a = NUM_SIGMOID_VALUES - 1 - (a % NUM_SIGMOID_VALUES);
        }

        if (which_one == 1)
        {
            set_leds_analog(SIGMOID[a], 0, 0);
        }
        else if (which_one == 2)
        {
            set_leds_analog(0, SIGMOID[a], 0);
        }
        else if (which_one == 3)
        {
            set_leds_analog(0, 0, SIGMOID[a]);
        }
        delay(ANIMATION_DELAY * 2);
    }
}

void read_soc()
{
    // Write analog value from ADC to soc variable (0-4095)
    soc = analogRead(PIN_SUPERCAP_MEASUREMENT);
}
