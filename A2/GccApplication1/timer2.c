/*
 * timer2.c
 *
 * Author: Peter Sutton
 *
 * timer 2 skeleton
 */

#include "timer2.h"
#include "project.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set up timer 2 for SSD
 */
void init_timer2(void)
{
	
	//TCNT2 = 0;
	//set up so that there is an interrupt every 10ms (100 times per second)
	OCR2A = 77; //clock divided by 1024 - count for 10000 cycles
	TCCR2A = 0; //CTC mode
	TCCR2B = (1<<WGM22) | (1<<CS22) | (1<<CS20); // Divide clock by 1024(PRE) 
	
	//enable interrupt on timer on output compare match
	TIMSK2 = (1<<OCIE2A);
	 
	//Ensure interrupt flag is cleared
	TIFR2 = (1<<OCF2A);
	
	// Turn on global interrupts
	sei();
}

ISR(TIMER2_COMPA_vect) {
	ssd_display();
}
