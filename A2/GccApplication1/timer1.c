/*
 * timer1.c
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#include "timer1.h"
#include "game.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set up timer 1 For buzzer
 */


void init_timer1(void)
{
	freq = 1;
	duty_cycle = 0;
	
	uint16_t clockperiod = (1000000UL / freq);
	uint16_t pulsewidth = (duty_cycle * clockperiod)/100;
	
	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
	
	// Set the count compare value based on the pulse width. The value will be 1 less
	// than the pulse width - unless the pulse width is 0.
	if(pulsewidth == 0) {
		OCR1B = 0;
		
		} else {
		OCR1B = pulsewidth - 1;
	}
	
	//set up timer 1 for;
	//Fast PWM, counting from 0 to the value in OCR1A
	//before reseting to 0. Count at 1MHz (CLK/8).
	//Configure output OC1B to be CTC non-inverting mode.
	TCCR1A = (1 << COM1B1) | (1 << WGM10) | (1 <<WGM11);
	TCCR1B = (1 << CS11) | (1 << WGM12) |(1 << WGM13);
	
}

void note_sound() {
	
	if (!disable_piezzo) { //if you dont want to turn off the piezzo do the following
		//turn on PORTd4 and set DDRD4 to be output
		DDRD |= 0b00010000;
		PORTD |= 0b00010000;
		
		uint16_t clockperiod = (1000000UL / freq);
		uint16_t pulsewidth = (duty_cycle * clockperiod)/100;
	
		OCR1A = clockperiod - 1;
		OCR1B = pulsewidth - 1;
		
	} else { //if you want to turn off the piezzo do the following
		//set bot checks to be 0
		OCR1A = 0;
		OCR1B = 0;
		
		//make DDRD4 to be input and PORTD4 to be off
		DDRD &= ~(1<<4);	//inverse of 1<<4
		PORTD &= 0b11101111;
			
	}
}

void pause_control(int is_paused) {
	if (is_paused) {
		OCR1A = 0;
		OCR1B = 0;
		
		DDRD &= ~(1<<4);	//inverse of 1<<4
		PORTD &= 0b11101111;
		
	} else {
		note_sound();
		/*
		DDRD |= 0b00010000;
		PORTD |= 0b00010000;
		
		uint16_t clockperiod = (1000000UL / freq);
		uint16_t pulsewidth = (duty_cycle * clockperiod)/100;
		
		OCR1A = clockperiod - 1;
		OCR1B = pulsewidth - 1;
		*/
	}
	
}