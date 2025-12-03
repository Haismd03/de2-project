#ifndef __RADIO_H__
#define __RADIO_H__
#include <stdint.h>
#include "model.h"

void radio_read_regs(project_model_t *model);

float int_to_float(uint16_t integerValue);

void radio_update(project_model_t *model);

#endif // __RADIO_H__