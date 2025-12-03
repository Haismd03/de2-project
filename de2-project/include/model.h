#ifndef __MODEL_H__
#define __MODEL_H__

/**
 * @file model.h
 * @defgroup model Model project implementation <model.h>
 * @ingroup project_files
 * @code #include <model.h> @endcode
 * @author David Haisman (haisman.david@seznam)
 * @brief Model project implementation for DE2-project using AVR
 * 
 * Project model structure containing radio, encoder mode and display information.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025 David Haisman
 * @{
 */

#include <stdint.h>

#define BUTTON_PIN PD7

/**
 * @brief Project model structure containing radio, encoder mode and display information
 * 
 */
typedef struct project_model_s {
    uint16_t volume;
    uint16_t frequency;
    uint16_t radio_index;
    char station_name[9];
    uint16_t RSSI;

    uint8_t frequency_encoder_mode; // 0 - radio index, 1 - frequency
} project_model_t;

extern project_model_t model;

#endif // __MODEL_H__