/*
 * timer1.h
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "game.h"
#include <stdint.h>

/* Set up our timer 
 */
void init_timer1(void);

//update appropriate bits and play sound
void note_sound(void);

//pause control function
void pause_control(int is_paused);

#endif /* TIMER1_H_ */
