#pragma once
#include "esp_system.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*	7SEG display pattern
	Hex number encode arrangement: 0bdp gfedcba
	aaaa
   f	b
   f	b
	gggg
   e	c
   e	c
	dddd dp
*/

uint8_t get_char_segment(char letter);