//https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html
#include <avr/eeprom.h>
#define checkBitLocation 50
#define checkBit 0x11
#define defaultLower 0x008b //ongeveer 18 graden celsius 
#define defaultUpper 0x0097 //ongeveer 25 graden celsius

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
void transmitChar(unsigned char data){
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = data;
}
// https://www.avrfreaks.net/forum/help-transmitting-strings-usart
void transmitString(char *str){
	while(*str){
		transmitChar(*str++);
	}
}
//https://stackoverflow.com/a/32819876
int getIntLength(int num){
	return snprintf(NULL, 0, "%d", num);
}


int main(){
	uint8_t ByteData;
	uint16_t upper;
	uint16_t lower;
	uart_init();
	transmitString("hello \n");
	//lezen wat er op de locatie staat, als er niks is aangepast het is 0xff, anders 0x11
	ByteData = eeprom_read_byte((uint8_t*)checkBitLocation); 
	if(ByteData == 0x11){
		//checkbit klopt, lees de opgeslagen waarden
		upper = eeprom_read_word((uint16_t*)(checkBitLocation+1));
		lower = eeprom_read_word((uint16_t*)(checkBitLocation+3));
	} else {
		//schrijf de checkbit en de waarden weg
		eeprom_write_byte((uint8_t*)50, 0x11); //schrijf 0x11 weg naar locatie 50 (0x32)
		eeprom_write_word((uint16_t*)(checkBitLocation+1), defaultUpper);
		eeprom_write_word((uint16_t*)(checkBitLocation+3), defaultLower);
		upper = defaultUpper;
		lower = defaultLower;
	}
	uint16_t val;
	int length;
	while(1)
	{
		transmitString("hello \n");
		
		
		val = eeprom_read_word((uint16_t*)(checkBitLocation+1));
		length = getIntLength(val);
		char* str = malloc(length+1);
		snprintf(str, length+1, "%d", val); 
		transmitString(str);
		transmitString("\n");
		_delay_ms(1000);		
	}



};
