#include <stdint.h>
#include "oled.h"
#include "model.h"
#include <string.h>
#include <stdio.h>

#define MAX_CHARS_PER_LINE    21  // Characters on one line
//#define SCROLL_PADDING_SPACES 3   // Spaces after text

// Draw only the station name line (line 3)
/*void draw_station_line(const char *text)
{
    oled_gotoxy(0, 3);

    // Clear the line
    for (uint8_t i = 0; i < MAX_CHARS_PER_LINE; i++) {
        oled_putc(' ');
    }

    oled_gotoxy(0, 3);
    oled_puts(text);
}*/

// Draw static text: title, frequency, volume, time
void draw_static_screen(project_model_t *model)
{

    static char previousName[9] = "";
    static uint16_t previousFrequency = 0;
    static uint16_t previousVolume = 0;

    // check if anything changed
    if (strcmp(model->station_name, previousName) == 0 &&
        model->frequency == previousFrequency &&
        model->volume == previousVolume) {
        // No changes, skip redraw
        return;
    }

    oled_init(OLED_DISP_ON);
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
    oled_gotoxy(0, 3);
    oled_puts(model->station_name);

    // Frequency
    char frequencyStr[32];
    snprintf(
        frequencyStr,
        sizeof(frequencyStr),
        "Frequency: %u.%u MHz",
        model->frequency / 10,
        model->frequency % 10
    );
    oled_gotoxy(0, 4);
    oled_puts(frequencyStr);

    // Volume
    char volumeStr[16];
    snprintf(
        volumeStr,
        sizeof(volumeStr),
        "Volume: %u",
        model->volume
    );
    oled_gotoxy(0, 5);
    oled_puts(volumeStr);

    oled_display();
}