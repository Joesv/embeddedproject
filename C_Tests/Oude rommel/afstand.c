/*
 * distance.c
 *
 * meausers distance betwee 2 and 70 cm
 *
 * Created: 29-6-2016 14:44:43
 *  Author: jacob
 */ 

/* 
 * HC-SR04
 * trigger to sensor : uno 0 (PD0) output
 * echo from sensor  : uno 3 (PD3 = INT1) input
 * 
 * DIO : uno 8  (PB0) data
 * CLK : uno 9  (PB1) clock
 * STB : uno 10 (PB2) strobe
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
volatile uint16_t gv_counter; // 16 bit counter value
volatile uint8_t gv_echo; // a flag
#define BAUD 9600 //maximum frequentie bits per seconde
#define BRC ((F_CPU/16/BAUD) -1) //Baud rate calculator = 16 bits


void uart_init() {
	
	UBRR0H = (BRC >> 8);
	UBRR0L = (BRC);
	UCSR0A = 0; //status data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); //Zet TX en RX aan, en zet een Intterupt bit voor RX
	UCSR0C = (1 << UCSZ01) |  (1 << UCSZ00); //8 bits
	sei(); //zet external interrupts aan
}
void transmitchar(unsigned char data){
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;
}
void transmitString(char *str){
	while(*str){
		transmitchar(*str++);
	}
}
//https://stackoverflow.com/a/32819876
int getIntLength(int num){
	return snprintf(NULL, 0, "%d", num);
}

char* intToString(uint16_t value){
	int length = getIntLength(value);
	char* str = malloc(length +1);
	snprintf(str, length+1, "%d", value);
	return str;
}

void init_ports(void)
{
	DDRB |= 0x01;
}

void trigger(void){
	PORTB |= 0x01; //zet pin 8 high (trigger van afstand)
	_delay_us(10); //10 ms wachten (volgens datasheet)
	PORTB &= ~(0x01); //pin 8 low zetten
}

void init_timer(void)
// prescaling : max time = 2^16/16E6 = 4.1 ms, 4.1 >> 2.3, so no prescaling required
// normal mode, no prescale, stop timer
{
    TCCR1A = 0;
    TCCR1B = 0;
}

void init_ext_int(void)
{
    // any change triggers ext interrupt 1
    EICRA = (1 << ISC10);
    EIMSK = (1 << INT1);
}


uint16_t calc_cm(uint16_t counter)
{
	
    return 0;
}

int main(void)
{
	init_ports();
	uart_init();

	uint8_t pinVal;

	transmitString("hello");
	while(1){
		trigger();
		uint8_t notFinished = 1;
		
		uint16_t counter = 0;
		pinVal = (PINB & (1<<PINB1)) >> PINB1;
		while(!(PINB & (1<<PINB1))){}
		while(notFinished){
			_delay_us(60); //van de datasheet
			counter++;
			pinVal = (PINB & (1<<PINB1));
			if(!pinVal){ // de echo pin is low, het is dus weer ontvangen
				notFinished = 0;
			}
		}
		uint16_t distance = (int)(counter * 60 /58); //spec
		transmitString(intToString(distance));
		transmitString("cm");
		_delay_ms(1000);	
	}
    return 0;
}

ISR (INT1_vect)
{
    return 0;
}
