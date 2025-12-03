// standart libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <timer.h>
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

#define BUTTON_PIN PD7

#define DEBUG_PRINT

// ================================================================================= Radio =================================================================================

uint16_t brnoRadios[41] = { 876, 883, 889, 895, 899, 904, 910, 920, 926, 931,
                            936, 946, 951, 955, 964, 968, 976, 979, 981, 990,
                            994, 999, 1002, 1004, 1008, 1013, 1020, 1025, 1030, 1034,
                            1038, 1041, 1045, 1051, 1055, 1058, 1062, 1065, 1070, 1075, 1078}; 


float foundRadios[41];

// ================================================================================ Encoder ===============================================================================

encoder_t volume_encoder;
encoder_t frequency_encoder;

// ================================================================================= main =================================================================================
project_model_t model;

int main(void) {	

    // intialize model
    model.volume = 0;
    model.radio_index = 0;
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

    /*float frequencyFloat = int_to_float(model.frequency);
    SI4703_SetVolume(model.volume);
    SI4703_SetFreq(frequencyFloat);*/

    // encoder
    enc_settings_t volume_settings = {0, 15, &model.volume};
    //enc_settings_t frequency_settings = {875, 1080, &model.frequency};
    //enc_settings_t radion_index_settings = {0, sizeof(brnoRadios)/sizeof(brnoRadios[0]) - 1, &model.radio_index};

    encoder_init(&volume_encoder, &PIND, PD4, PD3, &volume_settings);
    //encoder_init(&frequency_encoder, &PIND, PD6, PD5, &radion_index_settings);
    
    // Last millis
    uint32_t prevMillis_getRxRegs = millis();

    sei();

    #ifdef DEBUG_PRINT
        // TODO: remove
        char uart_msg[100];
        volatile uint16_t last_position = 0;
    #endif

    while(1) {

        // update encoders with data from ISR
        update_encoder(&volume_encoder);

        // TODO: update radio regs

        // update radio data every 1.5s
        if (millis() - prevMillis_getRxRegs >= 1500) {
            prevMillis_getRxRegs = millis();

            radio_read_regs(&model);
        }

        draw_static_screen(&model);



        #ifdef DEBUG_PRINT
        asm volatile("" ::: "memory");
        if (*(volume_encoder.settings_p->position_p) != last_position/* || encoder_2.position != last_position_2*/) {
            //sprintf(uart_msg, "Encoder position: \t1: %d \talias 1: %d \talias 2: %d \tsettings: %d \tmillis: %lu\n", *(encoder.settings_p->position_p), enc1_position, enc2_position, var_number, millis());
            sprintf(uart_msg, "Encoder position: \t1: %d \talias 1: %d \tmillis: %lu\n", *(volume_encoder.settings_p->position_p), model.volume, millis());
            //sprintf(uart_msg, "Encoder position: \t1: %d \t2: %d \talias: %d\n", encoder.position, encoder_2.position, *enc_allias_p);
            uart_puts(uart_msg);
            last_position = *(volume_encoder.settings_p->position_p);
            //last_position_2 = encoder_2.position;
        }
        #endif

    }
}

ISR(PCINT2_vect) {

    update_encoder_isr(&volume_encoder);
    //update_encoder_isr(&frequency_encoder);
    }