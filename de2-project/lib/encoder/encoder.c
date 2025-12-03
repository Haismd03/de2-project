#include "encoder.h"
#include <stdint.h>
#include <stddef.h>
#include "interrupts.h"
#include "gpio.h"
#include "millis.h"

uint8_t encoder_init(encoder_t *enc, volatile uint8_t *reg, uint8_t pin_a, uint8_t pin_b, enc_settings_t *settings) {
    if (enc == NULL) {
        return 1; // error: null pointer
    }
    
    enc->pin_a = pin_a;
    enc->pin_b = pin_b;
    enc->settings_p = settings;
    enc->reg_p = reg;
    
    //enc->last_update_ms = 0;
    
    uint8_t a = gpio_read(reg, pin_a);
    uint8_t b = gpio_read(reg, pin_b);
    enc->ab_state = (a << 1) | b; // initial quadrature state

    enc->inc = 0;

    pcint_enable(reg, pin_a);
    pcint_enable(reg, pin_b);

    return 0; // success
}

void update_encoder(encoder_t *enc) {

    if (enc->inc == 0) {
        return; // no change detected
    }
    enc->inc = 0; // reset increment flag

    /*if (millis() - enc->last_update_ms < 5) {
        return; // debounce: ignore changes within 5 ms
    }
    enc->last_update_ms = millis();*/

    /*int8_t inc = 0;

    if ((enc->pin_a_state == 0) && (enc->pin_a_last_state == 1)) { // falling edge on pin A
        if (enc->pin_b_state == 0) { // fall_A & down_B
            inc = 1; // clockwise
        } else { // fall_A & up_B
            inc = -1; // counter-clockwise
        }
    } else if ((enc->pin_a_state == 1) && (enc->pin_a_last_state == 0)) { // rising edge on pin A
        if (enc->pin_b_state == 1) { // rise_A & up_B
            inc = 1; // clockwise
        } else { // rise_A & down_B
            inc = -1; // counter-clockwise
        }
    } else {
        return; // no change
    }*/

    //enc->pin_a_last_state = enc->pin_a_state;

    if (*(enc->settings_p->position_p) < enc->settings_p->limit_min) { // protect underflow
        *(enc->settings_p->position_p) = enc->settings_p->limit_min;
    } else if (*(enc->settings_p->position_p) > enc->settings_p->limit_max) { // protect overflow
        *(enc->settings_p->position_p) = enc->settings_p->limit_max;
    }

    if ((enc->inc == 1) && (*(enc->settings_p->position_p) < enc->settings_p->limit_max)) { // increment position
        (*(enc->settings_p->position_p))++;
    } else if ((enc->inc == -1) && (*(enc->settings_p->position_p) > enc->settings_p->limit_min)) { // decrement position
        (*(enc->settings_p->position_p))--;
    }
}

inline void update_encoder_isr(encoder_t *enc) {
    uint8_t a = gpio_read(enc->reg_p, enc->pin_a);
    uint8_t b = gpio_read(enc->reg_p, enc->pin_b);
    
    uint8_t new_state = (a << 1) | b;
    uint8_t old_state = enc->ab_state;

    static const int8_t table[16] = {
        0,  +1,  -1,  0,
        -1,  0,   0, +1,
        +1,  0,   0, -1,
         0, -1,  +1,  0
    };

    int8_t inc = table[(old_state << 2) | new_state];

    if (inc != 0)
        enc->inc += inc;   // ← jen přičte změnu, nepřepisuje!

    enc->ab_state = new_state;
}