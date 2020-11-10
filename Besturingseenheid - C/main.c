/*
 * hoofddocument
 * vergeet niet de taskscheduler bestanden in een map genaamd func te doen
 * 
 * Created: 11/7/2020 7:58:54 PM
 * Author : joes vos
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>

#define BAUD 19200 //maximum frequentie bits per seconde
#define BRC ((F_CPU/16/BAUD) -1) //Baud rate calculator = 16 bits
#define RX_BUFFER_SIZE 128

char rxBuffer[RX_BUFFER_SIZE]; //rxBuffer heeft lijst met 128 lijnen
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;
char getChar(void);
char peekChar(void);

//byte en word locaties in eeprom:
#define checkBitLocatie 0x30
#define checkBitValue 0x11
#define tempLowerLocation 0x31
#define tempUpperLocation 0x33
#define lightLowerLocation 0x35
#define lightUpperLocation 0x37

#include "func/AVR_TTC_scheduler.c"

#define VREF 5
#define UBBRVAL 51
#define temperature 2
#define light 3
#define type temperature 

#define isDown 0

int ledstate = 0;
int isChanging = 0; //Laat het lampje knipperen
uint16_t sensorVal;

uint8_t byteval;
uint16_t wordval;

uint16_t tempLower = 0x008b; //ongeveer 18 graden
uint16_t tempUpper = 0x0097; //ongeveer 25 gaden

uint16_t lightLower = 0x00CD; //205, ongeveer 20%
uint16_t lightUpper = 0x0300; //768, ongeveer 75%

void uart_init() {
	
	UBRR0H = (BRC >> 8); //High byte van UBBR0H / hierin staat de baudrate snelheid
	UBRR0L = (BRC);      //Lower byte van UBBR0H / hierin staat de baudrate snelheid
	UCSR0A = 0; //status data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); //Zet TX en RX aan, en zet een Intterupt bit voor RX
	                                                      //Wanneer er nieuwe data binnenkomt op de RX word een interrupt getriggert die word afgehandeld in de ISR(USART_RX_vect) methode
	UCSR0C = (1 << UCSZ01) |  (1 << UCSZ00); //8 bits
	sei(); //zet external interrupts aan
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

uint8_t readByte(uint8_t location){
	return eeprom_read_byte((uint8_t*)location);
}

uint16_t readWord(uint8_t location){
	return eeprom_read_word((uint16_t*)location);
}

void initSettings(){
	byteval = readByte(checkBitLocatie);
	if(byteval == checkBitValue){
		//lees de gegevens uit eeprom als de checkbit aangepast is. 
		lightLower = readWord(lightLowerLocation);
		lightUpper = readWord(lightUpperLocation);
		tempLower = readWord(tempLowerLocation);
		tempUpper = readWord(tempUpperLocation);
	}
}

void handleVal(){
	if(type ==  temperature){
		
	}
	else if(type == light){
		
	}
}

void getVal(){
	uint16_t val;
	val = ReadADC(0); //analog A0;
	sensorVal = val;
}

void transmitInfo(){
	if(type == temperature){//als het temp is
		transmitString("tmp,");
	}
	if(type == light){ // als het licht is
		transmitString("light,");
	}
}

void transmitStatus(){
	if(isDown){
		transmitString("down");
	} else {
		transmitString("up");
	}
	transmitString(",");
	if(isChanging){ //als ie omhoog of omlaag gaat
		transmitString("yes");
	} else {
		transmitString("no");
	}
}

char* intToString(uint16_t value){
	int length = getIntLength(value);
	char* str = malloc(length +1);
	snprintf(str, length+1, "%d", value);
	return str;
}

void broadcastVal(){
	transmitInfo();
	transmitString(intToString(sensorVal));
	transmitString(",");
	transmitStatus();
	transmitString("\n");
}

void transmitSettings(){
	if(type == temperature){
		transmitString(intToString(tempLower));
		transmitString(",");
		transmitString(intToString(tempUpper));
	} else if (type == light) {
		transmitString(intToString(lightLower));
		transmitString(",");
		transmitString(intToString(lightUpper));
	}
}

void startupBroadcast(){
	transmitString("info,");
	transmitInfo();
	transmit(",");
	transmitSettings();
	transmitString("\n");
}


void Blinkie(){
	if(ledstate && isChanging){ //als het de bedoeling is dat het ledje knippert en de staat is dat hij moet gaan knipperen, gaat hij aan
		PORTB |= (1<<PORTB1);
		ledstate--;
	} else{ //anders uit
		PORTB &= ~(1<<PORTB1);
		ledstate++;
	}
}

void setupTasks(){
	if(type == temperature){//als het temp is
		SCH_Add_Task(getVal,100, 400); //tick staat op 10ms, na 1 sec meten en daarna elke 40 sec
		SCH_Add_Task(broadcastVal, 600, 1200); // na 1 minuut beginnen met 1 keer per minuut sturen;
	}
	if(type == light){//als het licht is
		SCH_Add_Task(getVal, 100, 300); //na 1 seconde meten en daarna elke 30sec
		SCH_Add_Task(broadcastVal, 600,1200); // na een minuut elke minuut sturen
	}
	
	SCH_Add_Task(Blinkie, 10, 50); //knipperend ledje
	SCH_Add_Task(startupBroadcast, 10,0); //broadcast eenmalig de gegevens wanneer er een serial verbinding is
}

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

int main() {
	SCH_Init_T1(); //task initialeren
	uart_init(); // serial verbinding
	InitADC(); //analoog naar digitaal converter initialiseren
	DDRB = 0b00001111; //naar output zetten voor het ledje
	//initSettings();
	setupTasks(); 
	SCH_Start();
	while(1){
		SCH_Dispatch_Tasks();
		char c = getChar(); //Haalt karakter op van serial bus
		handleCommands(c);
	}

	
}
