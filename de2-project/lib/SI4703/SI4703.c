/*
* SI4703.c
*
* Created: 2018-05-29 오전 10:43:04
*  Author: kiki
*/

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>
#include "uart.h"

#include "SI4703.h"
//#include "TWI_radio.h"
#include "twi.h"

#define RESET_PIN      PB2
#define RESET_DDR      DDRB
#define RESET_PORT     PORTB

#define SDIO_PIN       PC4
#define SDIO_DDR       DDRC
#define SDIO_PORT      PORTC


uint16_t SI4703_Regs[16] = {0,};

static bool SI4703_Wait(void);
static bool SI4703_RxRegs(void);
static bool SI4703_TxRegs(void);
static void SI4703_Reset(void);

bool SI4703_Init()
{
	/* OUTPUT SDIO, RST */
	//SI4703_DDR |= _BV(SI4703_RST) | _BV(SI4703_SDIO);
	
	/* SDIO Low */
	//SI4703_PORT &= ~_BV(SI4703_SDIO);
	
	uart_puts("SI4703_Init 1\n");
	
	/* SI4703 Reset */
	//SI4703_Reset();
	
	uart_puts("SI4703_Init 2\n");
	
	
	uint8_t Si4703_rstPin=2;
	uint8_t Si4703_sdioPin=4;
	#define RESET_PIN_DDR  DDRB
	#define RESET_PIN_PORT PORTB
	
	RESET_PIN_DDR |= (1 << Si4703_rstPin);  //  Ορίστε το Si4703_rstPin (10(PB2)) ως έξοδο //pinMode(resetPin, OUTPUT);
	DDRC |= (1 << Si4703_sdioPin); // Ορίστε το Si4703_sdioPin ως έξοδο pinMode(SDIO, OUTPUT); //SDIO is connected to A4 for I2C
	
	TWI_PORT &= ~(1 << Si4703_sdioPin); // Θέσε το Si4703_sdioPin σε LOW//  digitalWrite(SDIO, LOW);
	RESET_PIN_PORT &= ~(1 << Si4703_rstPin); //Θέσε το Si4703_rstPin digitalWrite(resetPin, LOW); //Put Si4703 into reset
	_delay_ms(1); //Some delays while we allow pins to settle
	
	RESET_PIN_PORT |= (1 << Si4703_rstPin);// Θέσε το Si4703_rstPin σε HIGH  digitalWrite(resetPin, HIGH); //Bring Si4703 out of reset with SDIO set to low and SEN pulled high with on-board resistor
	_delay_ms(1); //Allow Si4703 to come out of reset
	
	/* Init TWI(I2C) */
	twi_init();
	
	uart_puts("SI4703_Init 3\n");
	
	if(!SI4703_RxRegs()){
		uart_puts("SI4703_Init 4 failed\n");
		return false;
	}
	
	SI4703_UpdateRadioInfo();
	
	uart_puts("SI4703_Init 4 \n");
	
	/* Enable Oscillator  */
	SI4703_Regs[REG_TEST1] = 0x8100;		/* AN230 page 12, why 0x8100 ??? */
	if(!SI4703_TxRegs()) 
	{
		uart_puts("TxRegs() failed\n");
		return false;
	}
	/* Delay minimum 500 ms, AN230 page 12*/
	_delay_ms(500);
	
	if(!SI4703_RxRegs()) 
	{
		uart_puts("RxRegs() failed\n");
		return false;
	}
	/* Power-up sequence */
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_DMUTE);
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_ENABLE);
	
	/* Set Force Mode for single speaker */
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_MONO);
	
	/* Set Seek Mode as Stop at band limit */
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_SKMODE);
	
	/* Enable RDS */
	SI4703_Regs[REG_SYSCONFIG1] |= (1 << IDX_RDS);
	
	/* Set De-Emphasis 75us (Korea) */
	SI4703_Regs[REG_SYSCONFIG1] &= ~(1 << IDX_DE);
	
	/* Set Band as 00 (Korea) */
	SI4703_Regs[REG_SYSCONFIG2] &= ~((1 << IDX_BAND0)|(1 << IDX_BAND1));
	
	/* Set Space as 00 (Korea) */
	SI4703_Regs[REG_SYSCONFIG2] &= ~((1 << IDX_SPACE0)|(1 << IDX_SPACE1));
	
	/* Set Volume as 0x0F */
	SI4703_Regs[REG_SYSCONFIG2] &= 0xFFF0;
	SI4703_Regs[REG_SYSCONFIG2] |= 0x0F;
	
	/* Set Seek Threshold, Recommended 0x19 */
	SI4703_Regs[REG_SYSCONFIG2] &= ~(MASK_SEEKTH);
	SI4703_Regs[REG_SYSCONFIG2] |= (0x19 << 8);
	
	/* Set SKSNR, Recommended 0x04 */
	SI4703_Regs[REG_SYSCONFIG3] &= ~(MASK_SKSNR);
	SI4703_Regs[REG_SYSCONFIG3] |= (0x04 << 4);
	
	/* Set SKCNT, Recommended 0x08 */
	SI4703_Regs[REG_SYSCONFIG3] &= ~(MASK_SKCNT);
	SI4703_Regs[REG_SYSCONFIG3] |= 0x08;
	
	
	if(!SI4703_TxRegs()) return false;	
	
	#ifdef IN_EUROPE
	SI4703_Regs[REG_SYSCONFIG1] |= (1<<IDX_DE); //50kHz Europe setup
	SI4703_Regs[REG_SYSCONFIG2] |= (1<<IDX_SPACE0); //100kHz channel spacing for Europe
	#else
	SI4703_Regs[REG_SYSCONFIG2] &= ~(1<<IDX_SPACE1 | 1<<IDX_SPACE0) ; //Force 200kHz channel spacing for USA
	#endif
	
	
	//shadow.reg.SYSCONFIG2.word = SI4703_Regs[REG_SYSCONFIG2] ;
	
	//SI4703_Regs[REG_SYSCONFIG2] &= 0xFFF0; // 0xFFF0=1111111111110000
	//SI4703_Regs[REG_SYSCONFIG2] |= 0x0001;   // 0x0F=          00001111
		
	//shadow.reg.SYSCONFIG2.bits.VOLUME=2;
	
	
	//uart_puts_number(SI4703_Regs[REG_SYSCONFIG2]);
	//uart_puts(" \n");
	
	SI4703_TxRegs(); //Update
	
	/* Wait Powerup Time(110ms), Datasheet page 13 */
	_delay_ms(110);
	
	if(!SI4703_SetVolume(2)) return false;
	_delay_ms(100);
	//gotoChannel(1003);
	//if(!SI4703_SetFreq(100)) return false;
	
	
	_delay_ms(100);
	
	uart_puts("Freq=");
	//uart_puts(uart_intToStr(SI4703_GetFreq(),10)) ;
	uart_puts("\n");
	return true;
}


bool SI4703_SetVolume(uint8_t volume)
{
	if(volume > 15) volume = 15;
	
	if(!SI4703_RxRegs()) return false;
	
	/* Set Volume */
	SI4703_Regs[REG_SYSCONFIG2] &= 0xFFF0;
	SI4703_Regs[REG_SYSCONFIG2] |= volume;

	shadow.reg.SYSCONFIG2.word = SI4703_Regs[REG_SYSCONFIG2];
	
	if(!SI4703_TxRegs()) return false;
	
	return true;
}

bool SI4703_SetMono(bool mono)
{
	if(!SI4703_RxRegs()) return false;
	
	/* Set Mono or Stereo */
	if(mono)
	{
		SI4703_Regs[REG_POWERCFG] |= (1 << IDX_MONO);
	}
	else
	{
		SI4703_Regs[REG_POWERCFG] &= ~(1 << IDX_MONO);
	}
	
	if(!SI4703_TxRegs()) return false;
	
	return true;
}

bool SI4703_SetMute(bool mute)
{
	if(!SI4703_RxRegs()) return false;
	
	/* Set Mute */
	if(mute)
	{
		SI4703_Regs[REG_POWERCFG] &= ~(1 << IDX_DMUTE);
	}
	else
	{
		SI4703_Regs[REG_POWERCFG] |= (1 << IDX_DMUTE);
	}
	
	if(!SI4703_TxRegs()) return false;
	
	return true;
}

float SI4703_GetFreq()
{
	float freq;
	
	if(!SI4703_RxRegs()) return false;
	
	uint16_t channel = SI4703_Regs[REG_READCHAN] & MASK_READCHAN;
	
	/* F = (S x C) + L */
	freq = (0.2 * channel) + 87.5;	/* Band rage is between 87.5 and 108MHz, Channel Space is 0.2MHz for Korea */
	
	return freq;
}

bool SI4703_SetFreq(float freq)
{
	if(freq < 87.5) freq = 87.5;
	if(freq > 108.0) freq = 108.0;
	
	/* C = (F - L) / S */
	uint16_t channel = (freq - 87.5) / 0.2;
	
	if(!SI4703_RxRegs()) return false;
	
	/* Update frequency */
	SI4703_Regs[REG_CHANNEL] &= 0xFE00;
	SI4703_Regs[REG_CHANNEL] |= channel;
	SI4703_Regs[REG_CHANNEL] |= (1 << IDX_TUNE);
	
	if(!SI4703_TxRegs()) return false;
	
	/* Wait STC bit set & clear */
	if(!SI4703_Wait()) return false;
	
	return true;
}

void gotoChannel(int newChannel){
	//Freq(MHz) = 0.200(in USA) * Channel + 87.5MHz
	//97.3 = 0.2 * Chan + 87.5
	//9.8 / 0.2 = 49
	newChannel *= 10; //973 * 10 = 9730
	newChannel -= 8750; //9730 - 8750 = 980

	#ifdef IN_EUROPE
	newChannel /= 10; //980 / 10 = 98
	#else
	newChannel /= 20; //980 / 20 = 49
	#endif

	//These steps come from AN230 page 20 rev 0.5
	SI4703_RxRegs();
	SI4703_Regs[REG_CHANNEL] &= 0xFE00; //Clear out the channel bits
	SI4703_Regs[REG_CHANNEL] |= newChannel; //Mask in the new channel
	SI4703_Regs[REG_CHANNEL] |= (1<<IDX_TUNE); //Set the TUNE bit to start
	SI4703_TxRegs();

	_delay_ms(60); //Wait 60ms - you can use or skip this delay

	//Poll to see if STC is set
	//while(1) {
	//SI4703_RxRegs();
	//if( (SI4703_Regs[REG_STATUSRSSI] & (1<<IDX_STC)) != 0) break; //Tuning complete!
	//uart_puts("1. gotoChannel Tuning \n");
	//}

	SI4703_RxRegs();
	SI4703_Regs[REG_CHANNEL] &= ~(1<<IDX_TUNE); //Clear the tune after a tune has completed
	SI4703_TxRegs();

	//Wait for the si4703 to clear the STC as well
	//while(1) {
	//SI4703_RxRegs();
	//if( (SI4703_Regs[REG_STATUSRSSI] & (1<<IDX_STC)) == 0) break; //Tuning complete!
	//uart_puts("2. gotoChannel Waiting... \n");
	//}
}

bool SI4703_SeekUp()
{
	if(!SI4703_RxRegs()) return false;
	
	/* Set SEEKUP + SEEK bit */
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_SEEKUP);
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_SEEK);
	
	if(!SI4703_TxRegs()) return false;
	
	/* Wait STC bit set & clear */
	if(!SI4703_Wait()) return false;
	
	return true;
}

bool SI4703_SeekDown()
{
	if(!SI4703_RxRegs()) return false;
	
	/* Clear SEEKUP + SEEK bit */
	SI4703_Regs[REG_POWERCFG] &= ~(1 << IDX_SEEKUP);
	SI4703_Regs[REG_POWERCFG] |= (1 << IDX_SEEK);
	
	if(!SI4703_TxRegs()) return false;
	
	/* Wait STC bit set & clear */
	if(!SI4703_Wait()) return false;
	
	return true;
}

bool SI4703_CheckRDSReady()
{
	if(!SI4703_RxRegs()) return false;
	
	return (bool)((SI4703_Regs[REG_STATUSRSSI] & MASK_RDSR) >> 15);
}

static bool SI4703_Wait(void)
{
	uint8_t timeout = 0;
	
	while(1)
	{
		if(!SI4703_RxRegs()) return false;
		if((SI4703_Regs[REG_STATUSRSSI] & MASK_STC) != 0) break;
		_delay_ms(60);	/* Seek or Tune Time Delay */
		
		timeout++;
		if(timeout > 10) return false;
	}
	
	if(SI4703_Regs[REG_STATUSRSSI] & MASK_SFBL) return false;
	
	timeout = 0;
	
	while(1)
	{
		SI4703_Regs[REG_POWERCFG] &= ~(1 << IDX_SEEK);
		SI4703_Regs[REG_CHANNEL] &= ~(1 << IDX_TUNE);
		if(!SI4703_TxRegs()) return false;
		
		if(!SI4703_RxRegs()) return false;
		if((SI4703_Regs[REG_STATUSRSSI] & MASK_STC) == 0) break;
		_delay_ms(60);	/* Seek or Tune Time Delay */
		
		timeout++;
		if(timeout > 10) return false;
	}
	
	return true;
}

static bool SI4703_RxRegs()
{
	uint8_t buffer[32];
	
	if(!TWI_RxBuffer(SI4703_DEVICEADDR, buffer, sizeof(buffer))) return false;
	
	/* Cautions!!! Si4703 returns data from register 0x0A */
	for(int i=0; i < 6; i++)
	{
		SI4703_Regs[10+i] = (buffer[i*2] << 8) | buffer[i*2+1];
	}
	
	for(int i=0; i < 10; i++)
	{
		SI4703_Regs[i] = (buffer[12+2*i] << 8) | buffer[13+2*i];
	}
	
	return true;
}

static bool SI4703_TxRegs()
{
	uint8_t buffer[12];
	
	buffer[0] = SI4703_Regs[REG_POWERCFG] >> 8;
	buffer[1] = SI4703_Regs[REG_POWERCFG] & 0xFF;
	buffer[2] = SI4703_Regs[REG_CHANNEL] >> 8;
	buffer[3] = SI4703_Regs[REG_CHANNEL] & 0xFF;
	buffer[4] = SI4703_Regs[REG_SYSCONFIG1] >> 8;
	buffer[5] = SI4703_Regs[REG_SYSCONFIG1] & 0xFF;
	
	buffer[6] = shadow.reg.SYSCONFIG2.word >> 8;// SI4703_Regs[REG_SYSCONFIG2] >> 8;
	buffer[7] = shadow.reg.SYSCONFIG2.word  & 0xFF; //SI4703_Regs[REG_SYSCONFIG2] & 0xFF;
	//uart_puts("REG_SYSCONFIG2=");
	//uart_puts_number(SI4703_Regs[REG_SYSCONFIG2]);
	//uart_puts(" \n");
	//uart_puts("shadow.reg\n");
	//uart_puts_number(shadow.reg.SYSCONFIG2.word);
	//uart_puts("\n");
	
	
	
	buffer[8] = SI4703_Regs[REG_SYSCONFIG3] >> 8;
	buffer[9] = SI4703_Regs[REG_SYSCONFIG3] & 0xFF;
	buffer[10] = SI4703_Regs[REG_TEST1] >> 8;
	buffer[11] = SI4703_Regs[REG_TEST1] & 0xFF;
	
	if(!TWI_TxBuffer(SI4703_DEVICEADDR, buffer, sizeof(buffer))) return false;
	//uart_puts("k;k;k;lk;lk;lkk;klmkjlkjlkjlkjlkjlj\n");
	
	
	return true;
}

static void SI4703_Reset(void)
{
	SI4703_PORT &= ~(1 << SI4703_RST);
	_delay_ms(10);							/* Min 100us */
	SI4703_PORT |= (1 << SI4703_RST);
	_delay_ms(10);							/* Min 100us */
}

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

/**
 * @brief This function stores RDS data and RSSI value to the given variables.
 * 
 * @param RSSI variable for storing the value of RSSI
 * @param RDSAData variable for storing the first part of the RDS data
 * @param RDSBData variable for storing the second part of the RDS data
 * @param RDSCData variable for storing the third part of the RDS data
 * @param RDSDData variable for storing the fourth part of the RDS data
 * @return true if all data is succesfully stored
 * @return false if no data is found
 */

bool SI4703_GetRxRegs(uint16_t *RSSI, uint16_t *RDSAData, uint16_t *RDSBData, uint16_t *RDSCData, uint16_t *RDSDData)
{	
	uint8_t buffer[32];

	if(!TWI_RxBuffer(SI4703_DEVICEADDR, buffer, sizeof(buffer))) return false;

	*RSSI = (buffer[0] << 8) | buffer[1];
	
	*RDSAData = (buffer[4] << 8) | buffer[5];
	*RDSBData = (buffer[6] << 8) | buffer[7];
	*RDSCData = (buffer[8] << 8) | buffer[9];
	*RDSDData = (buffer[10] << 8) | buffer[11];

	return true;
}

#define PS_NAME_LENGTH 8 // Name of the station has 8 characters

// Global variable for storing of the result
static char psName[PS_NAME_LENGTH + 1] = "        "; // Initialization with spaces

/**
 * @brief This function tries to decode and assamble Program Service (PS) Name from RDS group.
 * It anticipates that the global structure 'shadow' is currently filled with the data from the chip.
 * This function has to be called repeatedly until the whole 8-character name is set.
 *
 * @return true if the valid RDS data was processed
 * @return false if the new data is not ready
 */
bool SI4703_DecodeRDS_PSName(uint16_t *StatusRSSI, uint16_t *RDSBData, uint16_t *RDSDData)
{
    // Error check BLER
    // Block A (PI code) usually has to be without any error (BLERA = 0)
    // Block B should have maximally 1 error.
    uint8_t blera = (*StatusRSSI >> 10) & 0x03;
	
	if (blera > 0) 
    {
        return false;
    }

    // Extraction and decoding of the RDS group (Block B)
    uint16_t RDSB = *RDSBData;
    uint16_t RDSD = *RDSDData;
    
    // Extraction of the Group Type and Version
    uint8_t GroupType = (RDSB >> 12) & 0x0F; // Bits B15-B12
    uint8_t Version   = (RDSB >> 11) & 0x01; // Bit B11 (Verse A=0 or B=1)
    
    // PS Name is only broadcasted in groups 0A and 0B
    if (GroupType == 0) 
    {
        // Extraction of the PS Name Address (Offset) 
        // In both cases segment offset is defined with bites B1-B0 of Block B
        uint8_t PSSegmentOffset = RDSB & 0x03; // Bits B1-B0 give offset 0, 1, 2 or 3.
        
        // Calculate index to the field psName
        int index = PSSegmentOffset * 2; // 0 -> 0, 1 -> 2, 2 -> 4, 3 -> 6
        
        if (index < PS_NAME_LENGTH)
        {
            // Character 1 (high byte) and character 2 (low byte) are always in Block D
            // Blok D carries 2 ASCII characters for PS Name
            psName[index]     = (char)((RDSD >> 8) & 0xFF); // Character 1 (High Byte)
            psName[index + 1] = (char)(RDSD & 0xFF);        // Character 2 (Low Byte)
        }
    }

    // End of the chain
    psName[PS_NAME_LENGTH] = '\0'; 
    
    return true;
}

/**
 * @brief This function gets the current PS Name
 * 
 * @return char* PS Name
 */

// Function for getting the current PS Name
char* SI4703_GetPSName(void)
{
    return psName;
}


/**
 * @brief This function reads the registers from the chip and copies the data from the field SI4703_Regs to shadow structure
 * 
 * @return true if all data is succesfully copied
 * @return false if the error during the reading of the registers occurs
 */

bool SI4703_UpdateRadioInfo(void)
{
    // Reads the registers from the chip to field SI4703_Regs
    if (!SI4703_RxRegs()) {
        return false;
    }

    // Synchronization - copies the data from the field SI4703_Regs to shadow structure
    for (int i = 0; i < 6; i++) {
        shadow.word[i] = SI4703_Regs[10 + i];
    }

    for (int i = 0; i < 10; i++) {
        shadow.word[6 + i] = SI4703_Regs[i];
    }

    return true;
}