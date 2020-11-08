/*
 * blinkie met ttc
 *
 * Created: 11/7/2020 7:58:54 PM
 * Author : joes vos
 */ 

#include <avr/io.h>

#include "func/AVR_TTC_scheduler.c"


int state = 0;

void Blinkie(){
	if(state){
		PORTB |= (1<<PORTB1);
		state--;
		} else{
		PORTB &= ~(1<<PORTB1);
		state++;
	}
}


int main() {
	SCH_Init_T1();
	DDRB = (1<<DDB1);
	SCH_Add_Task(Blinkie,10,50);
	SCH_Start();
	while(1){
		SCH_Dispatch_Tasks();
	}

	
}

