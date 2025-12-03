#ifndef __ENCODER_IMPLEMENTATION_H__
#define __ENCODER_IMPLEMENTATION_H__

#include "encoder.h"
#include <stdint.h>

void encoder_change_frequency_settings(encoder_t *enc, uint8_t button_state, enc_settings_t *settings_1, enc_settings_t *settings_2);

#endif // __ENCODER_IMPLEMENTATION_H__