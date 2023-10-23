/*
 * timer2.c
 *
 * Author: Peter Sutton
 *
 * timer 2 skeleton
 */

#include "timer2.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set up timer 2 for SSD
 */
void init_timer2(void)
{
	
	TCNT2 = 0;
	/*
	//set up so that there is an interrupt every 10ms (100 times per second)
	OCR1A = 9999; //clock divided by 8 - count for 10000 cycles
	TCCR1A = 0; //CTC mode
	TCCR1B = (1<<WGM12) | (1<<CS11); // Divide clock by 8
	
	//enable interrupt on timer on output compare match
	TIMSK1 = (1<<OCIE1A);
	
	//Ensure interrupt flag is cleared
	TIFR1 = (1<<OCF1A);
	
	// Turn on global interrupts
	sei();
	*/
}
