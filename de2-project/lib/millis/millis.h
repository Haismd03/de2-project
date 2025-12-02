#ifndef __MILLIS_H__
#define __MILLIS_H__

#include <stdint.h>

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

#endif // __MILLIS_H__