/*
 * ontvangViaSerial.c
 *
 * Created: 4-11-2020 16:36:31
 * Author : Echo
 
 Code om ledjes te laten branden op een arduino middels input die je ingeeft via de terminal
 De Arduino ontvangt data op het RX kanaal van de Serial en leest deze uit
 Aan de hand van de binnengekomen data kijkt de arduino in de while loop welke actie hij moet uitvoeren. 
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#define F_CPU 16000000 //kloksnelheid CPU
#define BAUD 9600 //maximum frequentie bits per seconde
#define BRC ((F_CPU/16/BAUD) -1) //Baud rate calculator = 16 bits

void USART_init( void ){
	UBRR0H = (BRC >> 8); //High byte shift 8 plaatsen om de hoogste 8 bits te pakken en plaatst deze in baudrate registerH
	UBRR0L = (BRC);      //Lower byte is BRC, de laagste 8 bits en plaatst deze in baudrate registerL
	
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0); //"UCSR0B = USART Control and Status Register B" //Zet de receiver aan = "RX op arduino", en de TX aan
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Zet de character size die de receiver kan gebruiken // 8 bits
}

ISR(USART_RX_vect)
{
	handleCommand(UDR0);
	UCSR0B = (0<<RXEN0)|(1<<TXEN0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
}

int main(void)
{
	USART_init();
	DDRB = 0b00001111; //Zet DDRB port 8, 9, 10 en 11 op output
	sei(); //zet external interrupts aan
	
	
	while (1)
	{
		
	}
}

void handleCommand(unsigned char inp){
	
	if(inp = 'a'){
		PORTB = 0x00;
		PORTB ^= 0b00000001; //Zet de LED aan
		} else if(inp = 'b'){
		PORTB = 0x00;
		PORTB ^= 0b00000010;
		} else if (inp = 'c'){
		PORTB = 0x00;
		PORTB ^= 0b00000100;
		} else if(inp = 'd'){
		PORTB = 0x00;
		PORTB ^= 0b00001000;
		} else if (inp = 'e'){
		PORTB = 0x00;
}


unsigned char USART_rx( void ) {
	while( ! (UCSR0A & (1<<RXC0)) ); // while no bit is set
	return UDR0;
}

void USART_Flush( void ) {
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	}
}

