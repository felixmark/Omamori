/*
    Omamori Program v1.0
    
    Author: Felix Mark
    Date:   14.03.2021

    Serial Protocol:
    <cmd>,<values (optional)>;
    
    So for instance:
    rgbw,<red byte>,<green byte>,<blue byte>,<white byte>,<brightness byte>;

    Available Modes are:
    0 - rgbw
*/



// ============================================================= INCLUDES
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>



// ============================================================= DEFINES
#define SERIAL_BAUD     9600
// Pins
#define PIN_LED         PCINT0
#define PIN_LED_ENABLE  PCINT1
#define PIN_RX          PCINT2
#define PIN_TX          PCINT3
#define PIN_SOLAR_CELL  PCINT4
// OTHERS
#define SLEEP_CYCLES    15



// ============================================================= CONSTANTS
const uint8_t SIGMOID [] = {
    1,   2,   3,   4,   5,   6,
    7,   8,   9,   10,  11,  12,  13,  14,  16,  17,  19,  21,  23,  25,  27,  30,  33,  36,  39,  42,  46,  50,  54,  58, 
    63,  68,  73,  79,  84,  90,  96,  102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176, 181, 186, 191, 196, 200,
    205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 248, 249, 249, 
    250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255
};



// ============================================================= VARIABLES
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, PIN_LED, NEO_GRBW + NEO_KHZ800);
SoftwareSerial mySerial(PIN_RX, PIN_TX);
volatile bool awake = true;
uint8_t sleep_cycle_counter = 0;

uint16_t brightness = 0;
uint8_t mode = 0;
uint8_t color_r = 50;
uint8_t color_g = 0;
uint8_t color_b = 10;
uint8_t color_w = 10;
uint8_t color_brightness = 20;
// EEPROM
uint8_t mode_eeprom EEMEM;
uint8_t color_r_eeprom EEMEM;
uint8_t color_g_eeprom EEMEM;
uint8_t color_b_eeprom EEMEM;
uint8_t color_w_eeprom EEMEM;
uint8_t color_brightness_eeprom EEMEM;



// ============================================================= SETUP 
void setup() {
    // Setup LED
    led.begin();
    led.show();
    
    // Setup Pins
    pinMode(PIN_SOLAR_CELL, INPUT);
    pinMode(PIN_LED_ENABLE, OUTPUT);
    
    // Setup WDT
    cli();                  // Disable Interrupts
    wdt_reset();            // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);  // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);    // Watchdog Change Enable
    WDTCR |= 1 << WDP0 | 1 << WDP3; // Set WDT timeout to 8 seconds
    WDTCR |= (1 << WDIE);   // Watchdog Timeout Interrupt Enable
    sei();                  // Enable Interrupts
    
    // Restore variables from EEPROM
    mode = eeprom_read_byte(&mode_eeprom);
    if (mode != 0xFF) {
        color_r = eeprom_read_byte(&color_r_eeprom);
        color_g = eeprom_read_byte(&color_g_eeprom);
        color_b = eeprom_read_byte(&color_b_eeprom);
        color_w = eeprom_read_byte(&color_w_eeprom);
        color_brightness = eeprom_read_byte(&color_brightness_eeprom);
    } else {
        mode = 0;
    }
}



void loop() {
    if (awake && sleep_cycle_counter == 0) {
        sleep_cycle_counter = SLEEP_CYCLES;
        
        // handle serial communication
        handle_serial();
  
        // Check Mode
        if (mode == 0) {
            // Mode = RGBW

            //brightness = analogRead(PIN_SOLAR_CELL);
            //brightness = (brightness * color_brightness) / 1024;

            uint8_t r_offset = color_r > 0 ? 1 : 0;
            uint8_t g_offset = color_g > 0 ? 1 : 0;
            uint8_t b_offset = color_b > 0 ? 1 : 0;
            uint8_t w_offset = color_w > 0 ? 1 : 0;
            
            digitalWrite(PIN_LED_ENABLE, HIGH);
            for (int i = 0; i < sizeof SIGMOID / sizeof SIGMOID[0]; ++i) {
                
                show_color(
                    r_offset + (color_r * (long) SIGMOID[i]) / 254, 
                    g_offset + (color_g * (long) SIGMOID[i]) / 254, 
                    b_offset + (color_b * (long) SIGMOID[i]) / 254,
                    w_offset + (color_w * (long) SIGMOID[i]) / 254
                );
                delay(15);
            }
            for (int i = (sizeof SIGMOID / sizeof SIGMOID[0]) - 1; i >= 0; --i) {
                show_color(
                  r_offset + (color_r * (long) SIGMOID[i]) / 254, 
                  g_offset + (color_g * (long) SIGMOID[i]) / 254, 
                  b_offset + (color_b * (long) SIGMOID[i]) / 254,
                  w_offset + (color_w * (long) SIGMOID[i]) / 254
                );
                delay(15);
            }
            show_color(0,0,0,0);
            digitalWrite(PIN_LED_ENABLE, LOW);
        }
    } else {
        digitalWrite(PIN_LED_ENABLE, HIGH);
        show_color(color_r, color_g, color_b, color_w);
        delay(1);
        show_color(0,0,0,0);
        digitalWrite(PIN_LED_ENABLE, LOW);
    }
    
    go_to_bed();
}



// ============================================================= WDT ISR
ISR(WDT_vect) {
    awake = true;
}



// ============================================================= ENTER SLEEP
void go_to_bed() {
    wdt_reset();
    sleep_cycle_counter -= 1;
    awake = false;
    mySerial.end();

    // Prepare for bed
    byte adcsra;
    adcsra = ADCSRA;                    // ADC Control and Status Register A sichern
    ADCSRA &= ~(1 << ADEN);             // ADC ausschalten
    MCUCR |= (1 << SM1) & ~(1 << SM0);  // Sleep-Modus = Power Down
    MCUCR |= (1 << SE);                 // Sleep Enable setzen

    // Sleep
    sleep_cpu();                        // Sleeping

    // Wake up
    MCUCR &= ~(1 << SE);                // Sleep Disable setzen
    ADCSRA = adcsra;                    // ADCSRA-Register laden
}



// ============================================================= SHOW COLOR
void show_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
    led.setPixelColor(0, red, green, blue, white);
    led.show();
}



// ============================================================= GET VALUE FROM A STRING 
String get_value(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}



// ============================================================= EETPROM FUNCTIONS
void update_eeprom(uint8_t &variable, uint8_t value) {
    if (eeprom_read_byte(&variable) != value) {
        eeprom_write_byte(&variable, value);
    }
}



// ============================================================= SERIAL COMMUNICATION
void handle_serial() {
    mySerial.begin(SERIAL_BAUD);
    unsigned long start_time = millis();
    while (millis() > start_time && millis() - start_time < 500 || millis() < start_time && millis() + (4294967295 - start_time) < 500) {

        // Wait for incomming serial bytes
        if (mySerial.available() == 0) {
          delay(100);
          continue;
        }

        // Read first line
        String cmd = mySerial.readStringUntil(',');
  
        mySerial.print("Received cmd: ");
        mySerial.println(cmd);
  
        if (cmd == "rgbw") {
            mode = 0;
            
            String values = mySerial.readStringUntil(';');
            color_r = get_value(values, ',', 0).toInt();
            color_g = get_value(values, ',', 1).toInt();
            color_b = get_value(values, ',', 2).toInt();
            color_w = get_value(values, ',', 3).toInt();
            color_brightness = get_value(values, ',', 4).toInt();
  
            mySerial.println("Received data.");
            mySerial.print("R: ");
            mySerial.println(color_r);
            mySerial.print("G: ");
            mySerial.println(color_g);
            mySerial.print("B: ");
            mySerial.println(color_b);
            mySerial.print("W: ");
            mySerial.println(color_w);
            mySerial.print("Bright: ");
            mySerial.println(color_brightness);
  
            // Update data in EEPROM if necessary
            update_eeprom(mode_eeprom, mode);
            update_eeprom(color_r_eeprom, color_r);
            update_eeprom(color_g_eeprom, color_g);
            update_eeprom(color_b_eeprom, color_b);
            update_eeprom(color_w_eeprom, color_w);
            update_eeprom(color_brightness_eeprom, color_brightness);
        }
    }
}
