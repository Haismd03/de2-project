#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gpio.h"
#include "interrupts.h"
#include "encoder.h"
#include "stdio.h"
#include "uart.h"
#include "millis.h"

#define LED_BUILTIN PB5  // On-board LED
#define BUTTON_PIN PB0  // On-board Button

encoder_t encoder;
//encoder_t encoder_2;

int main(void) {
    gpio_mode_output(&DDRB, LED_BUILTIN);
    gpio_mode_input_pullup(&DDRB, BUTTON_PIN);

    uart_init(UART_BAUD_SELECT(9600, F_CPU));

    millis_init();

    uint16_t enc1_position = 0;
    uint16_t enc2_position = 0;

    enc_settings_t settings_1 = {0, 15, &enc1_position};
    enc_settings_t settings_2 = {875, 1080, &enc2_position};

    encoder_init(&encoder, &PIND, PD4, PD3, &settings_1);
    //encoder_init(&encoder_2, &PIND, PD7, PD6, 20, 10);

    sei();

    char uart_msg[100];
    volatile uint16_t last_position = 0;

    uint8_t var_number = 0;
    uint8_t button_last_state = 0;
    uint32_t button_last_change_ms = 0;

    while(1) {

        uint8_t button_state = gpio_read(&PINB, BUTTON_PIN);
        if (button_state == 1 && button_last_state == 0 && (millis() - button_last_change_ms) > 10) {
            var_number ^= 1;

            encoder.settings_p = (var_number == 0) ? &settings_1 : &settings_2;
            button_last_change_ms = millis();
        }
        button_last_state = button_state;

        update_encoder(&encoder);
        //update_encoder(&encoder_2);
        
        asm volatile("" ::: "memory");
        if (*(encoder.settings_p->position_p) != last_position/* || encoder_2.position != last_position_2*/) {
            sprintf(uart_msg, "Encoder position: \t1: %d \talias 1: %d \talias 2: %d \tsettings: %d \tmillis: %lu\n", *(encoder.settings_p->position_p), enc1_position, enc2_position, var_number, millis());
            //sprintf(uart_msg, "Encoder position: \t1: %d \t2: %d \talias: %d\n", encoder.position, encoder_2.position, *enc_allias_p);
            uart_puts(uart_msg);
            last_position = *(encoder.settings_p->position_p);
            //last_position_2 = encoder_2.position;
        }
    }
}

ISR(PCINT2_vect) {

    update_encoder_isr(&encoder);
    //update_encoder_isr(&encoder_2);

    gpio_toggle(&PORTB, LED_BUILTIN);
}

