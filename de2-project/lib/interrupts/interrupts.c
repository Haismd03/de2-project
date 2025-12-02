
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t pcint_enable(volatile uint8_t *reg, uint8_t pin) {
    if (reg == &PINB) {
        PCICR  |= (1 << PCIE0);             // povolit skupinu 0 (PB0–PB7)
        PCMSK0 |= (1 << pin); 
    } else if (reg == &PINC) {
        PCICR  |= (1 << PCIE1);             // povolit skupinu 1 (PC0–PC5)
        PCMSK1 |= (1 << pin);
    } else if (reg == &PIND) {
        PCICR  |= (1 << PCIE2);             // povolit skupinu 2 (PD0–PD7)
        PCMSK2 |= (1 << pin);
    } else {
        return 1; // error: invalid register
    }
    return 0; // success
}
