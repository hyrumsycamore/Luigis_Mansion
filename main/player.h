#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdbool.h>
#include <stdint.h>
#include "lcd.h"
#include "cursor.h"
#include <math.h>
#include "graphic.h"
#include "pin.h"
#include "hw.h"

typedef enum {
    PLAYER_TYPE_HUNTER,
    PLAYER_TYPE_GHOST
} player_type_t;

typedef struct
{
    coord_t x1, y1;
    coord_t x2, y2;
} Wall;

typedef struct{
    int32_t currentState;

    player_type_t type; 
    // Always on for player, changing for the ghost
    bool visible;
    // Always off for the ghost, changing for the player
    bool flashlight;
    int8_t flashlight_length;
    float flashlight_rad;

    bool collision;

    // The previous player position is used in 
    // determining the direction of the flashlight beam
    coord_t x_previous;
    coord_t y_previous;
    
    coord_t x_current;
    coord_t y_current;

    int32_t elapsed_time;
    int32_t period;

} player_t;

/***************Initialize players *******************/

// checks if pixel is a wall
bool is_wall(coord_t x, coord_t y);

//testing line intersection for walls
bool lines_intersect(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
                     coord_t x3, coord_t y3, coord_t x4, coord_t y4);

//compares line intersection to walls
bool line_intersects_wall(coord_t x1, coord_t y1, coord_t x2, coord_t y2, const Wall *w);

//checks if valid move 
bool valid_move(coord_t x_pos, coord_t y_pos);

//allows player to move 
void move_player(player_t *player, coord_t target_x, coord_t target_y);

// Initialize hunter
void hunter_init(player_t *player);

// Initialize ghost
void ghost_init(player_t *player);

/************************Player tick******************************/

// Tick the state machine for a single player
void player_tick(player_t *player);

/**************************Additional Functions**********************/

// // Sets the other players coordinates and visibility
void position_set(player_t *player, coord_t *x, coord_t *y);

// // Check for a collision between the ghost and player
bool collision(player_t *player, coord_t x, coord_t y);

// // Check for the ghost in the hunter flashlight beam
bool in_beam(player_t *player, coord_t x, coord_t y);

//checks if in pressed state
bool a_is_pressed(player_t *player);

//sets flashlight to true
void set_a(player_t *player);

//set flashlight to false
void unset_a(player_t *player);

// Sets the collision flag
void set_collision(player_t *player);

//get the players position 
void player_get_loc(coord_t *x, coord_t *y, player_t *player);

#endif
