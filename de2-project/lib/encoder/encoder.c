#include "encoder.h"
#include <stdint.h>
#include <stddef.h>
#include "interrupts.h"
#include "gpio.h"

uint8_t encoder_init(encoder_t *enc, volatile uint8_t *reg, uint8_t pin_a, uint8_t pin_b, uint16_t limit, uint16_t initial_position) {
    if (enc == NULL) {
        return 1; // error: null pointer
    }
    pcint_enable(reg, pin_a);
    
    enc->pin_a = pin_a;
    enc->pin_b = pin_b;
    enc->position = initial_position;
    enc->reg_p = reg;
    enc->limit = limit;

    enc->pin_a_last_state = gpio_read(reg, pin_a);

    return 0; // success
}

void update_encoder(encoder_t *enc) {

    if (enc->change_flag == 0) {
        return; // no change detected
    }
    enc->change_flag = 0; // reset change flag

    int8_t inc = 0;

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
    }

    enc->pin_a_last_state = enc->pin_a_state;

    if ((inc == 1) && (enc->position < enc->limit)) {
        enc->position++;
    } else if ((inc == -1) && (enc->position > 0)) {
        enc->position--;
    }
}

inline void update_encoder_isr(encoder_t *enc) {
    enc->pin_a_state = gpio_read(enc->reg_p, enc->pin_a);
    enc->pin_b_state = gpio_read(enc->reg_p, enc->pin_b);
    enc->change_flag = 1;
}