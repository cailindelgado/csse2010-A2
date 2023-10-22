/*
 * timer1.c
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set up timer 1
 */
void init_timer1(void)
{
	TCNT1 = 0;
}

/*
void init_timer1(void)
{
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
}


ISR(TIMER1_COMPA_vect) {
	//Display the number to the SSD
	//change displayed digit, 
	seven_seg_cc = 1 ^ seven_seg_cc;
	
	if (points == 0) {
		//Display 0 on the ssd
		PORTC = seven_seg_data[0]
	}
	
}
*/
