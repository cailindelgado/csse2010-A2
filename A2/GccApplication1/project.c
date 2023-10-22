/*
 * project.c
 *
 * Main file
 *
 * Authors: Peter Sutton, Luke Kamols, Jarrod Bennett, Cody Burnett
 * Modified by <YOUR NAME HERE>
 */

#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 8000000UL
#include <util/delay.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

uint16_t game_speed;

//is manual on?	
int man_mode = 0;

//is the game paused?
int paused = 0;
	
/////////////////////////////// main //////////////////////////////////
int main(void)
{
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	points = 0;
	
	// Show the splash screen message. Returns when display
	// is complete.
	start_screen();
	
	// Loop forever and continuously play the game.
	while(1)
	{
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void)
{
	ledmatrix_setup();
	init_button_interrupts();
	
	//Make all bits of port C and the upper 5 bits of port D to be output bits
	DDRC = 0xFF; //0xFF => 0b11111111
	DDRD = 0xFC; //0xFC => 0b11111100																						//is this valid??
	
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200, 0);
	
	init_timer0();
	init_timer1();
	init_timer2();
	
	// Turn on global interrupts
	sei();
}

void start_screen(void)
{
	// Clear terminal screen and output a message
	clear_terminal();
	show_cursor();
	clear_terminal();
	hide_cursor();
	set_display_attribute(FG_WHITE);
	move_terminal_cursor(10,4);
	printf_P(PSTR("  ______   __     __  _______         __    __"));
	move_terminal_cursor(10,5);
	printf_P(PSTR(" /      \\ |  \\   |  \\|       \\       |  \\  |  \\"));
	move_terminal_cursor(10,6);
	printf_P(PSTR("|  $$$$$$\\| $$   | $$| $$$$$$$\\      | $$  | $$  ______    ______    ______"));
	move_terminal_cursor(10,7);
	printf_P(PSTR("| $$__| $$| $$   | $$| $$__| $$      | $$__| $$ /      \\  /      \\  /      \\"));
	move_terminal_cursor(10,8);
	printf_P(PSTR("| $$    $$ \\$$\\ /  $$| $$    $$      | $$    $$|  $$$$$$\\|  $$$$$$\\|  $$$$$$\\"));
	move_terminal_cursor(10,9);
	printf_P(PSTR("| $$$$$$$$  \\$$\\  $$ | $$$$$$$\\      | $$$$$$$$| $$    $$| $$   \\$$| $$  | $$"));
	move_terminal_cursor(10,10);
	printf_P(PSTR("| $$  | $$   \\$$ $$  | $$  | $$      | $$  | $$| $$$$$$$$| $$      | $$__/ $$"));
	move_terminal_cursor(10,11);
	printf_P(PSTR("| $$  | $$    \\$$$   | $$  | $$      | $$  | $$ \\$$     \\| $$       \\$$    $$"));
	move_terminal_cursor(10,12);
	printf_P(PSTR(" \\$$   \\$$     \\$     \\$$   \\$$       \\$$   \\$$  \\$$$$$$$ \\$$        \\$$$$$$"));
	move_terminal_cursor(10,14);
	// change this to your name and student number; remove the chevrons <>
	printf_P(PSTR("CSSE2010/7201 A2 by  Cailin Delgado - 48027942"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	show_start_screen();
	
	//Display current track
	move_terminal_cursor(10, 15);
	printf("Track: Through Fire & Flames");															//do %s and add a new track title at the end.

	uint32_t last_screen_update, current_time;
	last_screen_update = get_current_time();
	
	uint8_t frame_number = 0;
	game_speed = 1000;
	
	
	move_terminal_cursor(10, 17);
	clear_to_end_of_line();
	//update game speed in terminal appropriately
	if (game_speed == 1000) {
		printf("Current Game Speed: 1 (Normal: %d ms)", game_speed);
		
	} else if (game_speed == 500) {
		printf("Current Game Speed: 2 (Fast: %d ms)", game_speed);
		
	} else if (game_speed == 250) {
		printf("Current Game Speed: 3 (Extreme: %d ms)", game_speed);
	}
	

	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1)
	{
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available())
		{
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S')
		{
			break;
		} 
		
		
		//display the game speed to the terminal
		if (serial_input == '1') {
			game_speed = 1000; //set game speed to 1000ms
			
			move_terminal_cursor(10, 17);
			clear_to_end_of_line();
			printf("Current Game Speed: 1 (Normal: %d ms)", game_speed);
			
		} else if (serial_input == '2') {
			game_speed = 500; //set game speed to 500ms
			
			move_terminal_cursor(10, 17);
			clear_to_end_of_line();
			printf("Current Game Speed: 2 (Fast: %d ms)", game_speed);
			
		} else if (serial_input == '3') {
			game_speed = 250; //set game speed to 250ms
			
			move_terminal_cursor(10, 17);
			clear_to_end_of_line();
			printf("Current Game Speed: 3 (Extreme: %d ms)", game_speed);
		}
		
		
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED)
		{
			break;
			
		} else if (serial_input == 'm' || serial_input == 'M') {
			//clear manual mode alert
			move_terminal_cursor(10, 16);
			clear_to_end_of_line();
			
			if (man_mode) {
				man_mode = 0;  // toggle manual mode checker
				
			} else {
				man_mode = 1; //toggle manual mode checker				
				//print to terminal that manual mode is on
				printf("Manual Mode: ON");
			}
		} 		

		if (!man_mode) {
			// every 200 ms, update the animation
			current_time = get_current_time();
			if (current_time - last_screen_update > game_speed/5)
			{
				update_start_screen(frame_number);
				frame_number = (frame_number + 1) % 32;
				last_screen_update = current_time;
			} else {
				if (serial_input == 'n' || serial_input == 'N') {
					advance_note();
				}
			}
		}
	}
}

void display_countdown(int countdown) {
	
	//clear display
	ledmatrix_clear();
	
	switch (countdown) {
		case 1: //Then draw the 1
			for (uint8_t col = 4; col < 11; col++) {
				ledmatrix_update_pixel(col, 3, COLOUR_RED);
				ledmatrix_update_pixel(col, 4, COLOUR_RED);
				
				if (col == 5) {
					ledmatrix_update_pixel(col, 2, COLOUR_RED);
					ledmatrix_update_pixel(col, 4, COLOUR_RED);
					
					} else if (col == 10) {
					ledmatrix_update_pixel(col, 2, COLOUR_RED);
					ledmatrix_update_pixel(col, 5, COLOUR_RED);
					
				}
			}
			break;
			
		case 2:  //Then draw the 2
			ledmatrix_update_pixel(4, 2, COLOUR_RED);
			ledmatrix_update_pixel(4, 3, COLOUR_RED);
			ledmatrix_update_pixel(4, 4, COLOUR_RED);
			ledmatrix_update_pixel(4, 5, COLOUR_RED);
			
			ledmatrix_update_pixel(5, 1, COLOUR_RED);
			ledmatrix_update_pixel(5, 2, COLOUR_RED);
			ledmatrix_update_pixel(5, 5, COLOUR_RED);
			ledmatrix_update_pixel(5, 6, COLOUR_RED);
			
			ledmatrix_update_pixel(6, 5, COLOUR_RED);
			ledmatrix_update_pixel(6, 6, COLOUR_RED);
			
			ledmatrix_update_pixel(7, 5, COLOUR_RED);
			ledmatrix_update_pixel(7, 6, COLOUR_RED);
			
			ledmatrix_update_pixel(8, 2, COLOUR_RED);
			ledmatrix_update_pixel(8, 3, COLOUR_RED);
			ledmatrix_update_pixel(8, 4, COLOUR_RED);
			
			ledmatrix_update_pixel(9, 1, COLOUR_RED);
			ledmatrix_update_pixel(9, 2, COLOUR_RED);
			
			ledmatrix_update_pixel(10, 1, COLOUR_RED);
			ledmatrix_update_pixel(10, 2, COLOUR_RED);
			ledmatrix_update_pixel(10, 3, COLOUR_RED);
			ledmatrix_update_pixel(10, 4, COLOUR_RED);
			ledmatrix_update_pixel(10, 5, COLOUR_RED);
			ledmatrix_update_pixel(10, 6, COLOUR_RED);
			break;
			
		case 3: //Then draw the 3
			for (uint8_t col = 4; col < 11; col ++) {
				if (col == 4 || col == 10) {
					ledmatrix_update_pixel(col, 2, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 3, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 4, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 5, COLOUR_YELLOW);
					
					} else if (col == 5 || col == 9) {
					ledmatrix_update_pixel(col, 1, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 2, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 5, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 6, COLOUR_YELLOW);
					
					} else if (col == 6 || col == 8) {
					ledmatrix_update_pixel(col, 5, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 6, COLOUR_YELLOW);
					
					} else {
					ledmatrix_update_pixel(col, 2, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 3, COLOUR_YELLOW);
					ledmatrix_update_pixel(col, 4, COLOUR_YELLOW);
				}
			}
			break;
		
		case 4: //Then draw the Go
			ledmatrix_update_pixel(5, 1, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 2, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 3, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 5, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 6, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 7, COLOUR_GREEN);
			
			ledmatrix_update_pixel(6, 0, COLOUR_GREEN);
			ledmatrix_update_pixel(6, 5, COLOUR_GREEN);
			ledmatrix_update_pixel(6, 7, COLOUR_GREEN);
			
			ledmatrix_update_pixel(7, 0, COLOUR_GREEN);
			ledmatrix_update_pixel(7, 2, COLOUR_GREEN);
			ledmatrix_update_pixel(7, 3, COLOUR_GREEN);
			ledmatrix_update_pixel(7, 5, COLOUR_GREEN);
			ledmatrix_update_pixel(7, 7, COLOUR_GREEN);
			
			ledmatrix_update_pixel(8, 0, COLOUR_GREEN);
			ledmatrix_update_pixel(8, 3, COLOUR_GREEN);
			ledmatrix_update_pixel(8, 5, COLOUR_GREEN);
			ledmatrix_update_pixel(8, 7, COLOUR_GREEN);
			
			ledmatrix_update_pixel(9, 1, COLOUR_GREEN);
			ledmatrix_update_pixel(9, 2, COLOUR_GREEN);
			ledmatrix_update_pixel(5, 3, COLOUR_GREEN);
			ledmatrix_update_pixel(9, 5, COLOUR_GREEN);
			ledmatrix_update_pixel(9, 6, COLOUR_GREEN);
			ledmatrix_update_pixel(9, 7, COLOUR_GREEN);
		
		default:
			break;
	}
	
}

void game_countdown() {
	
	uint32_t current_time = get_current_time();
	uint32_t last_recorded_time = current_time;

	display_countdown(1); // display 1
	
	while (1) {
		//update current time
		current_time = get_current_time();
		
		//if the change in time between the 1 being drawn hits 2 * game speed
		if ((current_time - last_recorded_time) >= (2 * game_speed)) {
			display_countdown(2); //display 2
			last_recorded_time = current_time;
			break;
		}
	}
	
	while (1) {
		//update current time
		current_time = get_current_time();
		
		//if the change in time between the 1 being drawn hits 2 * game speed
		if ((current_time - last_recorded_time) >= (2 * game_speed)) {
			display_countdown(3); //display 3
			
			last_recorded_time = current_time;
			break;
		}
	}
	
	while (1) {
		//update current time
		current_time = get_current_time();
		
		//if the change in time between the 1 being drawn hits 2 * game speed
		if ((current_time - last_recorded_time) >= (2 * game_speed)) {
			display_countdown(4); //display go
			
			last_recorded_time = current_time;
			break;
		}
	}
	
																														//turn into a loop
	while (1) {
		//update current time
		current_time = get_current_time();
		
		//if the change in time between the 1 being drawn hits 2 * game speed
		if ((current_time - last_recorded_time) >= (2 * game_speed)) {
			break;
		}
	}
}

void new_game(void)
{
	// Clear the serial terminal
	clear_terminal();
	
	//reset number of points player has
	points = 0;
	
	//Start Game Countdown
	game_countdown();
		
	// Initialize the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void)
{
	
	uint32_t last_advance_time, current_time;
	
	int8_t btn; // The button pushed
	
	//int combo_line = 0;
	
	last_advance_time = get_current_time();
	
	//Display current track
	move_terminal_cursor(10, 15);
	printf("Track: Through Fire & Flames");																//do %s and add a new track title at the end.
	
	move_terminal_cursor(10, 18);
	clear_to_end_of_line();
	//update terminal line appropriately
	if (game_speed == 1000) {
		printf("Current Game Speed: 1 (Normal: %d ms)", game_speed);
		
	} else if (game_speed == 500) {
		printf("Current Game Speed: 2 (Fast: %d ms)", game_speed);
		
	} else if (game_speed == 250) {
		printf("Current Game Speed: 3 (Extreme: %d ms)", game_speed);
	}
	
	// We play the game until it's over
	while (!is_game_over())
	{
				
		// We need to check if any button has been pushed, this will be
		// NO_BUTTON_PUSHED if no button has been pushed
		// Checkout the function comment in `buttons.h` and the implementation
		// in `buttons.c`.
		btn = button_pushed();
		
		char keyboard_input = -1;
		
		if (serial_input_available()) {
			keyboard_input = fgetc(stdin);
		}
		
		if ((btn == BUTTON0_PUSHED) || (keyboard_input == 'f' || keyboard_input == 'F')) {
			// If button 0 play the lowest note (right lane)
			play_note(3);
		
		} else if ((btn == BUTTON1_PUSHED) || (keyboard_input == 'd' || keyboard_input == 'D')) {
			//If button 1 is pushed play the second lowest note
			play_note(2);		
		
		} else if ((btn == BUTTON2_PUSHED) || (keyboard_input == 's' || keyboard_input == 'S')) {
			//If button 2 is pushed play the second highest note
			play_note(1); 
			
		} else if ((btn == BUTTON3_PUSHED) || (keyboard_input == 'a' || keyboard_input == 'A')) {
			//If button 3 is pushed play the highest note
			play_note(0);
			
		} 
		
		if (keyboard_input == 'm' || keyboard_input == 'M') {
			//check to see if the game is currently paused
			if (!paused) {
				//clear manual mode alert
				move_terminal_cursor(10, 16);
				clear_to_end_of_line();
				if (man_mode) {
					man_mode = 0;
		
				} else {
					man_mode = 1;
					
					//print to terminal that manual mode is on
					printf("Manual Mode: ON");
				}
			}	
		}
		
		if (keyboard_input == 'p' || keyboard_input == 'P') {
			//Check to see if the game is in manual mode
			if (!man_mode) {
				//clear pause line in terminal
				move_terminal_cursor(10, 17);
				clear_to_end_of_line();
				if (paused) {
					paused = 0;
					PORTD = PORTD & 0b11110111;
			
				} else {
					paused = 1;
					PORTD = PORTD | (1<<3); //essentially just PORTD | 0b00001000
					
					//tell user that game is currently paused
					printf("Game Paused");
				}
				
			}
		}
		
		//Combo IO board LED's
		if (combo_count == 0) {
			//set portD outputs to the I/O boards LED matrix to be 0
			PORTD = PORTD & 0b10001111;

		} else if (combo_count == 1) {
			//set appropriate led connection to be high
			PORTD = PORTD | (1<<4); //essentially just PORTD | 0b00010000
			
		} else if (combo_count == 2) {
			//set appropriate led connection to be high
			PORTD = PORTD | (1<<5);
			
		} else if (combo_count >= 3) {
			//set appropriate led connection to be high
			PORTD = PORTD | (1<<6);
		}
		
		/*
		//Combo display																			//WHY DOES IT SLOW DOWN THE 'n' feature of manual mode.
		if ((combo_check) && (combo_line == 0)) {
			move_terminal_cursor(10, 3);
			printf("  ______                           __                  __");
			
			combo_line++; //increment combo line so in next loop the next line is printed of ascii art
		} else if (combo_line == 1) {
			move_terminal_cursor(10,4);
			printf(" /      \\                         |  \\                |  \\");
			
			combo_line++;
		} else if (combo_line == 2) {
			move_terminal_cursor(10,5);
			printf("|  $$$$$$\\  ______   ______ ____  | $$____    ______  | $$");
			
			combo_line++;
		} else if (combo_line == 3) {
			move_terminal_cursor(10,6);
			printf("| $$   \\$$ /      \\ |      \\    \\ | $$    \\  /      \\ | $$");
			
			combo_line++;
		} else if (combo_line == 4) {
			move_terminal_cursor(10,7);
			printf("| $$      |  $$$$$$\\| $$$$$$\\$$$$\\| $$$$$$$\\|  $$$$$$\\| $$");
			
			combo_line++;
		}  else if (combo_line == 5) {
			move_terminal_cursor(10,8);
			printf("| $$   __ | $$  | $$| $$ | $$ | $$| $$  | $$| $$  | $$ \\$$");
			
			combo_line++;
		} else if (combo_line == 6) {
			move_terminal_cursor(10,9);
			printf("| $$__/  \\| $$__/ $$| $$ | $$ | $$| $$__/ $$| $$__/ $$ __");
			
			combo_line++;
		} else if (combo_line == 7) {
			move_terminal_cursor(10,10);
			printf(" \\$$    $$ \\$$    $$| $$ | $$ | $$| $$    $$ \\$$    $$|  \\");
			
			combo_line++;
		} else if (combo_line == 8) {
			move_terminal_cursor(10,11);
			printf("  \\$$$$$$   \\$$$$$$  \\$$  \\$$  \\$$ \\$$$$$$$   \\$$$$$$  \\$$");
			
			combo_line = 0;
		}
		
		if (!combo_check) {
			for (int term_line = 3; term_line < 12; term_line++) {
				move_terminal_cursor(10, term_line);
				clear_to_end_of_line();
				
			}
		}
		*/
		
		if (!man_mode & !paused) {
			current_time = get_current_time();
			
			if (current_time >= last_advance_time + game_speed/5)
			{
				// 200ms (0.2 second) has passed since the last time we advance the
				// notes here, so update the advance the notes
				advance_note();
				
				// Update the most recent time the notes were advanced
				last_advance_time = current_time;
			}
			
		} else if (man_mode) {
			if (keyboard_input == 'n' || keyboard_input == 'N') {
				advance_note();
			}
		}
	}
}

void handle_game_over()
{
	clear_terminal();
	move_terminal_cursor(10,13);
	printf_P(PSTR("GAME OVER"));
	move_terminal_cursor(10,14);
	printf("Final Score: %d\n", points);
	//Display current track
	move_terminal_cursor(10, 15);
	printf("Track: Through Fire & Flames");																//do %s and add a new track title at the end.
	move_terminal_cursor(10, 16);
	clear_to_end_of_line();
	//display game sped
	if (game_speed == 1000) {
		printf("Current Game Speed: 1 (Normal: %d ms)", game_speed);
		
		} else if (game_speed == 500) {
		printf("Current Game Speed: 2 (Fast: %d ms)", game_speed);
		
		} else if (game_speed == 250) {
		printf("Current Game Speed: 3 (Extreme: %d ms)", game_speed);
	}
	
	move_terminal_cursor(10, 17);
	printf_P(PSTR("Press a button or 's'/'S' to start a new game"));
	
	// Do nothing until a button is pushed. Hint: 's'/'S' should also start a
	// new game
	while (button_pushed() == NO_BUTTON_PUSHED)
	{
		char keyboard_input = -1;
				
		if (serial_input_available()) {
			keyboard_input = fgetc(stdin);
		}
		
		//check if the user presses s
		if (keyboard_input == 's' || keyboard_input == 'S') {
			break;
		}
	}	
	
	start_screen();
}
