#include <stdint.h>
#include "oled.h"
#include "model.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void display_update(project_model_t *model)
{

    static char previousName[9] = "";
    static uint16_t previousFrequency = 0;
    static uint16_t previousVolume = 0;
    static uint16_t previousRSSI = 0;
    static uint16_t previousMode = 0;

    // check if anything changed
    if (strcmp(model->station_name, previousName) == 0 &&
    model->frequency == previousFrequency &&
    model->volume == previousVolume &&
    abs(model->RSSI - previousRSSI) < 3 &&
    model->frequency_encoder_mode == previousMode) {
        
        return; // No changes, skip redraw
    }

    // update previous vars to current
    strcpy(previousName, model->station_name);
    previousFrequency = model->frequency;
    previousVolume = model->volume;
    previousRSSI = model->RSSI; 
    previousMode = model->frequency_encoder_mode;

    oled_clrscr();

    // Title
    oled_home();
    oled_gotoxy(0, 0);
    oled_charMode(DOUBLESIZE);
    oled_puts("Radio:");
    oled_charMode(NORMALSIZE);

    // Labels
    oled_gotoxy(0, 2);
    oled_puts("Name:");
    oled_gotoxy(6, 2);
    oled_puts(model->station_name);

    // Frequency
    char frequencyStr[32];
    snprintf(frequencyStr, sizeof(frequencyStr), "Frequency: %u.%u MHz", model->frequency / 10, model->frequency % 10);
    oled_gotoxy(0, 3);
    oled_puts(frequencyStr);

    // Volume
    char volumeStr[16];
    snprintf(volumeStr, sizeof(volumeStr), "Volume: %u", model->volume);
    oled_gotoxy(0, 4);
    oled_puts(volumeStr);

    // RSSI
    char rssiStr[10];
    snprintf(rssiStr, sizeof(rssiStr), "RSSI: %u", model->RSSI);
    oled_gotoxy(0, 5);
    oled_puts(rssiStr);

    // Frequency encoder current mode
    char msg[10];
    char encMode[10];
    snprintf(msg, sizeof(msg), "Changing: ");
    oled_gotoxy(0, 6);
    if(model->frequency_encoder_mode == 0)
    {
        snprintf(encMode, sizeof(encMode), "station");
    }
    oled_puts(msg);

    oled_gotoxy(10, 6);
    if(model->frequency_encoder_mode == 1)
    {
        snprintf(encMode, sizeof(encMode), "frequency");
    }
    oled_puts(encMode);


    oled_display();
}