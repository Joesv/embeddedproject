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

/*  EEPROM locaties */
#define checkBitLocatie 0x30 //byte
#define checkBitValue 0x11
#define tempLowerLocation 0x31
#define tempUpperLocation 0x33
#define lightLowerLocation 0x35
#define lightUpperLocation 0x37
#define schermLowerLocation 0x39 //byte ipv word
#define schermUpperLocation 0x04 //byte ipv word

#include "func/AVR_TTC_scheduler.c" //scheduler

#define VREF 5
#define UBBRVAL 51


#define temperature 2
#define light 3
#define type temperature //het type van deze unit

uint8_t isDown  = 0; //of het scherm naar beneden is of niet

int ledstate = 0;
int isChanging = 1; //Laat het lampje knipperen
uint16_t sensorVal;

uint8_t byteval;
uint16_t wordval;

uint16_t tempLower = 0x008b; //ongeveer 18 graden
uint16_t tempUpper = 0x0097; //ongeveer 25 gaden

uint16_t lightLower = 0x00CD; //205, ongeveer 20%
uint16_t lightUpper = 0x0300; //768, ongeveer 75%

uint8_t schermLower = 0x05; //5cm
uint8_t schermUpper = 0x96; //150cm

char lastCommand = 0;

/* UART */
void uart_init() {

	UBRR0H = (BRC >> 8);
	UBRR0L = (BRC);
	UCSR0A = 0; //status data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); //Zet TX en RX aan, en zet een Intterupt bit voor RX
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

/* Analog to Digital Converter */

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

/* uit de EEPROM lezen */
//https://www.nongnu.org/avr-libc/user-manual/group__avr__eeprom.html
uint8_t readByte(uint8_t location){
	return eeprom_read_byte((uint8_t*)location);
}

uint16_t readWord(uint8_t location){
	return eeprom_read_word((uint16_t*)location);
}

/* EEPROM schrijven */
void writeByte(uint8_t location, uint8_t data){
	if(readByte(location) != data){ //alleen aanpassen als het nog niet die waarde heeft
		eeprom_write_byte((uint8_t*) location, data);
	}
}

void writeWord(uint8_t location, uint16_t data){
	if(readWord(location != data)){ //alleen aanpassen als het nog niet die waarde heeft
		eeprom_write_word((uint16_t*) location, data);
	}
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


/* afhandelen van de waarden van de sensoren */
void handleVal(){
	if(type ==  temperature){
		if(sensorVal > tempUpper && !isDown){ // als het warmer is dan de grens en het scherm is niet naar beneden
			isDown = 1; //de stand waar hij in moet staan is 'down'
			isChanging = 1;
		}
		if(sensorVal < tempLower && isDown){ //als het te koud is
			isDown = 0; //de stand waar hij in moet staan is 'up'
			isChanging = 1;
		}
	}
	else if(type == light){
		if(sensorVal > lightUpper && !isDown){ // als het licht intenser is dan de grens en het scherm is niet naar beneden
			isDown = 1; //de stand waar hij in moet staan is 'down'
			isChanging = 1;
		}
		if(sensorVal < lightLower && isDown){ //als het te donker is
			isDown = 0; //de stand waar hij in moet staan is 'up'
			isChanging = 1;
		}
	}
}

void getVal(){
	uint16_t val;
	val = ReadADC(0); //analog A0;
	sensorVal = val;
	handleVal();
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

//https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
char* intToString(uint16_t value){
	int length = getIntLength(value); //hoeveel tekens nodig zijn om de int in het decimale stel weer te geven
	char* str = malloc(length +1); //ruimte reserveren voor de lengte + 1, \0 moet aan het einde van de string
	snprintf(str, length+1, "%d", value); //de waarde in de char array stoppen
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
		PORTB |= (1<<PORTB2);
		ledstate--;
	} else{ //anders uit
		PORTB &= ~(1<<PORTB2);
		ledstate++;
	}
}

void setLeds(uint8_t status){

	if(status == 1){ //status == isDown, in dit geval moet rood aan staan en groen uit.
		PORTB |= (1<<PORTB1); //rood gaat aan
		PORTB &= ~(1<<PORTB3); //groen gaat uit
	} else {
		PORTB &= ~(1<<PORTB1);//rood uit
		PORTB |= (1<<PORTB3); //groen aan
	}
}

void trigger(){
	PORTD |= (1<<PIND2); //trigger HIGH
	_delay_us(10); //10 ns wachten
	PORTD &= ~(1<<PIND2); //trigger LOW
}

uint16_t convertToCM(uint16_t counter){
	return (int) (counter * 60 / 58); //60 ns delay elke keer, 58 komt uit de datasheet
}
uint16_t measureDistance(){
	trigger();
	uint8_t notFinished = 1;
	uint16_t counter = 0;
	uint8_t pinVal;
	while(!(PIND & (1<<PIND3))){} //wachten tot de echo pin HIGH is
	//transmitString("high");

	while(notFinished){
		_delay_us(60); //uit de datasheet
		counter++;
		if(!(PIND & (1<<PIND3))){ //volgens de datasheet gaat de echo pin LOW wanneer het klaar is
			notFinished = 0;
		}
		if(counter>500){ //er is iets mis gegaan
			notFinished = 0;
		}
	}
	return convertToCM(counter);

}


void handleDistance(){
	if(isChanging == 1){ //als ie moet veranderen
		uint16_t distance = measureDistance();
		if(distance <= schermLower && isDown){ //als hij naar beneden gaat en de afstand kleiner of gelijk is aan de vereiste afstand en hij moet omlaag
			isChanging = 0;
		}
		if(distance >= schermUpper && !isDown){ //als hij omhoog moet zijn en de afstand groter of gelijk is dan het vereiste
			isChanging = 0;
		}
	}
}

void setupTasks(){
	if(type == temperature){//als het temp is
		SCH_Add_Task(getVal,100, 400); //tick staat op 10ms, na 1 sec meten en daarna elke 40 sec
		SCH_Add_Task(handleDistance, 105, 100);
		SCH_Add_Task(broadcastVal, 600, 1200); // na 1 minuut beginnen met 1 keer per minuut sturen;
	}
	if(type == light){//als het licht is
		SCH_Add_Task(getVal, 100, 300); //na 1 seconde meten en daarna elke 30sec
		SCH_Add_Task(handleDistance, 105, 100);
		SCH_Add_Task(broadcastVal, 600,1200); // na een minuut elke minuut sturen
	}

	SCH_Add_Task(Blinkie, 10, 50); //knipperend ledje
	SCH_Add_Task(startupBroadcast, 10,0); //broadcast eenmalig de gegevens wanneer er een serial verbinding is
}

void handleCommands(char inp){

	if(inp == '1'){
		if(!isDown){
			isDown = 1;
			isChanging = 1;
		}
	} else if(inp == '2'){
		if(isDown){
			isDown = 0;
			isChanging = 1;
		}
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
	DDRB |= 0x0F; //laagste 4 bits output voor de leds
	DDRD |= (1<<PIND2);

	setLeds(isDown);
	//initSettings();
	setupTasks();
	SCH_Start();
	while(1){
		//measureDistance();
		//_delay_ms(1000);
		SCH_Dispatch_Tasks();
		char c = getChar(); //Haalt karakter op van serial bus
		handleCommands(c);
	}

	
}
