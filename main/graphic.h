#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "hw.h"
#include <math.h>
#include "config.h"

#include "lcd.h" // coord_t

// This function is responsible for drawing the walls
void graphic_print_wall();

//responsible for drawing ghost
void graphic_draw_ghost(coord_t x, coord_t y);

//draws hunter
void graphic_draw_player(coord_t x, coord_t y);

//draws initial start menu
void graphic_start_menu();

//draws timer
void graphic_draw_timer(int16_t t);

//draws scores
void graphic_draw_score(int8_t hunter_win, int8_t ghost_win);

#endif