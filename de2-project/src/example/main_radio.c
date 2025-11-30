/*
 * SI4703.c
 *
 * Created: 2018-05-29 오후 9:13:44
 * Author : kiki
 */ 

#define F_CPU	8000000UL


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "SI4703.h"
#include "uart.h"

#include <stdlib.h> // pro itoa nebo podobnou funkci

#define PRESET1	88.3    // KISS
#define PRESET2	90.4    // Vltava
#define PRESET3	95.7
#define PRESET4	97.5
#define PRESET5	101.5

float brnoRadios[41] = {87.6, 88.3, 88.9, 89.5, 89.9, 90.4, 91.0, 92.0, 92.6, 93.1,
                        93.6, 94.6, 95.1, 95.5, 96.4, 96.8, 97.6, 97.9, 98.1, 99.0,
                        99.4, 99.9, 100.2, 100.4, 100.8, 101.3, 102.0, 102.5, 103.0, 103.4,
                        103.8, 104.1, 104.5, 105.1, 105.5, 105.8, 106.2, 106.5, 107.0, 107.5, 107.8}; 


float foundRadios[41];

uint16_t RSSI;
uint16_t RDSAData;
uint16_t RDSBData;
uint16_t RDSCData;
uint16_t RDSDData;

bool sweep();

int main(void)
{	
	uart_init(9600);
    SI4703_Init();
    SI4703_SetVolume(0);

    sweep();

    uart_print("\nInitialized\n");
    SI4703_SetVolume(7);
    uart_print("\nVolume set\n");
    SI4703_SetFreq(90.4);
    uart_print("\nFrequency set\n");


    while(1)
    {
        // Používáme pouze jedno čtení, které naplní lokální proměnné
        if (SI4703_GetRxRegs(&RSSI, &RDSAData, &RDSBData, &RDSCData, &RDSDData))
        {
            // --- Tisk RSSI (pouze pro diagnostiku) ---
            char rssi_uart[10];
            // RSSI je 8 bitů v dolní části 16bitového registru 0x0A
            int actual_rssi = RSSI & 0xFF; 
            itoa(actual_rssi, rssi_uart, 10);
            uart_print("RSSI: ");
            uart_print(rssi_uart);
            uart_print("\n");

            // 2. Dekóduj RDS data: Předáváme celý Status registr (RSSI), Blok B a Blok D
            // Poznámka: Kód předpokládá, že jste upravili SI4703_DecodeRDS_PSName dle Opravy 1
            if (SI4703_DecodeRDS_PSName(&RSSI, &RDSBData, &RDSDData))
            {
                // 3. Získání a tisk názvu stanice (pouze při změně)
                char* currentName = SI4703_GetPSName();
                
                // Pro potlačení neustálého tisku:
                static char previousName[9] = ""; 
                //if (strcmp(currentName, previousName) != 0) 
                //{
                    uart_print("PS Name: ");
                    uart_print(currentName);
                    uart_print("\n");
                    strcpy(previousName, currentName);
                //}
            }
        }
        
        // Doporučená prodleva pro RDS (1-2s je obvyklé)
        _delay_ms(1500); 
    }
}


bool sweep()
{
    float currFreq = brnoRadios[0];
    uint8_t index = 0;

    while(currFreq <= brnoRadios[40])
    {
        SI4703_SetFreq(currFreq);
        _delay_ms(500);
        if (SI4703_GetRxRegs(&RSSI, &RDSAData, &RDSBData, &RDSCData, &RDSDData))
        {
            if (RSSI >= 54 && RSSI < 100)
            {
                foundRadios[index] = currFreq;
                index++;
            }
        }
        currFreq = brnoRadios[index];
    }

    for(uint8_t i=0; i<41; i++)
    {
        uint16_t uart_msg_int = foundRadios[i] * 10;
        char uart_msg[10];
        itoa(uart_msg_int, uart_msg, 10);
        uart_print(uart_msg);
        uart_print("\n");
    }

    return true;
}
