#ifndef __MILLIS_H__
#define __MILLIS_H__

#include <stdint.h>

/**
 * @file millis.h
 * @defgroup millis Millis Library <millis.h>
 * @ingroup custom_libraries
 * @code #include <millis.h> @endcode
 * @author David Haisman (haisman.david@seznam.cz)
 * @brief Millisecond counter library for AVR-GCC.
 * 
 * Millis library for providing millisecond timing functionality using AVR Timer1.
 * 
 * @version 0.1
 * @date 2025-12-03
 * 
 * @copyright Copyright (c) 2025
 * @{
 */


/**
 * @brief Initialize the millis timer
 * 
 */
void millis_init(void);

/**
 * @brief Get the current milliseconds count
 * 
 * @return uint32_t Current milliseconds count
 */
uint32_t millis(void);

/** @} */

#endif // __MILLIS_H__