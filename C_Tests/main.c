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
#define F_CPU 16E6
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>
#include <avr/eeprom.h>

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
int isChanging = 0;
uint16_t sensorVal;

uint8_t byteval;
uint16_t wordval;

uint16_t tempLower = 0x008b; //ongeveer 18 graden
uint16_t tempUpper = 0x0097; //ongeveer 25 gaden

uint16_t lightLower = 0x00CD; //205, ongeveer 20%
uint16_t lightUpper = 0x0300; //768, ongeveer 75%

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




int main() {
	SCH_Init_T1(); //task initialeren
	uart_init(); // serial verbinding
	InitADC(); //analoog naar digitaal converter initialiseren
	DDRB = (1<<DDB1); //naar output zetten voor het ledje
	//initSettings();
	setupTasks(); 
	SCH_Start();
	while(1){
		SCH_Dispatch_Tasks();
	}

	
}
