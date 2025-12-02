/*
 * TWI.h
 *
 * Created: 2018-04-11 오전 8:08:44
 *  Author: kiki
 */ 

#ifndef TWI_H
#define TWI_H


#include <avr/io.h>
#include <stdbool.h>

#define TWI_FREQ 100000UL  // 100 kHz

#define F_CPU 8000000UL


#define PC4 4
#define PC5 4

#define TWI_DDR    DDRC
#define TWI_PORT   PORTC
#define SDA_OUT    4
#define SCL_OUT    5

/* Read Write Flag */
#define TWI_WRITE  0x00
#define TWI_READ   0x01

/* Status Register Mask */
#define TWI_TWS_MASK  0xF8

/* TWI Status Code */
#define TWI_START     0x08
#define TWI_RESTART   0x10
#define TWI_MT_SLA_ACK 0x18
#define TWI_MT_SLA_NACK 0x20
#define TWI_MT_DATA_ACK 0x28
#define TWI_MT_DATA_NACK 0x30
#define TWI_MR_SLA_ACK 0x40
#define TWI_MR_SLA_NACK 0x48
#define TWI_MR_DATA_ACK 0x50
#define TWI_MR_DATA_NACK 0x58
#define TWI_NO_INFO   0xF8
#define TWI_BUS_ERROR 0x00

void TWI_Init(void);
bool TWI_Start(uint8_t address, uint8_t read_write);
void TWI_Stop(void);
bool TWI_Write(uint8_t data);
uint8_t TWI_Read(bool ack);
bool TWI_RxBuffer(uint8_t address, uint8_t *data, uint8_t length);
bool TWI_TxBuffer(uint8_t address, const uint8_t *data, uint8_t length);

#endif /* __128A_TWI_H_ */
