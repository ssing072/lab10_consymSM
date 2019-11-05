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

void set_PWM(double frequency){
	static double current_frequency;
	if(frequency != current_frequency){
		if(!frequency){ TCCR3B &= 0x08; }
		else{ TCCR3B |= 0x03; }
		if(frequency < .954){ OCR3A = 0xFFFF;}
		else if(frequency > 31250) { OCR3A = 0x0000; }
		else { OCR3A = (short)(8000000 / (128* frequency)) -1; }
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on(){
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off(){
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

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

unsigned long i = 0;
unsigned long j = 0;

enum states {CS_INIT, callBoth} CS_state;
void combostates(){
	switch(CS_state){
		case CS_INIT:
		CS_state = callBoth;
		break;
		case callBoth:
		CS_state = callBoth;
		break;
	}
	switch(CS_state){
		case CS_INIT:
		break;
		case callBoth:
		if(i == 150){
			BL_threeLed();
			i = 0;
		}
		i++;
		if(j == 500){
			TickFct_BlinkLed();
			j = 0;
		}
		j++;
		if(button){
			set_PWM(261.63);
		}
		else{
			set_PWM(0);
		}
		PORTB = (threeLeds | blinkingLED);
		break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(2);
	TimerOn();
	i = 0;
	j = 0;
	BL_State  = BL_SMStart;
	TL_State =  start;
	CS_state = CS_INIT;
	PWM_on();
	while (1) {
		combostates();
		while(!TimerFlag){}
		TimerFlag = 0;
	}
	return 1;
}
