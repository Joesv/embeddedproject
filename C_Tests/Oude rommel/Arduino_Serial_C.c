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
#define RX_BUFFER_SIZE 128

char rxBuffer[RX_BUFFER_SIZE]; //rxBuffer heeft lijst met 128 lijnen
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

char getChar(void);
char peekChar(void);

void handleCommands(char inp){
	if(inp == '1'){
		PORTB = 0x00;
		PORTB ^= 0b00000001; //Zet LED1 aan
		} else if(inp == '2'){
		PORTB = 0x00;
		PORTB = 0b00000010; //Zet LED2 aan
		} else if (inp == '3'){
		PORTB = 0x00;
		PORTB ^= 0b00000100; //Zet LED3 aan
		} else if(inp == '4'){
		PORTB = 0x00;
		PORTB ^= 0b00001000; //Zet LED4 aan
		} else if (inp == '0'){
		PORTB = 0x00; //Zet alle LED's uit
	}
}

int main(void)
{
    UBRR0H = (BRC >> 8); //High byte shift 8 plaatsen om de hoogste 8 bits te pakken en plaatst deze in baudrate registerH
    UBRR0L = (BRC);      //Lower byte is BRC, de laagste 8 bits en plaatst deze in baudrate registerL
	
	UCSR0B = (1 << RXEN0) | (1 << RXCIE0); //"UCSR0B = USART Control and Status Register B" //Zet de receiver aan = "RX op arduino", en set bit for interrupt enable, elke nieuwe data die binnenkomt triggert de interrupt
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Zet de character size die de receiver kan gebruiken // 8 bits
	
	DDRB = 0b00001111; //Zet DDRB port 8, 9, 10 en 11 op output
	sei(); //zet external interrupts aan
	
	
    while (1) 
    {
		char c = getChar(); //Haalt karakter op van serial bus
		handleCommands(c);
    }
}

char peekChar(void){ //Returnt het eerst volgende karakter dat gelezen gaat worden
	char ret = '\0';
	
	if(rxReadPos != rxWritePos){
		ret = rxBuffer[rxReadPos];
	}
	return ret;
}

char getChar(void){ //Haalt karakter op dat moet worden gelezen en increment de leespositie
	char ret = '\0';
	
	if(rxReadPos != rxWritePos){ //Als er nieuwe data is maar de readposition staat achter op de 
		ret = rxBuffer[rxReadPos];
		rxReadPos++;
		
		if(rxReadPos >= RX_BUFFER_SIZE){
			rxReadPos = 0;
		}
		return ret;
	}
}
ISR(USART_RX_vect){  //De interrupt
	
	rxBuffer[rxWritePos] = UDR0; //UDR0 is wat er in gelezen word vanaf serial
	rxWritePos++;
	
	if(rxWritePos >= RX_BUFFER_SIZE){ // Als de buffer van 128 vol is
		rxWritePos = 0; //rxWritePos is 0
	}
}
