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

// the 0x12 is the end of the long note of 0x10 and the start of a long note in lane 2


uint16_t beat;
uint8_t green_check = - 1; 
uint8_t game_over = 0;

void update_points() {
	//clear terminal line and reprint the game score with the points
	move_terminal_cursor(10, 13);
	clear_to_end_of_line();

	// if x >= 10 and [-9, 0)
	if ((points >= 10) || (points < 0 && points >= -9)) {
		printf("Game Score:   %d", points);
		
		//if x >= 100 and (-9, -99)
	} else if ((points >= 100) || (points < -9 && points > -99)) {
		printf("Game Score:  %d", points);
		
		//if x < -99
	} else if (points < -99) {
		printf("Game Score: %d", points);
		
	} else {
		printf("Game Score:    %d", points);
	}
}

void update_combo() {
	//Clear terminal line and reprint the combo count
	move_terminal_cursor(10, 14);
	clear_to_end_of_line();
	
	//adjusts the combo count padding so it matches points
	if (combo_count >= 10) {
		printf("Combo Count:  %d", combo_count);
		
	} else {
		printf("Combo Count:   %d", combo_count);
	}
	
	//update combo check accordingly
	if (combo_count >= 3) {
		combo_check = 1;
	} else {
		combo_check = 0;
	}
}


// Initialize the game by resetting the grid and beat
void initialise_game(void)
{
	// initialize the display we are using.
	default_grid();
	
	beat = 0;
	game_over = 0;
	update_points();
	update_combo();
}


// Play a note in the given lane
void play_note(uint8_t lane)
{	

	for (uint8_t col = 11; col < MATRIX_NUM_COLUMNS; col++) {
	
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col; //this is the next position that the note will be in
		uint8_t index = (future + beat) / 5;	//this is the note??
		
		if ((future + beat) % 5) {
			continue;
		}
		if (track[index] & (1<<lane))
		{	
			if (green_check == lane) {
				points--;
				update_points();
				
				combo_count = 0;
				update_combo();
				
				break;
			}
			
			green_check = lane;
			
			// if so, colour the two pixels green
			ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
			ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
			
			//if the note is in the two specified lanes then award the appropriate amount of points
			if (col == 11 || col == 15) {
				points++;
				combo_count = 0;
				
			} else if (col == 12 || col == 14) {
				points += 2;
				combo_count = 0;
				
			} else if (col == 13) {
				combo_count++;
				
				if (combo_count > 3) {
					points += 4;
				} else {
					points += 3;
					
				}
			}
			
		} else {
			points--;
			combo_count = 0;
		
		}
			
		update_points();
		update_combo();
		}	
}
	

// Advance the notes one row down the display
void advance_note(void)
{
	//if the user has a high combo score, set all notes to orange
	PixelColour dark_colour = COLOUR_DARK_RED;
	PixelColour color = COLOUR_RED;
	
	if (combo_check) {
		dark_colour = COLOUR_DARK_ORANGE;
		color = COLOUR_ORANGE;
	}
	
	
	// remove all the current notes; reverse of below
	for (uint8_t col=0; col<MATRIX_NUM_COLUMNS; col++)
	{
		uint8_t future = MATRIX_NUM_COLUMNS - 1 - col;
		uint8_t index = (future + beat) / 5;
		uint8_t ghost_start_index = ((MATRIX_NUM_COLUMNS - 1) + beat)/5;
		uint8_t ghost_index = ghost_start_index +1;
		
		//iterate over until the next valid future note is coming
		for (; ghost_index < TRACK_LENGTH; ghost_index++) {
			if (track[ghost_index] & 0x0F) {
				break;
			}
		}
		uint8_t ghost_note = track[ghost_index];
		
		if (index >= TRACK_LENGTH)
		{
			//Game Over
			game_over = 1;
			break;
		}
		if ((future+beat) % 5)
		{
			continue;
		}
		for (uint8_t lane = 0; lane < 4; lane++)
		{
			//check if the next note is in the appropriate lane
			if ((ghost_note == 0x01) & (lane == 0)) {
				ledmatrix_update_pixel(0, 2*lane, dark_colour);
				ledmatrix_update_pixel(0, 2*lane + 1, dark_colour);
				
			} else if ((ghost_note == 0x02) & (lane == 1)) {
				ledmatrix_update_pixel(0, 2*lane, dark_colour);
				ledmatrix_update_pixel(0, 2*lane + 1, dark_colour);
			
			} else if ((ghost_note == 0x04) & (lane == 2)) {
				ledmatrix_update_pixel(0, 2*lane, dark_colour);
				ledmatrix_update_pixel(0, 2*lane + 1, dark_colour);
			
			} else if ((ghost_note == 0x08) & (lane == 3)) {
				ledmatrix_update_pixel(0, 2*lane, dark_colour);
				ledmatrix_update_pixel(0, 2*lane + 1, dark_colour);
			
			}
				
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
		
		uint8_t ghost_index = ((MATRIX_NUM_COLUMNS - 1) + beat)/5;
		//next note in track that is coming
		uint8_t ghost_note = track[ghost_index];
		
		int long_check = 0;
		
		// if the index is beyond the end of the track,
		// no note can be drawn
		if (index >= TRACK_LENGTH || (index + 1) >= TRACK_LENGTH)
		{
			continue;
		}
				
		// iterate over the four paths
		for (uint8_t lane=0; lane<4; lane++)
		{									
			
			uint8_t current_note = track[index];
			
			/* check to see if in a long note logic
			//check if at the start of a long note
			if (current_note == current_note>>4) {
				long_check = 1;
			
			//check if in the middle of long note
			} else if ((long_check) && (track[index + 1] == current_note)) {
				continue; 
			//check if at the end of long note
			} else if ((long_check) && (track[index + 1] != track[index - 1])) {
				continue;
			
			//else outside long note.
			} else {
				long_check = 0;
				
			}
			
			if (long_check) {
				current_note = current_note>>4;
			}
			*/
			
			//check if there's a note in the specific path
			if (current_note & (1<<lane)) {
				
				//check if theres a note in the led matrix section
				if ((green_check == lane) & ((col >= 11) & (col <= 15))) {
					//if true set pixels to green
					ledmatrix_update_pixel(col, 2*lane, COLOUR_GREEN);
					ledmatrix_update_pixel(col, 2*lane+1, COLOUR_GREEN);
				
				} else if (ghost_note != current_note) {					
					ledmatrix_update_pixel(col, 2*lane, color);
					ledmatrix_update_pixel(col, 2*lane+1, color);
					
					//if note slides off screen and green_check isn't checked to a lane
					if (col == 15) {
						points--;
						update_points();		
						
						if (combo_count != 0) {
							combo_count = 0;
							update_combo();				
						}
					}
					
				} else {
					ledmatrix_update_pixel(col, 2*lane, color);
					ledmatrix_update_pixel(col, 2*lane+1, color);	
					
					//if note slides off screen and green_check isn't checked to a lane
					if (col == 15) {
						points--;
						update_points();		
						
						if (combo_count != 0) {
							combo_count = 0;
							update_combo();
						}			
					}
				} 
				
				//check if the current note goes off the screen
				if (col >= 15) {
					green_check = -1;
				}
				
								
			}
		}
	}
}

// Returns 1 if the game is over, 0 otherwise.
uint8_t is_game_over(void)
{
	// Detect if the game is over i.e. if a player has won.
	if (game_over) {	
		game_over = 0; //reset game over check
		return 1;	
	
	} else {
	return 0;
	
	}
}