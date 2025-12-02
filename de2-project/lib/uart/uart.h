/*
 * uart.h
 *
 * Created: 2018-04-14 오전 8:38:56
 *  Author: kiki
 */ 


#ifndef UART_H
#define UART_H

#define F_CPU 8000000UL
#undef BAUD  // avoid compiler warning
#define BAUD 9600
#define FOSC 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <string.h>





void uart_init(unsigned int baud);
void uart_transmit(unsigned char data);
void uart_print(const char* str);
unsigned char uart_receive(void);
void uart_print_register_values();
void uart_println(const char* str);
void uart_print_binary(uint8_t value);
void uart_print_number(uint16_t number);

// Συνάρτηση μετατροπής ακέραιου σε συμβολοσειρά
//static char uart_// Συνάρτηση μετατροπής ακέραιου σε συμβολοσειρά
char* uart_intToStr(int num, int base);



#endif /* 128A_USART_H_ */