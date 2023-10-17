/*
 * game.c
 *
 * Functionality related to the game state and features.
 *
 * Author: Jarrod Bennett, Cody Burnett
 */ 

#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "display.h"
#include "ledmatrix.h"
#include "terminalio.h"

static const uint8_t track[TRACK_LENGTH] = {0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x80, 0x00, 0x00, 0x04, 0x02,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x08, 0x08, 0x08, 0x80, 0x04, 0x02,
	0x04, 0x40, 0x02, 0x08, 0x80, 0x00, 0x02, 0x01,
	0x04, 0x40, 0x08, 0x80, 0x04, 0x02, 0x20, 0x01,
	0x10, 0x10, 0x12, 0x20, 0x00, 0x00, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x04, 0x40, 0x08, 0x04, 0x40, 0x40, 0x02, 0x20,
	0x01, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00};

uint16_t beat;
uint8_t green_check = 4; // 4 is a place holder value which will be changed when the buttons are first pressed

// Initialize the game by resetting the grid and beat
void initialise_game(void)
{
	// initialize the display we are using.
	default_grid();
	beat = 0;
}

// Play a note in the given lane
void play_note(uint8_t lane) // button lane, normally aligned
{
	// YOUR CODE HERE
	// possible steps:
	// a) check if there is a note in the scoring area of the given lane -
	//    look at advance_note below for a hint to the logic
	// b) if there is, immediately turn those notes green
	// c) set up a variable to remember if notes should be green,
	//    and trigger it simultaneously with b)
	// d) in advance_note below, poll that variable, and choose COLOUR_GREEN
	//    instead of COLOUR_RED for ledmatrix_update_pixel if required
	// e) depending on your implementation, clear the variable in
	//    advance_note when a note disappears from the screen

	green_check = lane;
		
	for (uint8_t col = 11; col < MATRIX_NUM_COLUMNS; col++) {
	
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col; //this is the next position that the note will be in
		uint8_t index = (future + beat) / 5;	//this is the note??
		
		if ((future+beat) % 5) {
			continue;
		}
		if (track[index] & (1<<lane))
		{
			// if so, colour the two pixels green
			ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
			ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
		}
	}
}

// Advance the notes one row down the display
void advance_note(void)
{
	// remove all the current notes; reverse of below
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		if (index >= TRACK_LENGTH)
		{
			break;
		}
		if ((future+beat) % 5)
		{
			continue;
		}
		for (uint8_t lane = 0; lane < 4; lane++)
		{
			if (track[index] & (1<<lane))
			{
				PixelColour colour;
				// yellows in the scoring area
				if (col==11 || col == 15)
				{
					colour = COLOUR_QUART_YELLOW;
				}
				else if (col==12 || col == 14)
				{
					colour = COLOUR_HALF_YELLOW;
				}
				else if (col==13)
				{
					colour = COLOUR_YELLOW;
				}
				else
				{
					colour = COLOUR_BLACK;
				}
				ledmatrix_update_pixel(col, 2*lane, colour);
				ledmatrix_update_pixel(col, 2*lane+1, colour);
			}
		}
	}
	
	// increment the beat
	beat++;
	
	// draw the new notes
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		// col counts from one end, future from the other
		uint8_t future = MATRIX_NUM_COLUMNS-1-col;
		// notes are only drawn every five columns
		if ((future+beat)%5)
		{
			continue;
		}
		
		// index of which note in the track to play
		uint8_t index = (future+beat)/5;
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH)
		{
			continue;
		}
		// iterate over the four paths
		for (uint8_t lane=0; lane<4; lane++)
		{
			// check if there's a note in the specific path	
			if (track[index] & (1<<lane)) {
				// if so, colour the two pixels red
				ledmatrix_update_pixel(col, 2*lane, COLOUR_RED);
				ledmatrix_update_pixel(col, 2*lane+1, COLOUR_RED);
			}
		}
	}
}

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void)
{
	// YOUR CODE HERE
	// Detect if the game is over i.e. if a player has won.
	return 0;
}