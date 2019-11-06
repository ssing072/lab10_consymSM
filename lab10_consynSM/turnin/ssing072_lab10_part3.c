/*	Author: ssing072
 *  Partner(s) Name: Neha Gupta
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define button (~PINA & 0x04)

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;


void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}



unsigned char threeLeds = 0;
unsigned char blinkingLED = 0;

enum BL_States { BL_SMStart, BL_LedOff, BL_LedOn } BL_State;
void TickFct_BlinkLed() {
	switch(BL_State){
		case BL_SMStart:
		BL_State = BL_LedOff;
		break;
		case BL_LedOff:
		BL_State = BL_LedOn;
		break;
		case BL_LedOn:
		BL_State = BL_LedOff;
		break;
	}
	switch(BL_State){
		case BL_SMStart:
		blinkingLED = 0x00;
		break;
		case BL_LedOff:
		blinkingLED = 0x00;
		break;
		case BL_LedOn:
		blinkingLED = 0x08;
		break;
	}
}


enum TL_States { start, TL_T0, TL_T1, TL_T2 } TL_State;


void BL_threeLed(){
	switch(TL_State){
		case start:
		TL_State = TL_T0;
		break;
		case TL_T0:
		TL_State = TL_T1;
		break;
		case TL_T1:
		TL_State = TL_T2;
		break;
		case TL_T2:
		TL_State = TL_T0;
		break;
	}
	switch(TL_State){
		case start:
		threeLeds = 0x00;
		break;
		case TL_T0:
		threeLeds = 0x01;
		break;
		case TL_T1:
		threeLeds = 0x02;
		break;
		case TL_T2:
		threeLeds = 0x04;
		break;
	}
}


enum speaker{SP_start, speakON, speakOff} SP_state;
unsigned char speakerSound = 0;

void play_Speaker(){
	switch(SP_state){
		case SP_start:
		SP_state = speakON;
		break;
		case speakON:
		SP_state = speakOff;
		break;
		case speakOff:
		SP_state = speakON;
		break;
		
	}
	switch(SP_state){
		case SP_start:
		break;
		case speakON:
		speakerSound = 0x10;
		break;
		case speakOff:
		speakerSound = 0x00;
		break;
	}
}

const unsigned long TL_Period = 300;
const unsigned long BL_Period = 1000;
unsigned long SP_period = 1;
unsigned long i = 0;
unsigned long j = 0;
unsigned long k = 0;

enum states {CS_INIT, callALL} CS_state;
void combostates(){
	switch(CS_state){
		case CS_INIT:
		CS_state = callALL;
		break;
		case callALL:
		CS_state = callALL;
		break;
	}
	switch(CS_state){
		case CS_INIT:
		break;
		case callALL:
		if(i >= 300){
			BL_threeLed();
			i = 0;
		}
		i++;
		if(j >= 1000){
			TickFct_BlinkLed();
			j = 0;
		}
		j++;
		if(button){
			if(k >= SP_period){
				play_Speaker();
				k = 0;
			}
			k++;
		}
		PORTB = (speakerSound | threeLeds | blinkingLED);
		break;
	}
}


int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1);
	TimerOn();
	i = 0;
	j = 0;
	k = 0;
	BL_State  = BL_SMStart;
	TL_State =  start;
	CS_state = CS_INIT;
	SP_state = SP_start;
	while (1) {
		combostates();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 1;
}
