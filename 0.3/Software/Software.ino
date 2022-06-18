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

    VCC: 1.8V -> 10MHz Clock Rate
    
    Serial Protocol:
    <cmd>:<values (if needed)>;
    
    List of commands:
    STA;                                                    // Get Status
    SLP:<1-100>;                                            // Set sleep time between blinks (multiplied by 8s)
*/



// ============================================================= INCLUDES
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>



// ============================================================= DEFINES
#define VERSION             "1.1"
#define SERIAL_BAUD         19200
#define MAXIMUM_BRIGHTNESS  255
#define BLINK_ON_TIME       10
#define ANIMATION_DELAY     15
#define NUM_ELEMENTS        105
#define SERIAL_TIMEFRAME    5000
#define SERIAL_BUFFER_SIZE  30

// Pins
#define PIN_LED             PCINT4
#define PIN_LED2            PCINT3
#define PIN_MEASUREMENT     A1
#define PIN_RX              PCINT1
#define PIN_TX              PCINT0



// ============================================================= CONSTANTS
static uint8_t SIGMOID [NUM_ELEMENTS] = {
    1,   2,   3,   4,   5,   6,
    7,   8,   9,   10,  11,  12,  13,  14,  16,  17,  19,  21,  23,  25,  27,  30,  33,  36,  39,  42,  46,  50,  54,  58,
    63,  68,  73,  79,  84,  90,  96,  102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176, 181, 186, 191, 196, 200,
    205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 248, 249, 249, 
    250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255
};



// ============================================================= VARIABLES
SoftwareSerial mySerial(PIN_RX, PIN_TX);

volatile bool awake = false;          // Used for waking up in ISR (Interrupt Service Routine)
uint8_t current_sleep_cycles = 1;     // Between 1 and 10 cycles = 8 to 80 seconds sleep
int8_t sleep_cycle_counter = 1;       // Counting how often uC woke up and blinked shortly
uint8_t isr_cnt = 0;                  // Short blink every 8 * isr_cnt seconds
uint8_t isr_cnt_to_wake_up = 2;       // How many interrupts needed to wake up (isr_cnt_to_wake_up * 8s)
int soc = 0;                          // Store SOC measurement (State of charge)
uint8_t serial_process_position = 0;  // How much of the received serial string has been processed

// EEPROM
uint8_t stored_message_eeprom EEMEM;
uint8_t isr_cnt_to_wake_up_eeprom EEMEM;



// ============================================================= SETUP 
void setup() {
    // Setup Pins
    pinMode(PIN_MEASUREMENT, INPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    set_leds_digital(LOW, LOW);

    // Setup WDT
    cli();                          // Disable Interrupts
    wdt_reset();                    // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);          // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);            // Watchdog Change Enable
    WDTCR |= 1 << WDP0 | 1 << WDP3; // Set WDT timeout to 8 seconds
    WDTCR |= (1 << WDIE);           // Watchdog Timeout Interrupt Enable
    sei();                          // Enable Interrupts

    // Restore variables from EEPROM if they are set
    uint8_t temp_isr_cnt_to_wake_up = eeprom_read_byte(&isr_cnt_to_wake_up_eeprom);
    if (temp_isr_cnt_to_wake_up != 0xFF) {
        isr_cnt_to_wake_up = temp_isr_cnt_to_wake_up;
    }

    // Handle Serial communication
    handle_serial();
}



void loop() {
    if (awake) {
        
        // Measure state of charge
        read_soc();
      
        if (sleep_cycle_counter <= 0) {
            // After some (defined) sleep cycles

            int8_t pos = 0;
            for (; pos < NUM_ELEMENTS; ++pos) {
                uint16_t value = SIGMOID[pos];
                set_leds_analog(value, value); // TODO Offset LED 2 by ~35 positions
                delay(ANIMATION_DELAY);
            }
            for (pos = NUM_ELEMENTS - 1; pos >= 0; --pos) {
                uint16_t value = SIGMOID[pos];
                set_leds_analog(value, value); // TODO Offset LED 2 by ~35 positions
                delay(ANIMATION_DELAY);
            }
          
            // Determine next sleep duration and set sleep_cycle_counter
            current_sleep_cycles = map(soc, 0, 1023, 10, 1);
            sleep_cycle_counter = current_sleep_cycles;
        } else {
            // Short blink every 8 seconds according to battery life
            if (soc < 614) {
                blink_led(1);
            } else if (soc < 818) {
                blink_led(2);
            } else {
                blink_led(0);
            }
        }   
    }
    go_to_bed();
}




// ============================================================= SERIAL COMMUNICATION
void handle_serial() {
    blink_led(1);
    mySerial.begin(SERIAL_BAUD);
    
    char recv_buffer[SERIAL_BUFFER_SIZE] = {};
    char character = 0;

    // Send current color for synchronizing Omamoris
    mySerial.println("LUV;");                // Used for pairing

    // Wait for incomming serial bytes
    while (millis() < SERIAL_TIMEFRAME) {   // While serial communication is enabled
        // Read only first line received
        if (mySerial.available() > 0) {
            uint8_t pos = 0;
            serial_process_position = 0;
            
            set_leds_analog(255,0);
            
            while (mySerial.available() > 0 && pos < SERIAL_BUFFER_SIZE) {
                character = mySerial.read();
                recv_buffer[pos++] = character;
                delay(5);
            }

            set_leds_analog(0,255);
            delay(10);
            mySerial.print("ACK:");
            for (uint8_t i = 0; i < pos; ++i) {
                mySerial.print(recv_buffer[i]);
            }
            mySerial.println();
            
            // LUV command for pairing
            if (recv_buffer[0] == 'L' && recv_buffer[1] == 'U' && recv_buffer[2] == 'V') {
                delay(100);
                mySerial.println("Cute.");
            }
            
            // STA command for getting status
            else if (recv_buffer[0] == 'S' && recv_buffer[1] == 'T' && recv_buffer[2] == 'A') {
                read_soc();
                mySerial.println("VER:" VERSION);
                mySerial.print("SOC:");
                mySerial.println(soc);
                mySerial.print("SLP:");
                mySerial.print(isr_cnt_to_wake_up * 8);
                mySerial.println("s");
            }
            
            // SLP command for setting sleep time (value x 8s)
            else if (recv_buffer[0] == 'S' && recv_buffer[1] == 'L' && recv_buffer[2] == 'P') {
                serial_process_position = 4;
                uint8_t tmp_isr_cnt_to_wake_up = get_value(recv_buffer, serial_process_position, ';');
                if (tmp_isr_cnt_to_wake_up >= 1 && tmp_isr_cnt_to_wake_up <= 100) {
                    isr_cnt_to_wake_up = tmp_isr_cnt_to_wake_up;
                    update_eeprom(isr_cnt_to_wake_up_eeprom, isr_cnt_to_wake_up);
                    print_okay();
                }
                else {
                    print_error();
                }
            }

            // Unknown command
            else {
                print_error();
            }

            set_leds_digital(LOW, LOW);
            break;
        }
    }
    
    blink_led(2);
    mySerial.end();
}

// ============================================================= FINISHED FUNCTIONS

// WDT ISR
ISR(WDT_vect) {
    // Increase isr_cnt and wake up, when necessary
    isr_cnt += 1;
    if (isr_cnt >= isr_cnt_to_wake_up) {
        awake = true;
        isr_cnt = 0;
    }
}

// ENTER SLEEP
void go_to_bed() {
    wdt_reset();                        // Reset WDT
    sleep_cycle_counter -= 1;           // Decrease sleep cycle counter, since one time is right now
    awake = false;                      // uC is not awake anymore

    // Prepare for bed
    byte adcsra = ADCSRA;               // ADC Control and Status Register A sichern
    ADCSRA &= ~(1 << ADEN);             // ADC ausschalten
    MCUCR |= (1 << SM1) & ~(1 << SM0);  // Sleep-Modus = Power Down
    MCUCR |= (1 << SE);                 // Sleep Enable setzen

    // Sleep
    sleep_cpu();                        // Sleeping

    // Wake up
    MCUCR &= ~(1 << SE);                // Sleep Disable setzen
    ADCSRA = adcsra;                    // ADCSRA-Register laden
}

// GET SOC
void read_soc() {
    soc = analogRead(PIN_MEASUREMENT);    // Write analog value from ADC to soc variable (0-1023)
}

void set_leds_analog(int led1, int led2) {
    analogWrite(PIN_LED, led1);
    analogWrite(PIN_LED2, led2);
}

void set_leds_digital(int high_low1, int high_low2) {
    digitalWrite(PIN_LED, high_low1);
    digitalWrite(PIN_LED2, high_low2);
}

// BLINK LED
void blink_led(uint8_t which_one) {
    if (which_one == 1 || which_one == 0) {
      digitalWrite(PIN_LED, HIGH);
    }
    if (which_one == 1 || which_one == 0) {
      digitalWrite(PIN_LED2, HIGH);
    }
    delay(BLINK_ON_TIME);
    digitalWrite(PIN_LED, LOW);
    digitalWrite(PIN_LED2, LOW);
}

// SEND SERIAL okay
void print_okay() {
    mySerial.println("OK");
}

// SEND SERIAL no
void print_error() {
    mySerial.println("ERROR");
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
    // Only write to eeprom if value is not already set (so writing is not performed that often)
    if (eeprom_read_byte(&variable) != value) {
        eeprom_write_byte(&variable, value);
    }
}
