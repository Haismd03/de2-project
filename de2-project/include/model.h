#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdint.h>

#define BUTTON_PIN PD7

typedef struct project_model_s {
    uint16_t volume;
    uint16_t frequency;
    uint16_t radio_index;
    char station_name[9];
    uint16_t RSSI;
} project_model_t;

extern project_model_t model;

#endif // __MODEL_H__