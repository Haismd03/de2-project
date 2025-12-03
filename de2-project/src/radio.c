#include "model.h"
#include "SI4703.h"
#include <stdlib.h>
#include <string.h>

void radio_read_regs(project_model_t *model) {
    uint16_t RSSI_raw;
    uint16_t RDSA, RDSB, RDSC, RDSD;
    if (SI4703_GetRxRegs(&RSSI_raw, &RDSA, &RDSB, &RDSC, &RDSD)) {
        //char rssi_uart[10];

        model->RSSI = RSSI_raw & 0xFF; 
        //itoa(model->SI4703.RSSI, rssi_uart, 10);
        //uart_puts("RSSI: ");
        //uart_print(rssi_uart);
        //uart_print("\n");

        if (SI4703_DecodeRDS_PSName(&model->RSSI, &RDSB, &RDSD)) {
            char* currentName = SI4703_GetPSName();

            // TODO: check length
            strcpy(model->station_name, currentName);

            //draw_static_screen(currentName, &g_currentTuner);
            
            /*static char previousName[9] = ""; 
            if (strcmp(currentName, previousName) != 0) {
                //uart_print("PS Name: ");
                //uart_print(currentName);
                //uart_print("\n");
                strcpy(previousName, currentName);
            }*/
        }
    }
}

float int_to_float(uint16_t integerValue)
{
    float floatValue = (float)integerValue / 10;

    return floatValue;
}