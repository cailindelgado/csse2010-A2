/*
 * pixel_colour.h
 *
 * Author: Peter Sutton
 */ 

#ifndef PIXEL_COLOUR_H_
#define PIXEL_COLOUR_H_

#include <stdint.h>

// Each PixelType is an 8 bit number - 4 bits of green in the high bits,
// 4 bits of red in the low bits
typedef uint8_t PixelColour;

// Some useful colour definitions
#define COLOUR_BLACK		(0x00)
#define COLOUR_RED			(0x0F) // -> 0000 1111
#define COLOUR_DARK_RED		(0x01) // -> 0000 0001
#define COLOUR_GREEN		(0xF0)
#define COLOUR_ORANGE		(0x3C) // -> 0011 1100
#define COLOUR_DARK_ORANGE	(0x1C) // -> 0001 1100
#define COLOUR_HALF_YELLOW  (0x55)
#define COLOUR_QUART_YELLOW (0x11)
#define COLOUR_YELLOW       (0xFF)

#endif /* PIXEL_COLOUR_H_ */
