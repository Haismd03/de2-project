#ifndef __ENCODER_IMPLEMENTATION_H__
#define __ENCODER_IMPLEMENTATION_H__

/**
 * @file encoder_implementation.h
 * @defgroup encoder_implementation Encoder Implementation project implementation <encoder_implementation.h>
 * @ingroup project_files
 * @code #include <encoder_implementation.h> @endcode
 * @author Tadeáš Fojtách (tad123@email.cz)
 * @brief Encoder Implementation project implementation for DE2-project using AVR
 * 
 * Encoder module implementation for handling rotary encoder.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025 Tadeáš Fojtách
 * @{
 */

#include "encoder.h"
#include <stdint.h>

/**
 * @brief Toggles between encoder settings for frequency based on button state
 * 
 * @param model Pointer to the project model
 * @param enc Pointer to the encoder instance
 * @param button_state Current state of the button (1 = pressed, 0 = not pressed)
 * @param settings_1 Pointer to the first encoder settings
 * @param settings_2 Pointer to the second encoder settings
 */
void encoder_change_frequency_settings(project_model_t *model, encoder_t *enc, uint8_t button_state, enc_settings_t *settings_1, enc_settings_t *settings_2);

/** @} */

#endif // __ENCODER_IMPLEMENTATION_H__