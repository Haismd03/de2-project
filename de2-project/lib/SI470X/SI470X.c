#include "twi.h"
#include "SI470X.h"
#include "stdio.h"
#include <string.h>
#include "gpio.h"
#include <util/delay.h>

void si470x_init(SI470X_t *radio)

{
    // Inicializuj ukazatele na shadow registry
    radio->reg00 = (si470x_reg00_t *)&radio->shadowRegisters[0];
    radio->reg01 = (si470x_reg01_t *)&radio->shadowRegisters[1];
    radio->reg02 = (si470x_reg02_t *)&radio->shadowRegisters[2];
    radio->reg03 = (si470x_reg03_t *)&radio->shadowRegisters[3];
    radio->reg04 = (si470x_reg04_t *)&radio->shadowRegisters[4];
    radio->reg05 = (si470x_reg05_t *)&radio->shadowRegisters[5];
    radio->reg06 = (si470x_reg06_t *)&radio->shadowRegisters[6];
    radio->reg07 = (si470x_reg07_t *)&radio->shadowRegisters[7];
    radio->reg08 = (si470x_reg08_t *)&radio->shadowRegisters[8];
    radio->reg09 = (si470x_reg09_t *)&radio->shadowRegisters[9];
    radio->reg0a = (si470x_reg0a_t *)&radio->shadowRegisters[10];
    radio->reg0b = (si470x_reg0b_t *)&radio->shadowRegisters[11];
    radio->reg0c = (si470x_reg0c_t *)&radio->shadowRegisters[12];
    radio->reg0d = (si470x_reg0d_t *)&radio->shadowRegisters[13];
    radio->reg0e = (si470x_reg0e_t *)&radio->shadowRegisters[14];
    radio->reg0f = (si470x_reg0f_t *)&radio->shadowRegisters[15];

    // Inicializace polí (odpovídá C++ konstruktoru)
    uint16_t startBand_default[4] = {8750, 7600, 7600, 6400};
    uint16_t endBand_default[4]   = {10800, 10800, 9000, 10800};
    uint16_t fmSpace_default[4]   = {20, 10, 5, 1};

    memcpy(radio->startBand, startBand_default, sizeof(startBand_default));
    memcpy(radio->endBand, endBand_default, sizeof(endBand_default));
    memcpy(radio->fmSpace, fmSpace_default, sizeof(fmSpace_default));

    // Výchozí hodnoty
    radio->deviceAddress = I2C_DEVICE_ADDR;
    radio->resetPin = -1;
    radio->currentFrequency = 0;
    radio->currentFMBand = FM_BAND_USA_EU;
    radio->currentFMSpace = 0;
    radio->currentVolume = 0;
    radio->rdsInterruptPin = -1;
    radio->seekInterruptPin = -1;
    radio->oscillatorType = OSCILLATOR_TYPE_CRYSTAL;
    radio->maxDelayAfterCrystalOn = MAX_DELAY_AFTER_OSCILLATOR;

    for (uint8_t i = 0; i < 16; ++i) {radio->shadowRegisters[i] = 0;}
}

void si470x_set_all_registers(SI470X_t *radio, uint8_t limit)
{
    word16_to_bytes_t aux;

    twi_start();
    twi_write((radio->deviceAddress << 1) | TWI_WRITE);

    for (uint8_t i = 0x02; i <= limit; i++)
    {
        aux.raw = radio->shadowRegisters[i];
        twi_write(aux.refined.highByte);
        twi_write(aux.refined.lowByte);
    }

    twi_stop();
}

void si470x_get_all_registers(SI470X_t *radio)
{
    word16_to_bytes_t aux;

    twi_start();
    twi_write((radio->deviceAddress << 1) | TWI_WRITE);
    twi_write(0x0A);
    twi_stop();

    _delay_us(1);

    twi_start();
    twi_write((radio->deviceAddress << 1) | TWI_READ);

    for (uint8_t i = 0; i < 16; i++)
    {
        aux.refined.highByte = twi_read(TWI_ACK);
        aux.refined.lowByte  = twi_read((i == 15) ? TWI_NACK : TWI_ACK);
        radio->shadowRegisters[(0x0A + i) % 16] = aux.raw;
    }

    twi_stop();
}

static void si470x_wait_for_stc(SI470X_t *radio, bool shouldBeSet)
{
    // Pokud shouldBeSet==true, čekáme STC==1, jinak čekáme STC==0
    const uint16_t STC_MASK = (1 << 14);
    // timeout (bez blokování navždy) - v iteracích; uprav podle potřeby
    uint16_t timeout = 1000;


    while (timeout--)
    {
        si470x_get_all_registers(radio);
        uint16_t status = radio->shadowRegisters[0x0A]; // STATUSRSSI
        if (shouldBeSet)
        {
            if (status & STC_MASK) return; // STC == 1 -> hotovo
        }
        else
        {
            if (!(status & STC_MASK)) return; // STC == 0 -> hotovo
        }
        _delay_ms(5);
    }
// Pokud dosáhneme sem, timeout - stále pokračujeme, ale není záruka, že STC dosáhl požadovaného stavu
}

void si470x_setChannel(SI470X_t *radio, uint16_t channel)
{
    radio->shadowRegisters[0x03] &= 0xFE00; // Vymaž starý kanál
    radio->shadowRegisters[0x03] |= (channel & 0x03ff); // Nastav nový kanál
    radio->shadowRegisters[0x03] |= (1 << 15); // TUNE = 1

    si470x_set_all_registers(radio, 0x03);

    // Čekej na STC = 1
    si470x_wait_for_stc(radio, true);

    radio->shadowRegisters[0x03] &= ~(1 << 15);    
    si470x_set_all_registers(radio, 0x03);

    si470x_wait_for_stc(radio, false);

    // Aktualizace shadow registrů - načti status
    si470x_get_all_registers(radio);
}

void si470x_setFrequency(SI470X_t *radio, uint16_t frequency)
{
    uint16_t chan;
    uint16_t spacing = radio->fmSpace[radio->currentFMSpace]; // 10, 5 nebo 20 kHz
    uint16_t base = radio->startBand[radio->currentFMBand];   // např. 8750

    chan = (frequency - base) / spacing;

    radio->currentFrequency = frequency;
    si470x_setChannel(radio, chan);
}

void si470x_setMute(SI470X_t *radio, bool value)
{
    if (value)
        radio->shadowRegisters[0x02] &= ~(1 << 14); // DMUTE = 0 -> mute
    else
        radio->shadowRegisters[0x02] |= (1 << 14);  // DMUTE = 1 -> unmute

    si470x_set_all_registers(radio, 0x07);
}

void si470x_setVolume(SI470X_t *radio, uint8_t value)
{
    if (value > 15)
        value = 15;

    radio->shadowRegisters[0x05] &= 0xFFF0; // smaž starou hlasitost
    radio->shadowRegisters[0x05] |= (value & 0x0F);

    radio->currentVolume = value;
    si470x_set_all_registers(radio, 0x07);
}

bool si470x_getRdsAllData(SI470X_t *radio, char **stationName, char **stationInformation, char **programInformation, char **utcTime)
{
    if (!radio->rds_buffer0A[0]) return false; // žádná data

    *stationName        = radio->rds_buffer0A;
    *stationInformation = radio->rds_buffer2B;
    *programInformation = radio->rds_buffer2A;
    *utcTime            = radio->rds_time;

    return true;
}

void si470x_setup(SI470X_t *radio, volatile uint8_t *reset_port, uint8_t reset_pin, uint8_t oscillator_type)
{
    // 🟩 1️⃣ Uložení nastavení do struktury
    radio->resetPort = reset_port;
    radio->resetPin = reset_pin;
    radio->oscillatorType = oscillator_type;
    radio->deviceAddress = I2C_DEVICE_ADDR;

    // 🟩 2️⃣ Inicializace GPIO a TWI
    gpio_mode_output(radio->resetPort, radio->resetPin);
    twi_init();

    // 🟩 3️⃣ Reset zařízení
    gpio_write_low(radio->resetPort, radio->resetPin);
    _delay_ms(1);
    gpio_write_high(radio->resetPort, radio->resetPin);
    _delay_ms(110); // stabilizace

    // 🟩 4️⃣ Inicializace shadow registrů (výchozí hodnoty)
    for (uint8_t i = 0; i < 16; i++)
        radio->shadowRegisters[i] = 0;

    radio->shadowRegisters[0x02] = (1 << 0);

    // Konfigurace napájení a oscilátoru
    radio->shadowRegisters[0x02] = (1 << 0); // ENABLE
    radio->shadowRegisters[0x03] = 0x0000;
    radio->shadowRegisters[0x04] = 0x0400;   // Deemphasis 75 µs
    radio->shadowRegisters[0x05] = 0x0010;   // Volume default
    radio->shadowRegisters[0x07] = 0x0000;

    // 🟩 5️⃣ Zápis do čipu (PowerUp)
    si470x_set_all_registers(radio, 0x07);
    _delay_ms(MAX_DELAY_AFTER_OSCILLATOR);

    // 🟩 6️⃣ Zapnutí RDS a demute
    radio->shadowRegisters[0x02] |= (1 << 14); // DMUTE = 1
    radio->shadowRegisters[0x04] |= (1 << 12); // RDS = 1
    si470x_set_all_registers(radio, 0x07);

    // 🟩 7️⃣ Načtení statusu po inicializaci
    si470x_get_all_registers(radio);

    // Výchozí hodnoty
    radio->currentFrequency = 10170;
    radio->currentVolume = 8;
    radio->currentFMBand = 0;
    radio->currentFMSpace = 0;
}












// void si470x_set_i2c_address(SI470X_t *radio, int addr)
// {
//     radio->deviceAddress = addr;
// }

// void si470x_set_delay_after_crystal_on(SI470X_t *radio, uint8_t ms)
// {
//     radio->maxDelayAfterCrystalOn = ms;
// }

// uint16_t si470x_get_shadow_register(SI470X_t *radio, uint8_t reg)
// {
//     if (reg < 17)
//         return radio->shadowRegisters[reg];
//     return 0;
// }

// void si470x_set_shadow_register(SI470X_t *radio, uint8_t reg, uint16_t value)
// {
//     if (reg < 17)
//         radio->shadowRegisters[reg] = value;
// }

// void si470x_reset(SI470X_t *radio)
// {
//     twi_init();
// 
//     gpio_mode_output(radio->resetPort, radio->resetPin);
// 
//     gpio_write_low(radio->resetPort, radio->resetPin);
//     _delay_ms(1);
// 
//     gpio_write_high(radio->resetPort, radio->resetPin);
//     _delay_ms(110);
// }

// void si470x_powerUp(SI470X_t *radio)
// {
//     si470x_reset(radio);
//     twi_init();
// 
//     radio->shadowRegisters[0x02] = 0x4001; // Enable, Disable Mute, Crystal
//     radio->shadowRegisters[0x03] = 0x0000; // Nastavení kanálu
//     radio->shadowRegisters[0x04] = 0x0000; // Nastavení de-emfáze
//     radio->shadowRegisters[0x05] = 0x0000; // Volume, Seek Threshold
//     radio->shadowRegisters[0x07] = 0x0000; // RDS off zatím
// 
//     si470x_set_all_registers(radio, 0x07);
// 
//     _delay_ms(MAX_DELAY_AFTER_OSCILLATOR);
// 
//     radio->shadowRegisters[0x04] |= (1 << 12); // RDS bit
//     si470x_set_all_registers(radio, 0x07);
// }

// void si470x_powerDown(SI470X_t *radio)
// {
//     radio->shadowRegisters[0x02] &= ~(1 << 0); // Vypne ENABLE bit
//     si470x_set_all_registers(radio, 0x07);
// }

// void si470x_setMono(SI470X_t *radio, bool value)
// {
//     if (value)
//         radio->shadowRegisters[0x02] |= (1 << 13); // MONO = 1
//     else
//         radio->shadowRegisters[0x02] &= ~(1 << 13); // MONO = 0
// 
//     si470x_set_all_registers(radio, 0x07);
// }