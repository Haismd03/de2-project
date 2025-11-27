#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gpio.h"
#include "interrupts.h"
#include "encoder.h"
#include "stdio.h"
#include "uart.h"

#define LED_BUILTIN PB5  // On-board LED

encoder_t encoder;
encoder_t encoder_2;

int main(void) {
    gpio_mode_output(&DDRB, LED_BUILTIN);

    gpio_write_high(&PORTB, LED_BUILTIN); // Rozsviť LED na začátku
    _delay_ms(500);
    gpio_write_low(&PORTB, LED_BUILTIN); // Zhasni LED na začátku

    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    encoder_init(&encoder, &PIND, PD4, PD3, 15, 0);
    encoder_init(&encoder_2, &PIND, PD7, PD6, 20, 10);

    sei();

    char uart_msg[100];
    volatile uint16_t last_position = 0;
    volatile uint16_t last_position_2 = 0;

    uint16_t *enc_allias_p = &encoder.position;

    while(1) {
        update_encoder(&encoder);
        update_encoder(&encoder_2);
        
        asm volatile("" ::: "memory");
        if (encoder.position != last_position || encoder_2.position != last_position_2) {
            sprintf(uart_msg, "Encoder position: \t1: %d \t2: %d \talias: %d\n", encoder.position, encoder_2.position, *enc_allias_p);
            uart_puts(uart_msg);
            last_position = encoder.position;
            last_position_2 = encoder_2.position;
        }
    }
}

ISR(PCINT2_vect) {

    update_encoder_isr(&encoder);
    update_encoder_isr(&encoder_2);

    gpio_toggle(&PORTB, LED_BUILTIN);
}

