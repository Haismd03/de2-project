#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t ms_counter = 0;

void millis_init(void) {
    // CTC režim
    TCCR1B |= (1 << WGM12);      // CTC: TOP = OCR1A

    // Compare match every 1 ms
    OCR1A = 249;                 // (16 MHz / 64 / 1000) - 1

    // Enable interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Start timer: prescaler 64
    TCCR1B |= (1 << CS11) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect) {
    ms_counter++;                // inkrement každých 1 ms
}

uint32_t millis(void) {
    uint32_t m;
    cli();
    m = ms_counter;
    sei();
    return m;
}