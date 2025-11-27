#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

typedef struct encoder_s{
    uint8_t pin_a;
    uint8_t pin_b;

    volatile uint8_t *reg_p;

    uint8_t change_flag;

    volatile uint8_t pin_a_state;
    volatile uint8_t pin_b_state;

    uint8_t pin_a_last_state;

    uint16_t position;
    uint16_t limit;
} encoder_t;

uint8_t encoder_init(encoder_t *enc, volatile uint8_t *reg, uint8_t pin_a, uint8_t pin_b, uint16_t limit, uint16_t initial_position);

void update_encoder(encoder_t *enc);
void update_encoder_isr(encoder_t *enc);

#endif // ENCODER_H