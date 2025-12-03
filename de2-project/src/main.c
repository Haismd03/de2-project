// standart libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>
// custom libraries
#include "SI4703.h"
#include "uart.h"
#include "oled.h"
#include "millis.h"
#include "encoder.h"
#include "gpio.h"
// project includes
#include "model.h"
#include "radio.h"
#include "display.h"
#include "encoder_implementation.h"

//#define DEBUG_PRINT

// ================================================================================ Encoder ===============================================================================

encoder_t volume_encoder;
encoder_t frequency_encoder;

// ================================================================================= main =================================================================================
project_model_t model;

int main(void) {	

    // intialize model
    model.frequency_encoder_mode = 0; // start in radio index mode
    model.volume = 7;
    model.radio_index = 34;
    model.frequency = brnoRadios[model.radio_index];

    gpio_mode_input_pullup(&DDRD, BUTTON_PIN);

    // ititialize UART
	uart_init(UART_BAUD_SELECT(9600, F_CPU));

    // initialize radio
    SI4703_Init();
    SI4703_SetVolume(model.volume);
    //sweep();

    // initialize display
    oled_init(OLED_DISP_ON);

    // initialize millis
    millis_init();

    // encoder - volume
    enc_settings_t volume_settings = {0, 15, &model.volume};
    // encoder - frequency & radio index
    enc_settings_t radio_index_settings = {0, sizeof(brnoRadios)/sizeof(brnoRadios[0]) - 1, &model.radio_index};
    enc_settings_t frequency_settings = {875, 1080, &model.frequency};

    encoder_init(&volume_encoder, &PIND, PD4, PD3, &volume_settings);
    encoder_init(&frequency_encoder, &PIND, PD6, PD5, &radio_index_settings);
    
    // Last millis
    uint32_t prevMillis_getRxRegs = 0;

    sei();

    #ifdef DEBUG_PRINT
        // TODO: remove
        char uart_msg[100];
        volatile uint16_t last_position = 0;
    #endif

    while(1) {

        // change frequency encoder settings based on button state, debounce protected
        uint8_t button_state = !gpio_read(&PIND, BUTTON_PIN);
        encoder_change_frequency_settings(&model, &frequency_encoder, button_state, &radio_index_settings, &frequency_settings);

        // update encoders with data from ISR
        update_encoder(&volume_encoder);
        update_encoder(&frequency_encoder);

        // radio update
        radio_update(&model);

        // read radio data every 0.5s
        if (millis() - prevMillis_getRxRegs >= 500) {
            prevMillis_getRxRegs = millis();

            radio_read_regs(&model);
        }

        // update display
        display_update(&model);


        #ifdef DEBUG_PRINT
        asm volatile("" ::: "memory");
        if (*(frequency_encoder.settings_p->position_p) != last_position/* || encoder_2.position != last_position_2*/) {
            sprintf(uart_msg, "Encoder position: \t1: %d \talias 1: %d \talias 2: %d \tsettings: %d \tbutton_state: %d \tmillis: %lu\n", *(frequency_encoder.settings_p->position_p), model.radio_index, model.frequency, model.frequency_encoder_mode, button_state, millis());
            //sprintf(uart_msg, "Encoder position: \t1: %d \talias 1: %d \tmillis: %lu\n", *(volume_encoder.settings_p->position_p), model.volume, millis());
            //sprintf(uart_msg, "Encoder position: \t1: %d \t2: %d \talias: %d\n", encoder.position, encoder_2.position, *enc_allias_p);
            uart_puts(uart_msg);
            last_position = *(frequency_encoder.settings_p->position_p);
            //last_position_2 = encoder_2.position;
        }
        #endif

    }
}

ISR(PCINT2_vect) {

    update_encoder_isr(&volume_encoder);
    update_encoder_isr(&frequency_encoder);
}