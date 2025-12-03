#ifndef ENCODER_H
#define ENCODER_H

/**
 * @file encoder.h
 * @defgroup encoder Rotary Encoder Library <encoder.h>
 * @ingroup custom_libraries
 * @code #include <encoder.h> @endcode
 * @author David Haisman (haisman.david@seznam.cz)
 * @brief Single edge encoder library for AVR-GCC.
 * 
 * Encoder Generic library for rotary encoder handling using ISR for pinchange on channel A. Simple debouncing is included. Only edge on channel A is detected. 
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * @{
 */

#include <stdint.h>

typedef struct encoder_s{
    uint8_t pin_a;
    uint8_t pin_b;
    volatile uint8_t *reg_p; // pin register

    volatile uint8_t change_flag;

    volatile uint8_t pin_a_state;
    volatile uint8_t pin_b_state;

    uint8_t pin_a_last_state;

    uint32_t last_update_ms; // used for debouncing

    struct enc_settings_s *settings_p;
} encoder_t;

typedef struct enc_settings_s {
    uint16_t limit_min;
    uint16_t limit_max;
    uint16_t *position_p;
} enc_settings_t;

/**
 * @brief Intialize instance of rotary encoder
 * 
 * @param enc pointer to encoder structure
 * @param reg pointer to register for reading encoder pins
 * @param pin_a pin number for encoder channel A
 * @param pin_b pin number for encoder channel B
 * @param settings pointer to encoder settings structure
 * @return uint8_t status code (0 for success, else for error)
 */
uint8_t encoder_init(encoder_t *enc, volatile uint8_t *reg, uint8_t pin_a, uint8_t pin_b, enc_settings_t *settings);

/**
 * @brief Update encoder position in loop based on pin states from ISR
 * 
 * @param enc pointer to encoder structure
 */
void update_encoder(encoder_t *enc);

/**
 * @brief Update pin states in ISR
 * 
 * @param enc pointer to encoder structure
 */
void update_encoder_isr(encoder_t *enc);

/** @} */

#endif // ENCODER_H