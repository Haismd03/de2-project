#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "oled.h"
#include <timer.h>

const uint8_t MAX_CHARS_PER_LINE    = 21;  // Characters on one line
const uint8_t SCROLL_PADDING_SPACES = 3;   // Spaces after text

typedef struct {
    uint8_t hours;
    uint8_t minutes;
    char stationName[32];
} RadioInfo;

typedef struct {
    uint8_t frequencyInt;
    uint8_t frequencyFrac;
    uint8_t volume;
} TunerInfo;

RadioInfo g_currentRadio = {
    .hours       = 10,
    .minutes     = 15,
    .stationName = "Raadio Krokodyy"
};

TunerInfo g_currentTuner = {
    .frequencyInt  = 103,
    .frequencyFrac = 5,
    .volume        = 15
};

void wait_tim1_ovf(uint8_t seconds)
{
    while (seconds--) {
        // Clear overflow flag
        TIFR1 |= (1 << TOV1);

        // Start timer (configured in tim1_ovf_262ms or similar)
        tim1_ovf_262ms();

        // Wait for overflow
        while (!(TIFR1 & (1 << TOV1))) {
        }

        // Clear flag and stop timer
        TIFR1 |= (1 << TOV1);
        tim1_stop();
    }
}

// Draw only the station name line (line 3)
void draw_station_line(const char *text)
{
    oled_gotoxy(0, 3);

    // Clear the line
    for (uint8_t i = 0; i < MAX_CHARS_PER_LINE; i++) {
        oled_putc(' ');
    }

    oled_gotoxy(0, 3);
    oled_puts(text);
}

// Draw static text: title, frequency, volume, time
void draw_static_screen(const RadioInfo *radio, const TunerInfo *tuner)
{
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
    oled_puts(radio->stationName);

    // Frequency
    char frequencyStr[32];
    snprintf(
        frequencyStr,
        sizeof(frequencyStr),
        "Frequency: %u.%u MHz",
        tuner->frequencyInt,
        tuner->frequencyFrac
    );
    oled_gotoxy(0, 4);
    oled_puts(frequencyStr);

    // Volume
    char volumeStr[16];
    snprintf(
        volumeStr,
        sizeof(volumeStr),
        "Volume: %u",
        tuner->volume
    );
    oled_gotoxy(0, 5);
    oled_puts(volumeStr);

    // Time
    char timeStr[16];
    snprintf(
        timeStr,
        sizeof(timeStr),
        "Time: %02u:%02u",
        radio->hours,
        radio->minutes
    );
    oled_gotoxy(0, 6);
    oled_puts(timeStr);

    oled_display();
}

// Scroll station name in an infinite loop
void scroll_station_name(const char *stationName)
{
    size_t nameLen = strlen(stationName);

    // If it fits, just draw once
    if (nameLen <= MAX_CHARS_PER_LINE) {
        draw_station_line(stationName);
        oled_display();
        return;
    }

    // Build scroll buffer: [stationName][spaces...]
    char scrollBuffer[32 + SCROLL_PADDING_SPACES];
    size_t i;

    // Copy name (max 32 chars)
    for (i = 0; i < nameLen && i < 32; i++) {
        scrollBuffer[i] = stationName[i];
    }

    // Add spaces after text
    for (uint8_t s = 0; s < SCROLL_PADDING_SPACES; s++) {
        scrollBuffer[i + s] = ' ';
    }

    size_t scrollLen = i + SCROLL_PADDING_SPACES;  // total length (text + spaces)
    size_t offset = 0;

    // Infinite scroll loop
    while (1) {
        oled_gotoxy(0, 3);

        // Draw one line starting at "offset"
        for (uint8_t col = 0; col < MAX_CHARS_PER_LINE; col++) {
            size_t index = offset + col;
            if (index >= scrollLen) {
                index -= scrollLen; // wrap manually instead of using %
            }
            oled_putc(scrollBuffer[index]);
        }

        oled_display();
        wait_tim1_ovf(2);  // delay between shifts

        offset++;
        if (offset >= scrollLen) {
            offset = 0; // wrap offset
        }
    }
}

int main(void)
{
    draw_static_screen(&g_currentRadio, &g_currentTuner);
    scroll_station_name(g_currentRadio.stationName);
    return 0;
}

