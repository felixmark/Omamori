/*
    Omamori Program v1.0
    
    Author: Felix Mark
    Date:   14.03.2021
*/


// ================================================= INCLUDES =================================================
#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>


// ================================================= DEFINES =================================================
// Personalization
#define C_R         255
#define C_G         0
#define C_B         0
#define C_W         100
#define BRIGHT_PERC 40
// Pins and other
#define PIN_LED     0
#define PIN_BATT_OK 1


// ================================================= CONSTS =================================================
const byte SIGMOID [] = {
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3,   3,   3,   4,   4,   4,   5,   5,   6,
    7,   7,   8,   9,   10,  11,  12,  13,  14,  16,  17,  19,  21,  23,  25,  27,  30,  33,  36,  39,  42,  46,  50,  54,  58, 
    63,  68,  73,  79,  84,  90,  96,  102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176, 181, 186, 191, 196, 200,
    205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 248, 249, 249, 
    250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};


// ================================================= VARIABLES =================================================
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, PIN_LED, NEO_GRBW + NEO_KHZ800);
volatile bool awake = true;


// ================================================= SETUP =================================================
void setup() {
    // Setup LED
    led.begin();
    led.show();

    // Setup WDT
    cli();                  // Disable Interrupts
    wdt_reset();            // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);  // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);    // Watchdog Change Enable
    WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP1); // Set WDT timeout to 8 seconds
    WDTCR |= (1 << WDIE);   // Watchdog Timeout Interrupt Enable
    sei();                  // Enable Interrupts
}


// ================================================= LOOP =================================================
void loop() {
    if (awake && digitalRead(PIN_BATT_OK) == HIGH) {
        for (int i = 0; i < sizeof SIGMOID / sizeof SIGMOID[0]; ++i) {
            showColor(C_R, C_G, C_B, C_W, (byte) (((long) SIGMOID[i]) * 100) / BRIGHT_PERC);  // Sigmoid up
        }
        for (int i = (sizeof SIGMOID / sizeof SIGMOID[0]) - 1; i >= 0; --i) {
            showColor(C_R, C_G, C_B, C_W, (byte) (((long) SIGMOID[i]) * 100) / BRIGHT_PERC);  // Sigmoid down
        }
        goToSleep();
    } else if (awake) {
        goToSleep();  // Supercap charge is still too low -> Go to sleep and check afterwards
    }
}


// ================================================= WDT ISR =================================================
ISR(WDT_vect) {
    awake = true;   // Set awake flag for loop function
}


// ================================================= ENTER SLEEP =================================================
void goToSleep() {
    awake = false;  // Set awake flag to false for loop function
    
    byte adcsra;
    adcsra = ADCSRA;                    // ADC Control and Status Register A sichern
    ADCSRA &= ~(1 << ADEN);             // ADC ausschalten
    MCUCR |= (1 << SM1) & ~(1 << SM0);  // Sleep-Modus = Power Down
    MCUCR |= (1 << SE);                 // Sleep Enable setzen
    sleep_cpu();                        // Sleeping
    MCUCR &= ~(1 << SE);                // Sleep Disable setzen
    ADCSRA = adcsra;                    // ADCSRA-Register laden
}


// ================================================= SHOW COLOR =================================================
void showColor(byte red, byte green, byte blue, byte white, byte brightness) {
    led.setPixelColor(PIN_LED, red, green, blue, white);
    led.setBrightness(brightness);
    led.show();
}
