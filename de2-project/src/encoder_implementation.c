#include "encoder.h"
#include "model.h"
#include <stdint.h>
#include "gpio.h"
#include "millis.h"

void encoder_change_frequency_settings(encoder_t *enc, uint8_t button_state, enc_settings_t *settings_1, enc_settings_t *settings_2) {
    
    static uint8_t button_last_state = 0;
    static uint32_t button_last_change_ms = 0;
    static uint8_t var_number = 0;

    if (button_state == 1 && button_last_state == 0 && (millis() - button_last_change_ms) > 10) {
        var_number ^= 1;

        enc->settings_p = (var_number == 0) ? settings_1 : settings_2;
        button_last_change_ms = millis();
    }
    button_last_state = button_state;
}