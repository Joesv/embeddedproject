#include <stdio.h>
#include <stdlib.h>
#define F_CPU 16E6
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#define VREF 5
#define arrlen(arr) (sizeof(arr) / sizeof(arr[0]))
#define UBBRVAL 51

void uart_init() {
	UBRR0H = 0;
	UBRR0L = UBBRVAL;
	UCSR0A = 0;
	UCSR0B = _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) |  _BV(UCSZ00);
}

//transmit de int
void transmit(uint16_t data){
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;
}
//zelfde als de vorige alleen dan met een char als parameter
void transmitchar(unsigned char data){
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;
}
// https://www.avrfreaks.net/forum/help-transmitting-strings-usart
void transmitString(char *str){
	while(*str){
		transmitchar(*str++);
	}
}
//https://stackoverflow.com/a/32819876
int getIntLength(int num){
	return snprintf(NULL, 0, "%d", num);
}
//https://embedds.com/adc-on-atmega328-part-1/
void InitADC()
{
	// Select Vref=AVcc
	ADMUX |= (1<<REFS0);
	//set prescaller to 128 and enable ADC
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);
}
uint16_t ReadADC(uint8_t ADCchannel)
{
	//select ADC channel with safety mask
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//single conversion mode
	ADCSRA |= (1<<ADSC);
	// wait until ADC conversion is complete
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}
int main()
{
	uint16_t val;
	//initialize ADC
	InitADC();
	//Initialize USART0
	uart_init();
	while(1)
	{
		val = ReadADC(0);
		
		int length = getIntLength(val);
		char* str = malloc(length +1 );
		snprintf(str, length+1, "%d", val);		// heb nog geen goede manier gevonden om het getal (max 1023) via normale bits te sturen, (of eerder, de python code wat niet lukt)
		transmitString(str);
		transmitString("\n"); //zodat de code doorheeft dat het getal ten einde is
		_delay_ms(1000); // om te testen
	}
}

