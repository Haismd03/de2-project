#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// private global variable to hold milliseconds count
volatile uint32_t ms_counter = 0;

void millis_init(void) {

    TCCR1B |= (1 << WGM12);      // CTC (clear timer on compare match): TOP = OCR1A
    OCR1A = 249;                 // 1 ms = (16 MHz / 64 / 1000) - 1

    TIMSK1 |= (1 << OCIE1A);    // Enable Timer1 compare interrupt

    TCCR1B |= (1 << CS11) | (1 << CS10);    // Prescaler = 64
}

ISR(TIMER1_COMPA_vect) {
    ms_counter++;   // increments every 1 ms
}

/**
 * @brief Get the current milliseconds count
 * 
 * @return uint32_t Current milliseconds count
 */
uint32_t millis(void) {
    uint32_t m;
    TIMSK1 &= ~(1 << OCIE1A); // disable timer interrupt
    m = ms_counter;
    TIMSK1 |= (1 << OCIE1A); // re-enable timer interrupt
    return m;
}