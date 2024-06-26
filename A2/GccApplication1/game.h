/*
 * game.h
 *
 * Author: Jarrod Bennett, Cody Burnett
 *
 * Function prototypes for game functions available externally. You may wish
 * to add extra function prototypes here to make other functions available to
 * other files.
 */


#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>

#define TRACK_LENGTH 129


//functions
// Initialize the game by resetting the grid and beat
void initialise_game(void);

// Play a note in the given lane
void play_note(uint8_t lane);

// Advance the notes one row down the display
void advance_note(void);

//updates the LED matrix colors appropriately
void draw_notes();

void set_track(int track_number);

//global variables
// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void);

//number of points player has
int points;

//the current combo count that the player has
int combo_count;

//check if the combo is above or equal to 3
int combo_check;

//frequency of PWM in Hz
uint16_t freq;

uint16_t back_up_frequency;

//duty cycle of PWM in %
float duty_cycle;

//see if advance note is run 5 times or not
int advance_count;

//dis-ables the peizzo buzzer
int disable_piezzo;

//track number to be displayed
int track_no;

char track_name[25];

#endif
