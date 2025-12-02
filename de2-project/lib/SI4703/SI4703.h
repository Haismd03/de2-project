/*
 * SI4703.h
 *
 * Created: 2018-05-29 오전 10:42:54
 *  Author: kiki
 */ 


#ifndef SI4703_H_
#define SI4703_H_

//#include <util/setbaud.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include "uart.h"

#define SI4703_DDR		DDRC
#define SI4703_PORT		PORTC
#define SI4703_PIN		PINC
#define SI4703_SDIO		PC4
#define SI4703_SCLK		PC5
#define SI4703_RST		PB2		

#define SI4703_DEVICEADDR	0x10

#define REG_DEVICEID		0x00
#define REG_CHIPID			0x01
#define REG_POWERCFG		0x02
#define REG_CHANNEL			0x03
#define REG_SYSCONFIG1		0x04
#define REG_SYSCONFIG2		0x05
#define REG_SYSCONFIG3		0x06
#define REG_TEST1			0x07
#define REG_TEST2			0x08
#define REG_BOOTCONFIG		0x09
#define REG_STATUSRSSI		0x0A
#define REG_READCHAN		0x0B
#define REG_RDSA			0x0C
#define REG_RDSB			0x0D
#define REG_RDSC			0x0E
#define REG_RDSD			0x0F

#define MASK_PN				0xF000
#define MASK_MFGID			0x0FFF
#define MASK_REV			0xFC00
#define MASK_DEV			0x03C0
#define MASK_FIRMWARE		0x003F
#define MASK_DSMUTE			0x8000
#define MASK_DMUTE			0x4000
#define MASK_MONO			0x2000
#define MASK_RDSM			0x0800
#define MASK_SKMODE			0x0400
#define MASK_SEEKUP			0x0200
#define MASK_SEEK			0x0100
#define MASK_DISABLE		0x0040
#define MASK_ENABLE			0x0001
#define MASK_TUNE			0x8000
#define MASK_CHAN			0x03FF
#define MASK_RDSIEN			0x8000
#define MASK_STCIEN			0x4000
#define MASK_RDS			0x1000
#define MASK_DE				0x0800
#define MASK_AGCD			0x0400
#define MASK_BLNDADJ		0x00C0
#define MASK_GPIO3			0x0030
#define MASK_GPIO2			0x000C
#define MASK_GPIO1			0x0003
#define MASK_SEEKTH			0xFF00
#define MASK_BAND			0x00C0
#define MASK_SPACE			0x0030
#define MASK_VOLUME			0x000F
#define MASK_SMUTER			0xC000
#define MASK_SMUTEA			0x3000
#define MASK_VOLEXT			0x0100
#define MASK_SKSNR			0x00F0
#define MASK_SKCNT			0x000F
#define MASK_XOSCEN			0x8000
#define MASK_AHIZEN			0x4000
#define MASK_RDSR			0x8000
#define MASK_STC			0x4000
#define MASK_SFBL			0x2000
#define MASK_AFCRL			0x1000
#define MASK_RDSS			0x0800
#define MASK_BLERA			0x0600
#define MASK_ST				0x0100
#define MASK_RSSI			0x00FF
#define MASK_BLERB			0xC000
#define MASK_BLERC			0x3000
#define MASK_BLERD			0x0C00
#define MASK_READCHAN		0x03FF

#define IDX_DSMUTE			15
#define IDX_DMUTE			14
#define IDX_MONO			13
#define IDX_RDSM			11
#define IDX_SKMODE			10
#define IDX_SEEKUP			9
#define IDX_SEEK			8
#define IDX_DISABLE			6
#define IDX_ENABLE			0
#define IDX_TUNE			15
#define IDX_RDSIEN			15
#define IDX_STCIEN			14
#define IDX_RDS				12
#define IDX_DE				11
#define IDX_AGCD			10
#define IDX_BAND0			6			
#define IDX_BAND1			7
#define IDX_SPACE0			4
#define IDX_SPACE1			5
#define IDX_SMUTER0			14
#define IDX_SMUTER1			15
#define IDX_SMUTEA0			12
#define IDX_SMUTEA1			13
#define IDX_VOLEXT			8
#define IDX_XOSCEN			15
#define IDX_AHIZEN			14
#define IDX_RDSR			15
#define IDX_STC				14
#define IDX_SFBL			13
#define IDX_AFCRL			12
#define IDX_RDSS			11
#define IDX_ST				8

#define IN_EUROPE


///////////////////////////////////////////////////////////////////////////

// Registers shadow
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t MFGID   :12;   // Manufacturer ID
		uint8_t  PN      :4;    // Part Number
	} bits;
} DEVICEID_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t FIRMWARE:6;    // Firmware Version
		uint16_t DEV     :4;    // Device
		uint16_t REV     :6;    // Chip Version
	} bits;
} CHIPID_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t ENABLE  :1;    // Powerup Enable
		uint16_t         :5;    // Reserved
		uint16_t DISABLE :1;    // Powerup Disable
		uint16_t         :1;    // Reserved
		uint16_t SEEK    :1;    // Seek Disable/Enable
		uint16_t SEEKUP  :1;    // Seek Direction Down/Up
		uint16_t SKMODE  :1;    // Seek Mode Wrap/Stop
		uint16_t RDSM    :1;    // RDS Mode Standard/Verbose
		uint16_t         :1;    // Reserved
		uint16_t MONO    :1;    // Mono Select Stereo/Mono
		uint16_t DMUTE   :1;    // Mute Disable Enable/Disable
		uint16_t DSMUTE  :1;    // Softmute Disable Enable/Disable
	} bits;
} POWERCFG_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t CHAN    :10;   // Channel Select
		uint16_t         :5;    // Reserved
		uint16_t TUNE    :1;    // Tune Disable/Enable
	} bits;
} CHANNEL_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t GPIO1   :2;    // General Purpose I/O 1
		uint16_t GPIO2   :2;    // General Purpose I/O 2
		uint16_t GPIO3   :2;    // General Purpose I/O 3
		uint16_t BLNDADJ :2;    // Stereo/Mono Blend Level Adjustment
		uint16_t         :2;    // Reserved
		uint16_t AGCD    :1;    // AGC Disable Enable/Disable
		uint16_t DE      :1;    // De-emphasis
		uint16_t RDS     :1;    // RDS Enable Disable/Enable
		uint16_t         :1;    // Reserved
		uint16_t STCIEN  :1;    // Seek/Tune Complete Interrupt Enable Disable/Enable
		uint16_t RDSIEN  :1;    // RDS Interrupt Enable Disable/Enable
	} bits;
} SYSCONFIG1_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t VOLUME  :4;    // Volume 0-15
		uint16_t SPACE   :2;    // Channel Spacing
		uint16_t BAND    :2;    // Band Select
		uint16_t SEEKTH  :8;    // RSSI Seek Threshold
	} bits;
} SYSCONFIG2_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t SKCNT   :4;    // Seek FM Impulse Detection Threshold
		uint16_t SKSNR   :4;    // Seek SNR Threshold
		uint16_t VOLEXT  :1;    // Extended Volume Range
		uint16_t         :3;    // Reserved
		uint16_t SMUTEA  :2;    // Softmute Attenuation
		uint16_t SMUTER  :2;    // Softmute Attack/Recover Rate
	} bits;
} SYSCONFIG3_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t         :14;   // Reserved
		uint16_t AHIZEN  :1;    // Audio High-Z Enable Disable/Enable
		uint16_t XOSCEN  :1;    // Crystal Oscillator Enable Disable/Enable
	} bits;
} TEST1_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t         :16;   // Reserved
	} bits;
} TEST2_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t         :16;   // Reserved
	} bits;
} BOOTCONFIG_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t RSSI    :8;    // RSSI
		uint16_t ST      :1;    // Stereo Indicator
		uint16_t BLERA   :2;    // RDS Block A Errors
		uint16_t RDSS    :1;    // RDS Synchronized
		uint16_t AFCRL   :1;    // AFC Rail
		uint16_t SFBL    :1;    // Seek Fail/Band Limit
		uint16_t STC     :1;    // Seek/Tune Complete
		uint16_t RDSR    :1;    // RDS Ready
	} bits;
} STATUSRSSI_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t READCHAN:10;   // Read Channel
		uint16_t BLERD   :2;    // RDS Block D Errors
		uint16_t BLERC   :2;    // RDS Block C Errors
		uint16_t BLERB   :2;    // RDS Block B Errors
	} bits;
} READCHAN_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t RDSA    :16;   // RDSA
	} bits;
} RDSA_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t RDSB    :16;   // RDSB
	} bits;
} RDSB_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t RDSC    :16;   // RDSC
	} bits;
} RDSC_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word;
	struct {
		uint16_t RDSD    :16;   // RDSD
	} bits;
} RDSD_t;
//------------------------------------------------------------------------------------------------------------
typedef union {
	uint16_t word[16];           // 32 bytes = 16 x 16 bits Registers
	struct {
		STATUSRSSI_t STATUSRSSI; // Register 0x0A - 00
		READCHAN_t   READCHAN;   // Register 0x0B - 01
		RDSA_t       RDSA;       // Register 0x0C - 02
		RDSB_t       RDSB;       // Register 0x0D - 03
		RDSC_t       RDSC;       // Register 0x0E - 04
		RDSD_t       RDSD;       // Register 0x0F - 05
		DEVICEID_t   DEVICEID;   // Register 0x00 - 06
		CHIPID_t     CHIPID;     // Register 0x01 - 07
		POWERCFG_t   POWERCFG;   // Register 0x02 - 08
		CHANNEL_t    CHANNEL;    // Register 0x03 - 09
		SYSCONFIG1_t SYSCONFIG1; // Register 0x04 - 10
		SYSCONFIG2_t SYSCONFIG2; // Register 0x05 - 11
		SYSCONFIG3_t SYSCONFIG3; // Register 0x06 - 12
		TEST1_t      TEST1;      // Register 0x07 - 13
		TEST2_t      TEST2;      // Register 0x08 - 14
		BOOTCONFIG_t BOOTCONFIG; // Register 0x09 - 15
	} reg;
} shadow_t;

static shadow_t shadow;  // Static instance of the shadow register structure

//////////////////////////////////////////////////////////////////////////

// Band Settings
static int _band;					// Band Range code
static int _space;					// Band Spacing code
static int _de;					// De-Emphasis
static int	_bandStart;				// Bottom of Band (MHz)
static int	_bandEnd;				// Top of Band (MHz)
static int	_bandSpacing;			// Band Spacing (MHz)

// Seek Settings
static int _skmode;				// Seek Mode
static int _seekth;				// Seek Threshold
static int _skcnt;					// Seek Clicks Number Threshold
static int _sksnr;					// Seek Signal/Noise Ratio
static int _agcd;					// AGC disable


//------------------------------------------------------------------------------------------------------------

// Band Select
static const uint8_t  	BAND_US_EU		= 0b00;	// 87.5–108 MHz (US / Europe, Default)
static const uint8_t  	BAND_JPW		= 0b01;	// 76–108 MHz (Japan wide band)
static const uint8_t  	BAND_JP			= 0b10;	// 76–90 MHz (Japan)

// De-emphasis
static const uint8_t	DE_75us			= 0b0;	// De-emphasis 75 μs. Used in USA (default)
static const uint8_t	DE_50us			= 0b1;	// De-emphasis 50 μs. Used in Europe, Australia, Japan.

// Channel Spacing
static const uint8_t  	SPACE_200KHz	= 0b00;	// 200 kHz (US / Australia, Default)
static const uint8_t  	SPACE_100KHz 	= 0b01;	// 100 kHz (Europe / Japan)
static const uint8_t  	SPACE_50KHz  	= 0b10;	//  50 kHz (Other)

// Seek Mode
static const uint8_t 	SKMODE_WRAP		= 0b0;	// Wrap when reaching band limit
static const uint8_t 	SKMODE_STOP		= 0b1;	// Stop when reaching band limit

// Seek SNR Threshold
static const uint8_t 	SKSNR_DIS		= 0x0; // disabled (default) (Values are 0x0 to 0xF)
static const uint8_t 	SKSNR_MIN		= 0x1; // min value (most stops)
static const uint8_t 	SKSNR_MAX		= 0xF; // max value (fewest stops)

// Seek FM Impulse Detection Threshold
static const uint8_t 	SKCNT_DIS		= 0x0; // disabled (default) (Values are 0x0 to 0xF)
static const uint8_t 	SKCNT_MAX		= 0x1; // max value (most stops)
static const uint8_t 	SKCNT_MIN		= 0xF; // min value (fewest stops)



bool SI4703_Init(void);
bool SI4703_SetVolume(uint8_t volume);
bool SI4703_SetMono(bool mono);
bool SI4703_SetMute(bool mute);
float SI4703_GetFreq();
bool SI4703_SetFreq(float freq);
void gotoChannel(int newChannel);
bool SI4703_SeekUp(void);
bool SI4703_SeekDown(void);
bool SI4703_UpdateRadioInfo(void);
bool SI4703_CheckRDSReady(void);

bool SI4703_GetRxRegs(uint16_t *RSSI, uint16_t *RDSAData, uint16_t *RDSBData, uint16_t *RDSCData, uint16_t *RDSDData);
bool SI4703_UpdateRadioInfo(void);
char* SI4703_GetPSName(void);
bool SI4703_DecodeRDS_PSName(uint16_t *StatusRSSI, uint16_t *RDSBData, uint16_t *RDSDData);

#endif /* SI4703_H_ */