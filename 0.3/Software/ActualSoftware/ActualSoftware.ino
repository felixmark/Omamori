#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define SLEEP_SECONDS 5

void setup() {
  // Set all pins to low power mode:
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTA + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTB + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}

  // Wait for all register to be synchronized
  while (RTC.STATUS > 0) {}

  // RTC configuration
	RTC.PER = 1024 * SLEEP_SECONDS;                                               // 10 secs between wakes.
	RTC.INTCTRL = 0 << RTC_CMP_bp	| 1 << RTC_OVF_bp;                              // Overflow interrupt
	RTC.CTRLA = RTC_PRESCALER_DIV1_gc	| 1 << RTC_RTCEN_bp | 1 << RTC_RUNSTDBY_bp; // No Prescaler + Enable RTC + Run in standby
	RTC.CLKSEL = RTC_CLKSEL_INT1K_gc;                                             // 32KHz divided by 32, i.e run at 1.024kHz
	
	// Enable interrupts
	sei();
}

void loop() {
  set_sleep_mode(SLEEP_MODE_STANDBY);
  sleep_enable();

  //Go to sleep
  sleep_cpu();
  
  //On awakening
  delay(2000);
}


// RTC ISR
ISR(RTC_CNT_vect) {
	RTC.INTFLAGS = RTC_OVF_bm;
}