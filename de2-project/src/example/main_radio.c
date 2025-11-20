
// -- Includes ---------------------------------------------
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "twi.h"            // I2C/TWI library for AVR-GCC
#include <stdio.h>          // C library. Needed for `sprintf`
#include "timer.h"
#include <stdint.h>
#include "SI470X.h"
#include "gpio.h"

// -- Defines-----------------------------------------------
#define RST_PIN PD2

#ifndef F_CPU
#define F_CPU 16000000   // For delay
#endif

SI470X_t radio;

int main(void)
{
    gpio_mode_output(&DDRD, RST_PIN);

    twi_init();

    si470x_init(&radio);
    si470x_setup(&radio, &DDRD, RST_PIN, OSCILLATOR_TYPE_CRYSTAL);
    si470x_setFrequency(&radio, 9100);
    si470x_setVolume(&radio, 10);

    while(1) {
    }

    return 0;
}