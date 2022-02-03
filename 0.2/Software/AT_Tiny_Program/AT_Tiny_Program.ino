/*
    Omamori Program v1.0
    
    Author: Felix Mark
    Date:   23.05.2021

         AT TINY PINOUT
             ______
       RST -|O     |- VCC
    LED EN -|      |- MEASURE 
       LED -|      |- RX
       GND -|______|- TX
    
    Serial Protocol:
    <cmd>:<values (if needed)>;
    
    List of commands:
    STA;                                                    // Get Status
    COL:<red byte>,<green byte>,<blue byte>,<white byte>;   // Set Color
    BAT;                                                    // Set Mode to Battery display
    DEF;                                                    // Set Mode to Default
*/



// ============================================================= INCLUDES
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>



// ============================================================= DEFINES
#define VERSION             "1.0"
#define SERIAL_BAUD         19200
#define MAXIMUM_BRIGHTNESS  70
#define BLINK_ON_TIME       10
#define ANIMATION_DELAY     15
#define NUM_ELEMENTS        105
#define SERIAL_TIMEFRAME    5000
#define SERIAL_BUFFER_SIZE  30
#define ISR_CNT_TO_WAKE_UP  2

// Pins
#define PIN_LED             PCINT4
#define PIN_LED_ENABLE      PCINT3
#define PIN_MEASUREMENT     A1
#define PIN_RX              PCINT1
#define PIN_TX              PCINT0

// Other
#define POS_RED             0
#define POS_GRN             1
#define POS_BLU             2
#define POS_WHT             3

// Modes
#define MODE_DEFAULT        0
#define MODE_BATTERY        1



// ============================================================= CONSTANTS
static uint8_t SIGMOID [NUM_ELEMENTS] = {
    1,   2,   3,   4,   5,   6,
    7,   8,   9,   10,  11,  12,  13,  14,  16,  17,  19,  21,  23,  25,  27,  30,  33,  36,  39,  42,  46,  50,  54,  58,
    63,  68,  73,  79,  84,  90,  96,  102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176, 181, 186, 191, 196, 200,
    205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 248, 249, 249, 
    250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 255
};



// ============================================================= VARIABLES
Adafruit_NeoPixel led = Adafruit_NeoPixel(1, PIN_LED, NEO_GRBW + NEO_KHZ800);
SoftwareSerial mySerial(PIN_RX, PIN_TX);

volatile bool awake = false;
uint8_t current_sleep_cycles = 1; // Between 1 and 10 cycles = 8 to 80 seconds sleep
int8_t sleep_cycle_counter = 1;
uint8_t isr_cnt = 0;              // Short blink every 8 * isr_cnt seconds
int soc = 0;
uint8_t serial_process_position = 0;

uint8_t mode = MODE_DEFAULT;
uint8_t color_r = 255;
uint8_t color_g = 0;
uint8_t color_b = 80;
uint8_t color_w = 100;

uint8_t offsets[4] = {0,0,0,0};

// EEPROM
uint8_t mode_eeprom EEMEM;
uint8_t color_r_eeprom EEMEM;
uint8_t color_g_eeprom EEMEM;
uint8_t color_b_eeprom EEMEM;
uint8_t color_w_eeprom EEMEM;
uint8_t color_set_eeprom EEMEM;



// ============================================================= SETUP 
void setup() {
    // Setup LED
    led.begin();
    led.show();
    
    // Setup Pins
    pinMode(PIN_MEASUREMENT, INPUT);
    pinMode(PIN_LED_ENABLE, OUTPUT);

    // Setup WDT
    cli();                  // Disable Interrupts
    wdt_reset();            // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);  // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);    // Watchdog Change Enable
    WDTCR |= 1 << WDP0 | 1 << WDP3; // Set WDT timeout to 8 seconds
    WDTCR |= (1 << WDIE);   // Watchdog Timeout Interrupt Enable
    sei();                  // Enable Interrupts

    // Restore variables from EEPROM if they are set
    uint8_t temp_mode = eeprom_read_byte(&mode_eeprom);
    if (temp_mode != 0xFF) {
        mode = temp_mode;
    }
    if (eeprom_read_byte(&color_set_eeprom) != 0xFF) {
        color_r = eeprom_read_byte(&color_r_eeprom);
        color_g = eeprom_read_byte(&color_g_eeprom);
        color_b = eeprom_read_byte(&color_b_eeprom);
        color_w = eeprom_read_byte(&color_w_eeprom);
    }

    // Handle Serial communication
    handle_serial();
    
    // Calculate offsets
    offsets[POS_RED] = color_r > 0 ? 1 : 0;
    offsets[POS_GRN] = color_g > 0 ? 1 : 0;
    offsets[POS_BLU] = color_b > 0 ? 1 : 0;
    offsets[POS_WHT] = color_w > 0 ? 1 : 0;
}



void loop() {
    if (awake) {
        
        // Measure state of charge
        read_soc();
      
        if (sleep_cycle_counter <= 0) {
            // After some sleep cycles

            enable_led();

            int8_t pos = 0;
            for (; pos < NUM_ELEMENTS; ++pos) {
                uint16_t value = SIGMOID[pos];
                show_color(
                    offsets[POS_RED] + (((color_r * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_GRN] + (((color_g * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_BLU] + (((color_b * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_WHT] + (((color_w * value) / 255) * MAXIMUM_BRIGHTNESS) / 254
                );
                delay(ANIMATION_DELAY);
            }
            for (pos = NUM_ELEMENTS - 1; pos >= 0; --pos) {
                uint16_t value = SIGMOID[pos];
                show_color(
                    offsets[POS_RED] + (((color_r * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_GRN] + (((color_g * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_BLU] + (((color_b * value) / 255) * MAXIMUM_BRIGHTNESS) / 254,
                    offsets[POS_WHT] + (((color_w * value) / 255) * MAXIMUM_BRIGHTNESS) / 254
                );
                delay(ANIMATION_DELAY);
            }
            disable_led();
          
            // Determine next sleep duration and set sleep_cycle_counter
            current_sleep_cycles = map(soc, 0, 1023, 10, 1);
            sleep_cycle_counter = current_sleep_cycles;
        } else {
            // Short blink every 8 seconds
            if (mode == MODE_DEFAULT) {
                blink_led(
                    offsets[POS_RED] + (color_r * MAXIMUM_BRIGHTNESS / 4) / 254,
                    offsets[POS_GRN] + (color_g * MAXIMUM_BRIGHTNESS / 4) / 254,
                    offsets[POS_BLU] + (color_b * MAXIMUM_BRIGHTNESS / 4) / 254,
                    offsets[POS_WHT] + (color_w * MAXIMUM_BRIGHTNESS / 4) / 254
                );
            } else {
                if (soc < 614) {
                    blink_led(30,0,0,0);
                } else if (soc < 818) {
                    blink_led(0,0,0,30);
                } else {
                    blink_led(0,30,0,0);
                }
            }
        }   
    }
    go_to_bed();
}




// ============================================================= SERIAL COMMUNICATION
void handle_serial() {
    blink_led(0,0,0,30);
    mySerial.begin(SERIAL_BAUD);
    
    char recv_buffer[SERIAL_BUFFER_SIZE] = {};
    char character = 0;

    // Send current color for synchronizing Omamoris
    mySerial.println("Omamori v" VERSION);
    print_current_color();

    // Wait for incomming serial bytes
    while (millis() < SERIAL_TIMEFRAME) {
        // Read first line
        if (mySerial.available() > 0) {
            uint8_t pos = 0;
            serial_process_position = 0;
            
            while (mySerial.available() > 0 && pos < SERIAL_BUFFER_SIZE) {
                character = mySerial.read();
                recv_buffer[pos++] = character;
                delay(5);
            }

            blink_led(0,50,0,0);
            delay(10);
            
            mySerial.print("Received: ");
            for (uint8_t i = 0; i < pos; ++i) {
                mySerial.print(recv_buffer[i]);
            }
            mySerial.println();

            if (recv_buffer[0] == 'C' && recv_buffer[1] == 'O' && recv_buffer[2] == 'L') {
                serial_process_position = 4;
                
                color_r = get_value(recv_buffer, serial_process_position, ',');
                color_g = get_value(recv_buffer, serial_process_position, ',');
                color_b = get_value(recv_buffer, serial_process_position, ',');
                color_w = get_value(recv_buffer, serial_process_position, ';');

                // Update data in EEPROM if necessary
                update_eeprom(color_set_eeprom, 1);
                update_eeprom(color_r_eeprom, color_r);
                update_eeprom(color_g_eeprom, color_g);
                update_eeprom(color_b_eeprom, color_b);
                update_eeprom(color_w_eeprom, color_w);
            }
            else if (recv_buffer[0] == 'B' && recv_buffer[1] == 'A' && recv_buffer[2] == 'T') {
                mode = MODE_BATTERY;
                update_eeprom(mode_eeprom, mode);
                mySerial.println("Mode: Battery");
            }
            else if (recv_buffer[0] == 'D' && recv_buffer[1] == 'E' && recv_buffer[2] == 'F') {
                mode = MODE_DEFAULT;
                update_eeprom(mode_eeprom, mode);
                mySerial.println("Mode: Default");
            }
            else if (recv_buffer[0] == 'S' && recv_buffer[1] == 'T' && recv_buffer[2] == 'A') {
                mySerial.println("Omamori v" VERSION);
                read_soc();
                mySerial.print("SOC: ");
                mySerial.println(soc);
                print_current_color();
            }
            else if (recv_buffer[0] == 'F' && recv_buffer[1] == 'J' && recv_buffer[2] == 'M') {
                mySerial.println("UwU");
            }
            else {
                mySerial.println("Unknown command.");
            }

            // Serial Request has been processed. Print new line.
            mySerial.println("");
            break;
        }
    }
    
    blink_led(0,0,0,30);
    mySerial.end();
}

// ============================================================= FINISHED FUNCTIONS

// WDT ISR
ISR(WDT_vect) {
    isr_cnt += 1;
    if (isr_cnt >= ISR_CNT_TO_WAKE_UP) {
        awake = true;
        isr_cnt = 0;
    }
}

// ENTER SLEEP
void go_to_bed() {
    wdt_reset();
    sleep_cycle_counter -= 1;
    awake = false;

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

// SHOW COLOR
void show_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
    led.setPixelColor(0, red, green, blue, white);
    led.show();
}

// ENABLE LED
void enable_led() {
    digitalWrite(PIN_LED_ENABLE, HIGH);
    delay(10);
}

// DISABLE LED
void disable_led() {
    show_color(0,0,0,0);
    delay(10);
    digitalWrite(PIN_LED_ENABLE, LOW);
}

// GET SOC
void read_soc() {
    soc = analogRead(PIN_MEASUREMENT);
}

// BLINK LED
void blink_led(uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
    enable_led();
    show_color(red, green, blue, white);
    delay(BLINK_ON_TIME);
    disable_led();
}

// PRINT CURRENT COLOR
void print_current_color() {
    mySerial.print("COL:");
    mySerial.print(color_r);
    mySerial.print(",");
    mySerial.print(color_g);
    mySerial.print(",");
    mySerial.print(color_b);
    mySerial.print(",");
    mySerial.print(color_w);
    mySerial.println(";");
}

// GET VALUE FROM A STRING
uint8_t get_value(char data[SERIAL_BUFFER_SIZE], uint8_t begin_pos, char end_char) {
    uint8_t value = 0;
    uint8_t digit_cnt = 0;
    for (int i = begin_pos; data[i] != end_char && i < SERIAL_BUFFER_SIZE; ++i) {
        digit_cnt += 1;
    }
    for (int i = begin_pos; i < begin_pos + digit_cnt; ++i) {
        uint8_t next_digit = ((uint8_t) (data[i] - '0'));
        
        uint8_t multiplikator = 1;
        uint8_t exponent = (digit_cnt - 1) - (i - begin_pos);
        for (int u = 0; u < exponent; ++u) {
            multiplikator *= 10;
        }
        next_digit *= multiplikator;
        value += next_digit;
    }
    
    serial_process_position += digit_cnt + 1;
    return value;
}

// EETPROM FUNCTIONS
void update_eeprom(uint8_t &variable, uint8_t value) {
    if (eeprom_read_byte(&variable) != value) {
        eeprom_write_byte(&variable, value);
    }
}
