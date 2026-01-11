#ifndef FLASHLIGHT_H_
#define FLASHLIGHT_H_

#include "player.h"
#include "config.h"

/*****Flashlight Init *******/

//Initialize state machine and flags
void flashlight_init(player_t *player);

/******Flashlight Control ******/

//state machine tick function
void flashlight_tick(void);

/******Additional functions  ******/

// Using the new and previous coordinates, you construct the
void graphic_draw_flashlight(coord_t old_x, coord_t old_y, coord_t new_x, coord_t new_y, uint8_t RAD);

//draws battery pack
void graphic_draw_battery();

//checks if close to battery
bool close_to_battery();

#endif