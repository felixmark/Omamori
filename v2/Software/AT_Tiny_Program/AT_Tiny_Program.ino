/*
    Omamori Program v1.0
    
    Author: Felix Mark
    Date:   14.03.2021

    Serial Protocol:
    <mode>
    <values (optional)>
    
    So for instance:
    rgbw
    <red byte>, <green byte>, <blue byte>, <white byte>, <brightness byte>

    Available Modes are:
    0 - rgbw
*/


// ================================================= INCLUDES =================================================
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>


// ================================================= DEFINES =================================================
// Pins and other
#define PIN_LED         PCINT0
#define PIN_BATT_OK     PCINT1
#define PIN_RX          PCINT2
#define PIN_TX          PCINT3


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
SoftwareSerial mySerial(PIN_RX, PIN_TX);
volatile bool awake = true;

// Variables
uint8_t mode = 0;
uint8_t color_r = 0;
uint8_t color_g = 0;
uint8_t color_b = 0;
uint8_t color_w = 255;
uint8_t color_brightness = 50;
// EEPROM
uint8_t mode_eeprom EEMEM;
uint8_t color_r_eeprom EEMEM;
uint8_t color_g_eeprom EEMEM;
uint8_t color_b_eeprom EEMEM;
uint8_t color_w_eeprom EEMEM;
uint8_t color_brightness_eeprom EEMEM;


// ============================================================= MAIN
int main() {

    // ============================================================= SETUP 
    // Setup LED
    led.begin();
    led.show();

    // Setup Serial
    mySerial.begin(9600);

    // Setup WDT
    cli();                  // Disable Interrupts
    wdt_reset();            // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);  // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);    // Watchdog Change Enable
    WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP1); // Set WDT timeout to 8 seconds
    WDTCR |= (1 << WDIE);   // Watchdog Timeout Interrupt Enable
    sei();                  // Enable Interrupts

    // Restore variables from EEPROM
    mode = eeprom_read_byte(&mode_eeprom);
    color_r = eeprom_read_byte(&color_r_eeprom);
    color_g = eeprom_read_byte(&color_g_eeprom);
    color_b = eeprom_read_byte(&color_b_eeprom);
    color_w = eeprom_read_byte(&color_w_eeprom);
    color_brightness = eeprom_read_byte(&color_brightness_eeprom);


    // ============================================================= LOOP 
    while (true) {
      if (awake && digitalRead(PIN_BATT_OK) == HIGH) {
        
          unsigned long start_time = millis();
          while (mySerial.available() > 0 && (
              (millis() > start_time && millis() - start_time < 500) || 
              (millis() < start_time && millis() + (4294967295 - start_time) < 500)
          )){
              String new_mode = mySerial.readStringUntil('\r\n');

              mySerial.print("Received mode: ");
              mySerial.println(new_mode);
      
              if (new_mode == "rgbw") {
                  mode = 0;
                  
                  String values = mySerial.readStringUntil('\r\n');
                  color_r = getValue(values, ',', 0).toInt();
                  color_g = getValue(values, ',', 1).toInt();
                  color_b = getValue(values, ',', 2).toInt();
                  color_w = getValue(values, ',', 3).toInt();
                  color_brightness = getValue(values, ',', 4).toInt();

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
                  updateEEPROM(mode_eeprom, mode);
                  updateEEPROM(color_r_eeprom, color_r);
                  updateEEPROM(color_g_eeprom, color_g);
                  updateEEPROM(color_b_eeprom, color_b);
                  updateEEPROM(color_w_eeprom, color_w);
                  updateEEPROM(color_brightness_eeprom, color_brightness);
              }
          }


          // ============================================================= CHECK MODE AND EXECUTE 

          // MODE = RGBW
          if (mode == 0) {
              for (int i = 0; i < sizeof SIGMOID / sizeof SIGMOID[0]; ++i) {
                  showColor(color_r, color_g, color_b, color_w, (byte) (((long) SIGMOID[i]) * 255) / color_brightness);  // Sigmoid up
              }
              for (int i = (sizeof SIGMOID / sizeof SIGMOID[0]) - 1; i >= 0; --i) {
                  showColor(color_r, color_g, color_b, color_w, (byte) (((long) SIGMOID[i]) * 255) / color_brightness);  // Sigmoid down
              }
          }

          // Go to sleep afterwards
          goToSleep();
      } else if (awake) {
          // ============================================================= BATTERY STATUS != OK
          // Supercap charge is still too low -> Go to sleep and check on wakeup
          goToSleep();
      }
    }
    
    return 0;
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


// ================================================= GET VALUE FROM A STRING =================================================
String getValue(String data, char separator, int index) {
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


// ================================================= EETPROM FUNCTIONS =================================================
void updateEEPROM(uint8_t &variable, uint8_t value) {
    if (eeprom_read_byte(&variable) != value) {
        eeprom_write_byte(&variable, value);
    }
}
