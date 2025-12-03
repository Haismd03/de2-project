#ifndef __RADIO_H__
#define __RADIO_H__¨

/**
 * @file radio.h
 * @defgroup radio Radio project implementation <radio.h>
 * @ingroup project_files
 * @code #include <radio.h> @endcode
 * @author Karel Matoušek (k.matousek20@gmail.com)
 * @brief Radio project implementation for DE2-project using AVR
 * 
 * Radio module implementation for handling radio functionalities.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025 Karel Matoušek
 * @{
 */

#include <stdint.h>
#include "model.h"

/**
 * @brief Read registers from the radio and update the model
 * 
 * @param model Pointer to the project model to update
 */
void radio_read_regs(project_model_t *model);

/**
 * @brief Convert integer frequency value to float representation
 * 
 * @param integerValue Frequency value as integer
 * @return float Frequency value as float
 */
float int_to_float(uint16_t integerValue);

/**
 * @brief Update the radio settings based on the data from model
 * 
 * @param model Pointer to the project model containing desired settings
 */
void radio_update(project_model_t *model);

/**
 * @brief Array of predefined radio frequencies corresponding to radio stations in Brno
 * 
 */
extern uint16_t brnoRadios[41];

/** @} */

#endif // __RADIO_H__