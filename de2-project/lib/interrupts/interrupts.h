#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

/**
 * @brief Enable pin change interrupt for a specific pin on a given register
 * 
 * @param reg Pointer to the PIN register (e.g., &PINB, &PINC, &PIND)
 * @param pin Pin number within the register to enable interrupt for
 * @return uint8_t Status code (0 for success, 1 for error)
 */
uint8_t pcint_enable(volatile uint8_t *reg, uint8_t pin);

#endif // INTERRUPTS_H  