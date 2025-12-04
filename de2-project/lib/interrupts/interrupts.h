/*
 * Copyright (c) 2025 David Haisman
 * MIT License
 */

/**
 * @file interrupts.h
 * @defgroup interrupts Interrupt Library <interrupts.h>
 * @ingroup custom_libraries
 * @code #include <interrupts.h> @endcode
 * @author David Haisman (haisman.david@seznam.cz)
 * @brief Pin change interrupt library for AVR-GCC.
 * 
 * This library provides functions to enable pin change interrupts on specific pins of AVR microcontrollers.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * @{
 */

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

/** @} */

#endif // INTERRUPTS_H  