/*
    Omamori Program v1.1
    
    Author: Felix Mark
    Date:   23.05.2021

    Instructions:
    Set internal clock to 1MHz -> Burn bootloader and flash software

    ATTiny 85 V Pinout:
             ________
 <A0>  RST -|O       |- VCC (1.8V)
  <3>   RX -|        |- TX   <2>
  <4> LED3 -|        |- LED2 <1>
       GND -|________|- LED1 <0>
    
    List of serial commands:
    VER                     // Get version of the Omamori
    INF                     // Get main info stored on the Omamori
    MSG:<message>           // Set message of the Omamori (max. 80 Bytes)
    NAM:<name>              // Set owner name of the Omamori (max. 14 Bytes)
    GET                     // Get owner name of the Omamori
    ADD:<name>              // Add name to the list of friends on the Omamori

    EEPROM:
    256 Bytes
    80 Bytes for message
    14 Bytes for own name
    1 Byte for number of stored names
    256 - everything = memory for linked omamoris = 256 - 80 - 14 - 1 = 161 Bytes / 14 Byte per name = 11.5 => 11 Friends
*/



// ============================================================= INCLUDES
#include <SoftwareSerial.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/eeprom.h>



// ============================================================= DEFINES
// General
#define VERSION             "1.1"
#define SERIAL_BAUD         19200
#define NUM_SIGMOID_VALUES  105
#define EEPROM_BUFFER_SIZE  81
#define BLINK_ON_DURATION   20                  // ms
#define ANIMATION_DELAY     5                   // ms
#define ON_TIME_DELAY       50                  // ms
#define SERIAL_TIMEFRAME    5000                // ms
#define SERIAL_BUFFER_SIZE  90                  // 90 characters receive buffer
#define NUM_SHORT_BLINKS_BEFORE_LONG_BLINK  10  // times 8 seconds

// Pins
#define PIN_LED1            PCINT0
#define PIN_LED2            PCINT1
#define PIN_RX              PCINT2
#define PIN_TX              PCINT3
#define PIN_LED3            PCINT4
#define PIN_MEASUREMENT     PCINT5



// ============================================================= CONSTANTS
static uint8_t SIGMOID [NUM_SIGMOID_VALUES] = {
    0,   1,   2,   3,   4,   5,
    6,   7,   8,   9,  10,  11,  12,  13,  15,  17,  19,  21,  23,  25,  27,  30,  33,  36,  39,  42,  46,  50,  54,  58,
    63,  68,  73,  79,  84,  90,  96,  102, 108, 115, 121, 128, 134, 140, 146, 153, 159, 165, 170, 176, 181, 186, 191, 196, 200,
    205, 208, 212, 216, 219, 222, 225, 227, 230, 232, 234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 248, 249, 249, 
    250, 250, 251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 254, 254, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255
};



// ============================================================= VARIABLES
SoftwareSerial mySerial(PIN_RX, PIN_TX);

volatile bool awake = false;          // Used for waking up in ISR (Interrupt Service Routine)
int8_t sleep_cycle_counter = 1;       // Counting how often uC woke up and blinked shortly
uint8_t isr_cnt = 0;                  // Short blink every 8 * isr_cnt seconds
int soc = 0;                          // Store SOC measurement (State of charge)
uint8_t serial_process_position = 0;  // How much of the received serial string has been processed



// ============================================================= EEPROM
char eeprom_buffer[EEPROM_BUFFER_SIZE] = {};
// Own message and name
uint8_t EEMEM message_eeprom[80];
uint8_t EEMEM name_eeprom[14];

// Friends
uint8_t friend_count = 0;
uint8_t EEMEM friend_count_eeprom;
uint8_t EEMEM friend_list_eeprom[11][14];



// ============================================================= SETUP 
void setup() {
  
    // Setup Pins
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    pinMode(PIN_MEASUREMENT, INPUT);

    // Setup WDT
    cli();                          // Disable Interrupts
    wdt_reset();                    // Reset Watchdog Timer
    MCUSR &= ~(1 << WDRF);          // Disable Watchdog System Reset
    WDTCR = (1 << WDCE);            // Watchdog Change Enable
    WDTCR |= 1 << WDP0 | 1 << WDP3; // Set WDT timeout to 8 seconds 4 would be: (1<<WDP3) | (0<<WDP2) | (0<<WDP1) | (0<<WDP0);
    WDTCR |= (1 << WDIE);           // Watchdog Timeout Interrupt Enable
    sei();                          // Enable Interrupts
    
    // Handle Serial communication
    blink_led(1);
    handle_serial();
    blink_led(3);
}



void loop() {
    if (awake) {
        if (sleep_cycle_counter <= 0) {
          
            // After some (defined) sleep cycles show breathing pattern

            int delayed = 0, delayed2 = 0;
            int a = 0, b = 0, c = 0;
            for (int i = 0; i < NUM_SIGMOID_VALUES * 2 + 140; i += 1) {
                a = i;
                b = delayed;
                c = delayed2;
                if (a >= NUM_SIGMOID_VALUES * 2 - 1) {
                    a = 0;
                }
                else if (a >= NUM_SIGMOID_VALUES) {
                    a = NUM_SIGMOID_VALUES - 1 - (a % NUM_SIGMOID_VALUES);
                }
                if (a < 0) {
                    a = 0;
                }
                if (b >= NUM_SIGMOID_VALUES * 2 - 1) {
                    b = 0;
                }
                else if (b >= NUM_SIGMOID_VALUES) {
                    b = NUM_SIGMOID_VALUES - 1 - (b % NUM_SIGMOID_VALUES);
                }
                if (b < 0) {
                    b = 0;
                }
                if (c >= NUM_SIGMOID_VALUES * 2 - 1) {
                    c = 0;
                }
                else if (c >= NUM_SIGMOID_VALUES) {
                    c = NUM_SIGMOID_VALUES - 1 - (c % NUM_SIGMOID_VALUES);
                }
                if (c < 0) {
                    c = 0;
                }
                set_leds_analog(SIGMOID[a], SIGMOID[b], SIGMOID[c]);
                delay(ANIMATION_DELAY);
                
                if (i >= 70) {
                    delayed++;
                }
                if (i >= 140) {
                    delayed2++;
                }
            }

            // Set sleep_cycle_counter
            sleep_cycle_counter = NUM_SHORT_BLINKS_BEFORE_LONG_BLINK;
        } else {
            // Short blink an LED every 8 seconds
            read_soc();
            if (soc <= 341) {
                blink_led(3);
            } else if (soc <= 682) {
                blink_led(2);
            } else {
                blink_led(1);
            }
        }
    }
    go_to_bed();
}




// ============================================================= SERIAL COMMUNICATION
void handle_serial() {
    
    char recv_buffer[SERIAL_BUFFER_SIZE] = {};
    char character = 0;

    // Send GET for synchronizing Omamoris
    mySerial.begin(SERIAL_BAUD);
    mySerial.println("GET");                // Used for pairing

    // Wait for incomming serial bytes
    while (millis() < SERIAL_TIMEFRAME) {   // While serial communication is enabled
        // Read only first line received
        if (mySerial.available() > 0) {
            uint8_t pos = 0;
            serial_process_position = 0;

            // Receive bytes and write them into receive buffer
            while (mySerial.available() > 0 && pos < SERIAL_BUFFER_SIZE) {
                character = mySerial.read();
                recv_buffer[pos++] = character;
                delay(5);
            }

            // Short delay after receiving a command
            delay(10);

            // Send ACK with received command
            mySerial.print("ACK:");
            for (uint8_t i = 0; i < pos; ++i) {
                mySerial.print(recv_buffer[i]);
            }
            mySerial.println();


            // Check received command

            // INF command for getting information
            if (recv_buffer[0] == 'I' && recv_buffer[1] == 'N' && recv_buffer[2] == 'F') {
                mySerial.print("NAM:");
                eeprom_read_block(eeprom_buffer, name_eeprom, sizeof(name_eeprom));
                mySerial.println(eeprom_buffer);
                clear_eeprom_buffer();
                mySerial.print("MSG:");
                eeprom_read_block(eeprom_buffer, message_eeprom, sizeof(message_eeprom));
                mySerial.println(eeprom_buffer);
                clear_eeprom_buffer();
                mySerial.print("FRN:");
                uint8_t num_friends = eeprom_read_byte(friend_count_eeprom);
                for (int i = 0; i < num_friends; ++i) {
                    eeprom_read_block(eeprom_buffer, friend_list_eeprom[i], sizeof(message_eeprom));
                    mySerial.print("- ");
                    mySerial.println(eeprom_buffer);
                }
                clear_eeprom_buffer();
            }

            // GET version of Omamori
            else if (recv_buffer[0] == 'V' && recv_buffer[1] == 'E' && recv_buffer[2] == 'R') {
                mySerial.println("VER:" VERSION);
            }

            // GET SOC of Omamori
            else if (recv_buffer[0] == 'S' && recv_buffer[1] == 'O' && recv_buffer[2] == 'C') {
                mySerial.print("SOC:");
                mySerial.println(soc);
            }
            
            // SET name of owner of Omamori
            else if (recv_buffer[0] == 'N' && recv_buffer[1] == 'A' && recv_buffer[2] == 'M') {
                remove_command_from_array(recv_buffer);
                eeprom_write_block(recv_buffer, name_eeprom, sizeof(message_eeprom));
            }

            // SET message of Omamori
            else if (recv_buffer[0] == 'M' && recv_buffer[1] == 'S' && recv_buffer[2] == 'G') {
                remove_command_from_array(recv_buffer);
                eeprom_write_block(recv_buffer, message_eeprom, sizeof(message_eeprom));
            }
            
            // Add a friend to the list
            else if (recv_buffer[0] == 'A' && recv_buffer[1] == 'D' && recv_buffer[2] == 'D') {
                uint8_t num_friends = eeprom_read_byte(friend_count_eeprom);
                remove_command_from_array(recv_buffer);
                eeprom_write_block(recv_buffer, friend_list_eeprom[num_friends], sizeof(friend_list_eeprom[num_friends]));
                num_friends += 1;
                update_eeprom(friend_count_eeprom, num_friends);
            }
            
            // GET command for pairing
            else if (recv_buffer[0] == 'G' && recv_buffer[1] == 'E' && recv_buffer[2] == 'T') {
                eeprom_read_block(eeprom_buffer, name_eeprom, sizeof(name_eeprom));
                mySerial.println(eeprom_buffer);
                clear_eeprom_buffer();
            }

            // Unknown command was received
            else {
                print_error();
            }

            // End receiving
            break;
        }
    }
    
    mySerial.end();
}

// ============================================================= FINISHED FUNCTIONS

// WDT ISR
ISR(WDT_vect) {
    // Wake up
    awake = true;
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

// SET ANALOG STATE OF ALL 3 LEDS
void set_leds_analog(int led1, int led2, int led3) {
    analogWrite(PIN_LED1, led1);
    analogWrite(PIN_LED2, led2);
    analogWrite(PIN_LED3, led3);
}

// SET DIGITAL STATE OF ALL 3 LEDS
void set_leds_digital(int high_low1, int high_low2, int high_low3) {
    digitalWrite(PIN_LED1, high_low1);
    digitalWrite(PIN_LED2, high_low2);
    digitalWrite(PIN_LED3, high_low3);
}

// BLINK LED
void blink_led(uint8_t which_one) {
    set_leds_digital(
        which_one == 1 || which_one == 0, 
        which_one == 2 || which_one == 0, 
        which_one == 3 || which_one == 0
    );
    delay(BLINK_ON_DURATION);
    set_leds_digital(LOW, LOW, LOW);
}

// SEND SERIAL okay
void print_okay() {
    mySerial.println("OK");
}

// SEND SERIAL no
void print_error() {
    mySerial.println("ERROR");
}

// CLEAR EEPROM BUFFER
void clear_eeprom_buffer() {
  for (int i = 0; i < EEPROM_BUFFER_SIZE; ++i) {
      eeprom_buffer[i] = 0;
  }
}

// EEPROM FUNCTIONS
void update_eeprom(uint8_t &variable, uint8_t value) {
    // Only write to eeprom if value is not already set (so writing is not performed that often)
    if (eeprom_read_byte(&variable) != value) {
        eeprom_write_byte(&variable, value);
    }
}

void remove_command_from_array(char recv_buffer[]) {
    for (uint8_t i = 0; i < SERIAL_BUFFER_SIZE; ++i) {
        if (i < SERIAL_BUFFER_SIZE - 5) {
            recv_buffer[i] = recv_buffer[i+4];
        } else {
            recv_buffer[i] = 0;
        }
    }
}

// GET SOC
void read_soc() {
    soc = analogRead(PIN_MEASUREMENT);    // Write analog value from ADC to soc variable (0-1023)
}
