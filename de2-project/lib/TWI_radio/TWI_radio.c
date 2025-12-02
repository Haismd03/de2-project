/*
 * TWI.c
 *
 * Created: 2018-04-11 오전 8:08:44
 *  Author: kiki
 */ 

#include "TWI_radio.h"
#include "uart.h"
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

void TWI_Init(void)
{
	//uart_print("Initializing TWI...\n");
    // Ενεργοποίηση των pull-up αντιστάσεων
    DDRC &= ~((1 << PC4) | (1 << PC5));  // Ρύθμιση PC4 και PC5 ως είσοδοι
    
	
	PORTC |= (1 << PC4) | (1 << PC5);    // Ενεργοποίηση των pull-up αντιστάσεων

    // Ρύθμιση του TWI (I2C)
	// initialize twi prescaler and bit rate
	cbi(TWSR, TWPS0);
	cbi(TWSR, TWPS1);
    TWBR = ((F_CPU/TWI_FREQ)-16)/2; // Υπολογισμός του Bit Rate Register
    
	
	TWSR = (0 << TWPS1) | (0 << TWPS0);  // Ρύθμιση του Prescaler
    TWCR = (1 << TWEN);                   // Ενεργοποίηση του TWI	
}

bool TWI_Start(uint8_t address, uint8_t read_write)
{
    TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWEN); // Send start condition
    while (!(TWCR & (1 << TWINT))); // Wait for start condition to be transmitted
    
    TWDR = (address << 1) | read_write; // Load address and R/W bit
    TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
    while (!(TWCR & (1 << TWINT))); // Wait for completion
    
    return (TWSR & TWI_TWS_MASK) == TWI_MT_SLA_ACK || (TWSR & TWI_TWS_MASK) == TWI_MR_SLA_ACK;
}

void TWI_Stop(void)
{
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN); // Send stop condition
    while (TWCR & (1 << TWSTO)); // Wait for stop condition to be executed
}

bool TWI_Write(uint8_t data)
{
    TWDR = data; // Load data into data register
    TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission
    while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
    
    return (TWSR & TWI_TWS_MASK) == TWI_MT_DATA_ACK;
}

uint8_t TWI_Read(bool ack)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (ack ? (1 << TWEA) : 0); // Start reception and send ACK/NACK
    while (!(TWCR & (1 << TWINT))); // Wait for reception to complete
    
    return TWDR; // Return received data
}

bool TWI_RxBuffer(uint8_t address, uint8_t *data, uint8_t length)
{
    if (!TWI_Start(address, TWI_READ)) return false;
    
    for (uint8_t i = 0; i < length; i++)
    {
        data[i] = TWI_Read(i < (length - 1)); // Send ACK for all but last byte
    }
    
    TWI_Stop();
    return true;
}

bool TWI_TxBuffer(uint8_t address, const uint8_t *data, uint8_t length)
{
    if (!TWI_Start(address, TWI_WRITE)) return false;
    
    for (uint8_t i = 0; i < length; i++)
    {
        if (!TWI_Write(data[i])) return false;
    }
    
    TWI_Stop();
	
    return true;
}
