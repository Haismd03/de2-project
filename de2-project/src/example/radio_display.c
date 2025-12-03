#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "SI4703.h"
#include "uart.h"
#include <stdlib.h>
#include <stdint.h>
#include "oled.h"
#include <timer.h>
#include <avr/interrupt.h>


uint16_t volume = 7;
uint16_t frequency = 1055;
float frequencyFloat;

// ================================================================================= Radio =================================================================================

uint16_t brnoRadios[41] = { 876, 883, 889, 895, 899, 904, 910, 920, 926, 931,
                            936, 946, 951, 955, 964, 968, 976, 979, 981, 990,
                            994, 999, 1002, 1004, 1008, 1013, 1020, 1025, 1030, 1034,
                            1038, 1041, 1045, 1051, 1055, 1058, 1062, 1065, 1070, 1075, 1078}; 


float foundRadios[41];

uint16_t RSSI;
uint16_t RDSAData;
uint16_t RDSBData;
uint16_t RDSCData;
uint16_t RDSDData;

// bool sweep();
float int_to_float(uint16_t integerValue);
bool SI4703_DecodeRDS_PSName(uint16_t *StatusRSSI, uint16_t *RDSBData, uint16_t *RDSDData);

// ================================================================================ Display ================================================================================

const uint8_t MAX_CHARS_PER_LINE    = 21;  // Characters on one line
const uint8_t SCROLL_PADDING_SPACES = 3;   // Spaces after text

// typedef struct {
//     uint8_t hours;
//     uint8_t minutes;
//     char stationName[32];
// } RadioInfo;

typedef struct {
    uint8_t frequencyInt;
    uint8_t frequencyFrac;
    uint8_t volume;
    uint8_t rssi;
} TunerInfo;

// Draw only the station name line (line 3)
void draw_station_line(const char *text);

// Draw static text: title, frequency, volume, time
void draw_static_screen(const char *stationName, const TunerInfo *tuner);

TunerInfo g_currentTuner = {
    .frequencyInt  = 103,
    .frequencyFrac = 5,
    .volume        = 10,
    .rssi          = 0
};


// ================================================================================= main =================================================================================


int main(void)
{	
	uart_init(UART_BAUD_SELECT(9600, F_CPU));
    sei();

    SI4703_Init();
    SI4703_SetVolume(0);

    //sweep();

    frequencyFloat = int_to_float(frequency);
    SI4703_SetVolume(volume);
    SI4703_SetFreq(frequencyFloat); 
    
    g_currentTuner.frequencyInt = frequency / 10;
    g_currentTuner.frequencyFrac = frequency % 10;
    g_currentTuner.volume = volume;
    
    

    while(1)
    {
        if (SI4703_GetRxRegs(&RSSI, &RDSAData, &RDSBData, &RDSCData, &RDSDData))
        {
            char rssi_uart[10];

            int actual_rssi = RSSI & 0xFF; 
            g_currentTuner.rssi = actual_rssi;
            itoa(actual_rssi, rssi_uart, 10);
            uart_puts("RSSI: ");
            uart_puts(rssi_uart);
            uart_puts("\n");

            if (SI4703_DecodeRDS_PSName(&RSSI, &RDSBData, &RDSDData))
            {
                char* currentName = SI4703_GetPSName();

                draw_static_screen(currentName, &g_currentTuner);
                
                static char previousName[9] = ""; 
                if (strcmp(currentName, previousName) != 0) 
                {
                    uart_puts("PS Name: ");
                    uart_puts(currentName);
                    uart_puts("\n");
                    strcpy(previousName, currentName);
                }
            }
        }
        
        _delay_ms(1500); 
    }
}

/**
 * @brief Ths function goes through the frequencies given in the 'brnoRadios' field and stores those it is really able to receive to the field 'foundRadios'
 * 
 * @return true always
 */

//bool sweep()
//{
//    uint16_t currFreq = brnoRadios[0];
//    uint8_t i = 0;
//    uint8_t j = 0;
//
//    while(currFreq <= brnoRadios[40])
//    {   
//        float currFreqFloat = int_to_float(currFreq);
//        SI4703_SetFreq(currFreqFloat);
//        _delay_ms(1000);
//        if (SI4703_GetRxRegs(&RSSI, &RDSAData, &RDSBData, &RDSCData, &RDSDData))
//        {
//            if (RSSI >= 46)
//            {
//                foundRadios[i] = currFreqFloat;
//                i++;
//            }
//        }
//        j++;
//        currFreq = brnoRadios[j];
//    }
//
//    for(uint8_t i=0; i<41; i++)
//    {
//        uint16_t uart_msg_int = foundRadios[i] * 10;
//        char uart_msg[10];
//        itoa(uart_msg_int, uart_msg, 10);
//        uart_puts(uart_msg);
//        uart_puts("\n");
//    }
//
//    return true;
//}

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
void draw_static_screen(const char *stationName, const TunerInfo *tuner)
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
    oled_gotoxy(6, 2);
    oled_puts(stationName);

    // Frequency
    char frequencyStr[32];
    snprintf(
        frequencyStr,
        sizeof(frequencyStr),
        "Frequency: %u.%u MHz",
        tuner->frequencyInt,
        tuner->frequencyFrac
    );
    oled_gotoxy(0, 3);
    oled_puts(frequencyStr);

    // Volume
    char volumeStr[16];
    snprintf(
        volumeStr,
        sizeof(volumeStr),
        "Volume: %u",
        tuner->volume
    );
    oled_gotoxy(0, 4);
    oled_puts(volumeStr);

    // RSSI
    char rssiStr[10];
    snprintf(
        rssiStr,
        sizeof(rssiStr),
        "RSSI: %u",
        tuner->rssi
    );
    oled_gotoxy(0, 5);
    oled_puts(rssiStr);

    oled_display();
}

float int_to_float(uint16_t integerValue)
{
    float floatValue = (float)integerValue / 10;

    return floatValue;
}


/**
 * @brief This function tries to decode and assamble Program Service (PS) Name from RDS group.
 * It anticipates that the global structure 'shadow' is currently filled with the data from the chip.
 * This function has to be called repeatedly until the whole 8-character name is set.
 *
 * @return true if the valid RDS data was processed
 * @return false if the new data is not ready
 */
//bool SI4703_DecodeRDS_PSName(uint16_t *StatusRSSI, uint16_t *RDSBData, uint16_t *RDSDData)
//{
//    // Error check BLER
//    // Block A (PI code) usually has to be without any error (BLERA = 0)
//    // Block B should have maximally 1 error.
//    uint8_t blera = (*StatusRSSI >> 10) & 0x03;
//	
//	if (blera > 0) 
//    {
//        return false;
//    }
//
//    // Extraction and decoding of the RDS group (Block B)
//    uint16_t RDSB = *RDSBData;
//    uint16_t RDSD = *RDSDData;
//    
//    // Extraction of the Group Type and Version
//    uint8_t GroupType = (RDSB >> 12) & 0x0F; // Bits B15-B12
//    uint8_t Version   = (RDSB >> 11) & 0x01; // Bit B11 (Verse A=0 or B=1)
//    
//    // PS Name is only broadcasted in groups 0A and 0B
//    if (GroupType == 0) 
//    {
//        // Extraction of the PS Name Address (Offset) 
//        // In both cases segment offset is defined with bites B1-B0 of Block B
//        uint8_t PSSegmentOffset = RDSB & 0x03; // Bits B1-B0 give offset 0, 1, 2 or 3.
//        
//        // Calculate index to the field psName
//        int index = PSSegmentOffset * 2; // 0 -> 0, 1 -> 2, 2 -> 4, 3 -> 6
//        
//        if (index < PS_NAME_LENGTH)
//        {
//            // Character 1 (high byte) and character 2 (low byte) are always in Block D
//            // Blok D carries 2 ASCII characters for PS Name
//            stationName[index]     = (char)((RDSD >> 8) & 0xFF); // Character 1 (High Byte)
//            stationName[index + 1] = (char)(RDSD & 0xFF);        // Character 2 (Low Byte)
//        }
//    }
//
//    // End of the chain
//    stationName[PS_NAME_LENGTH] = '\0'; 
//    
//    return true;
//}