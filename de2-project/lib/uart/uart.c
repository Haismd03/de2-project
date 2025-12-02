#include "uart.h"


//#undef BAUD  // avoid compiler warning
//#define BAUD 9600
//
///*
 //* Send character c down the UART Tx, wait until tx holding register
 //* is empty.
 //*/
//int uart_putchar(char c, FILE *stream) {
  //if (c == '\a') {
    //fputs("*ring*\n", stderr);
    //return 0;
  //}
//
  //if (c == '\n')
    //uart_putchar('\r', stream);
  //loop_until_bit_is_set(UCSR0A, UDRE0);
  //UDR0 = (uint8_t) c;
//
  //return 0;
//}
//
//void uart_init(void) {
    //// Initialize the UART
    //static FILE uart_stream = FDEV_SETUP_STREAM(&uart_putchar, NULL, _FDEV_SETUP_RW);
//
    //// Set the baud rate registers
    //UBRR0H = UBRRH_VALUE;
    //UBRR0L = UBRRL_VALUE;
//
//#if USE_2X
    //UCSR0A |= _BV(U2X0);
//#else
    //UCSR0A &= ~_BV(U2X0);
//#endif
//
    //// Enable the transmitter
    //UCSR0B |= _BV(TXEN0);
//
    //// Asynchronous
    //UCSR0C |= (0 << UMSEL00) | (0 << UMSEL01);
    //// Set frame format: 8 data bits, 1 stop bit
    //UCSR0C |= (0 << UCSZ02) | _BV(UCSZ01) | _BV(UCSZ00);
//
    //// Redirect stdout to UART
    //stdout = stderr = &uart_stream;
//}


void uart_init(unsigned int baud) {
	unsigned int ubrr = FOSC/16/baud-1;
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	//BRR0L = 0x67;
	
	unsigned char ubrr_shifted = ubrr << 1; // Shift left κατά μία θέση
	UBRR0L = ubrr_shifted | 0b00000001; // Πρόσθεσε το bit που λείπει
	
	/*Enable receiver and transmitter
	RXEN0: Ενεργοποιεί τον δέκτη (Receiver Enable). Επιτρέπει στον μικροελεγκτή να λαμβάνει δεδομένα.
	TXEN0: Ενεργοποιεί τον πομπό (Transmitter Enable). Επιτρέπει στον μικροελεγκτή να στέλνει δεδομένα.*/
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (0<<USBS0)|(3<<UCSZ00); // 1 stop bit

	/* Set frame format: 8data, 2stop bit */
	//UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	
}

void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));
	
	// Προσθήκη ελέγχου για overflow
	if (UCSR0A & (1<<FE0) || UCSR0A & (1<<DOR0)) {
		uart_print("Buffer overflow detected!\n");
		return;
	}
	
	// Put data into buffer, sends the data
	UDR0 = data;
}

void uart_print(const char* str) {
	while (*str) {
		uart_transmit(*str++);
		_delay_ms(10);
	}
}

void uart_print_binary(uint8_t value) {
	for (int i = 7; i >= 0; i--) {
		if (value & (1 << i)) {
			uart_transmit('1');
			} else {
			uart_transmit('0');
		}
	}
	uart_print("\n");
}





unsigned char uart_receive(void) {
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void uart_print_register_values() {
	char buffer[32];
	
	sprintf(buffer, "UBRR0H: 0x%02X\n", UBRR0H);
	uart_print(buffer);
	
	sprintf(buffer, "UBRR0L: 0x%02X\n", UBRR0L);
	uart_print(buffer);
	
	sprintf(buffer, "UCSR0A: 0x%02X\n", UCSR0A);
	uart_print(buffer);
	
	sprintf(buffer, "UCSR0B: 0x%02X\n", UCSR0B);
	uart_print(buffer);
	
	sprintf(buffer, "UCSR0C: 0x%02X\n", UCSR0C);
	uart_print(buffer);
	
	unsigned int ubrr = FOSC/16/9600-1;
	sprintf(buffer, "ubrr: 0x%02X\n", ubrr);
	uart_print(buffer);
}


// Συνάρτηση μετατροπής ακέραιου σε συμβολοσειρά
//static char uart_// Συνάρτηση μετατροπής ακέραιου σε συμβολοσειρά
char* uart_intToStr(int num, int base) {
	static char buffer[12]; // Στατική μνήμη για αποθήκευση του αποτελέσματος (αρκετή για 32-bit int)
	int i = 0;
	int isNegative = 0;

	// Χειρισμός αρνητικών αριθμών για base 10
	if (num < 0 && base == 10) {
		isNegative = 1;
		num = -num;
	}

	// Μετατροπή αριθμού σε συμβολοσειρά
	do {
		int rem = num % base;
		buffer[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	} while (num != 0);

	// Προσθήκη του αρνητικού προσήμου
	if (isNegative) {
		buffer[i++] = '-';
	}

	buffer[i] = '\0'; // Τερματισμός συμβολοσειράς

	// Αντιστροφή συμβολοσειράς
	int start = 0;
	int end = i - 1;
	while (start < end) {
		char temp = buffer[start];
		buffer[start] = buffer[end];
		buffer[end] = temp;
		start++;
		end--;
	}

	return buffer; // Επιστροφή της μετατρεπόμενης συμβολοσειράς
}


void uart_println(const char* str) {
	uart_print(str);
	uart_transmit('\n');
}

void uart_print_number(uint16_t number) {
	char buffer[10];
	itoa(number, buffer, 10);
	uart_print(buffer);
}