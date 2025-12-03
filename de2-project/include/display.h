#ifndef __DISPLAY_H__
#define __DISPLAY_H__

/**
 * @file display.h
 * @defgroup display Display project implementation <display.h>
 * @ingroup project_files
 * @code #include <display.h> @endcode
 * @author Tadeáš Fojtách (tad123@email.cz)
 * @brief Display project implementation for DE2-project using AVR
 * 
 * Display module implementation for showing radio information on OLED display.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025 Tadeáš Fojtách
 * @{
 */

#include "model.h"

/**
 * @brief Update the display with current model information
 * 
 * @param model Pointer to the project model containing data
 */
void display_update(project_model_t *model);

/** @} */

#endif // __DISPLAY_H__