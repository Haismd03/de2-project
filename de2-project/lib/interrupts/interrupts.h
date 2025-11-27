#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

uint8_t pcint_enable(volatile uint8_t *reg, uint8_t pin);

#endif // INTERRUPTS_H  