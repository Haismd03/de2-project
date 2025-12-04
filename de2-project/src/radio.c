/*
 * Copyright (c) 2025 Karel Matoušek
 * MIT License
 */

#include "model.h"
#include "SI4703.h"
#include <stdlib.h>
#include <string.h>

uint16_t brnoRadios[41] = { 876, 883, 889, 895, 899, 904, 910, 920, 926, 931,
                            936, 946, 951, 955, 964, 968, 976, 979, 981, 990,
                            994, 999, 1002, 1004, 1008, 1013, 1020, 1025, 1030, 1034,
                            1038, 1041, 1045, 1051, 1055, 1058, 1062, 1065, 1070, 1075, 1078}; 


void radio_read_regs(project_model_t *model) {
    uint16_t RSSI_raw;
    uint16_t RDSA, RDSB, RDSC, RDSD;
    if (SI4703_GetRxRegs(&RSSI_raw, &RDSA, &RDSB, &RDSC, &RDSD)) {

        model->RSSI = RSSI_raw & 0xFF; 

        if (SI4703_DecodeRDS_PSName(&model->RSSI, &RDSB, &RDSD)) {
            char* currentName = SI4703_GetPSName();

            static char prevName[9];

            // Deletes the name when station is changed
            if (strcmp(currentName, prevName) != 0) {
                strcpy(model->station_name, currentName);
            }

            strcpy(prevName, currentName);
        }
    }
}

float int_to_float(uint16_t integerValue)
{
    float floatValue = (float)integerValue / 10;

    return floatValue;
}

void radio_update(project_model_t *model)
{
    static uint16_t previousFrequency = 0;
    static uint16_t previousVolume = 0;
    static uint16_t previousIndex = 0;

    float frequencyFloat;

    // check if frequency changed
    if(model->frequency != previousFrequency)
    {
        previousFrequency = model->frequency;
        frequencyFloat = int_to_float(model->frequency);
        SI4703_SetFreq(frequencyFloat);     // change frequency
        strcpy(model->station_name, "        ");
    }

    // check if station changed
    if(model->radio_index != previousIndex)
    {
        previousIndex = model->radio_index;
        model->frequency = brnoRadios[model->radio_index];
        frequencyFloat = int_to_float(model->frequency);
        SI4703_SetFreq(frequencyFloat);     // change station
        strcpy(model->station_name, "        ");
    }

    //check if volume changed
    if(model->volume != previousVolume)
    {
        previousVolume = model->volume;
        SI4703_SetVolume(model->volume);    // change volume
    }
    else
    {
        return;     // no change
    }

}