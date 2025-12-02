#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

typedef struct encoder_s{
    uint8_t pin_a;
    uint8_t pin_b;
    volatile uint8_t *reg_p;

    volatile uint8_t change_flag;

    volatile uint8_t pin_a_state;
    volatile uint8_t pin_b_state;

    uint32_t last_update_ms;

    uint8_t pin_a_last_state;

    struct enc_settings_s *settings_p;
} encoder_t;

typedef struct enc_settings_s {
    uint16_t limit_min;
    uint16_t limit_max;
    uint16_t *position_p;
} enc_settings_t;

uint8_t encoder_init(encoder_t *enc, volatile uint8_t *reg, uint8_t pin_a, uint8_t pin_b, enc_settings_t *settings);

void update_encoder(encoder_t *enc);
void update_encoder_isr(encoder_t *enc);

#endif // ENCODER_H