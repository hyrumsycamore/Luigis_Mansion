#include "player.h"
#include <stdio.h>
#include "config.h"

// walls
const Wall walls[] = {
    // Outer boundaries
    {0, 0, 320, 10},    // Top
    {0, 230, 320, 240}, // Bottom
    {0, 0, 10, 240},    // Left
    {310, 0, 320, 240}, // Right

    // Vertical barriers
    {100, 10, 105, 70},
    {100, 180, 105, 240},
    {215, 10, 220, 70},
    {215, 180, 220, 240},

    // Horizontal platforms
    {0, 160, 80, 165},
    {0, 90, 80, 95},
    {240, 160, 320, 165},
    {240, 90, 320, 95},

    // Central vertical barriers
    {190, 40, 195, 200},
    {125, 40, 130, 200}};

// Player States
enum playerControl_st_t
{
    moving,
    A_PRESSED,
    endGame
};

/***************Initialize players *******************/

//checks is pixel is a wall
bool is_wall(coord_t x, coord_t y)
{
    // Check play area boundaries
    if (x < PLAY_AREA_MIN_X || x > PLAY_AREA_MAX_X ||
        y < PLAY_AREA_MIN_Y || y > PLAY_AREA_MAX_Y)
    {
        return true;
    }

    // Check against wall definitions
    for (size_t i = 0; i < sizeof(walls) / sizeof(walls[0]); i++)
    {
        const Wall *w = &walls[i];
        if (x >= w->x1 && x <= w->x2 &&
            y >= w->y1 && y <= w->y2)
        {
            return true;
        }
    }

    return false;
}

// Check if two line segments intersect
bool lines_intersect(coord_t x1, coord_t y1, coord_t x2, coord_t y2,
                     coord_t x3, coord_t y3, coord_t x4, coord_t y4)
{
    int d1 = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    int d2 = (x2 - x1) * (y4 - y1) - (y2 - y1) * (x4 - x1);
    int d3 = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    int d4 = (x4 - x3) * (y2 - y3) - (y4 - y3) * (x2 - x3);

    return (d1 * d2 < 0) && (d3 * d4 < 0);
}

// Check if a line intersects a rectangle
bool line_intersects_wall(coord_t x1, coord_t y1, coord_t x2, coord_t y2, const Wall *w)
{
    return lines_intersect(x1, y1, x2, y2, w->x1, w->y1, w->x2, w->y1) || // Top
           lines_intersect(x1, y1, x2, y2, w->x2, w->y1, w->x2, w->y2) || // Right
           lines_intersect(x1, y1, x2, y2, w->x2, w->y2, w->x1, w->y2) || // Bottom
           lines_intersect(x1, y1, x2, y2, w->x1, w->y2, w->x1, w->y1);   // Left
}

// checks if Valid Move
bool valid_move(coord_t x_pos, coord_t y_pos)
{
    if (is_wall(x_pos + CHARACTER_RAD, y_pos) ||
        is_wall(x_pos - CHARACTER_RAD, y_pos) ||
        is_wall(x_pos, y_pos + CHARACTER_RAD) ||
        is_wall(x_pos, y_pos - CHARACTER_RAD))
    {
        return false;
    }
    return true;
}

//allows player to move
void move_player(player_t *player, coord_t target_x, coord_t target_y)
{
    // Try X movement
    if (valid_move(target_x, player->y_current))
    {
        player->x_current = target_x;
    }
    // Try Y movement
    if (valid_move(player->x_current, target_y))
    {
        player->y_current = target_y;
    }
}

// Initialize hunter
void hunter_init(player_t *player)
{
    player->type = PLAYER_TYPE_HUNTER;
    player->currentState = moving;
    player->flashlight = false;
    player->flashlight_rad = FIFTEEN_DEGREES;
    player->collision = false;
    player->x_current = INIT_WIDTH;
    player->y_current = INIT_HEIGHT;
    player->x_previous = INIT_WIDTH;
    player->y_previous = INIT_HEIGHT;
}

// Initialize ghost
void ghost_init(player_t *player)
{
    player->type = PLAYER_TYPE_HUNTER;
    player->currentState = moving;
    player->collision = false;
    player->x_current = LCD_W / 2;
    player->y_current = LCD_H / 2;
}

/************************Player tick******************************/

// Tick the state machine for a single player
void player_tick(player_t *player)
{
    // Transition
    switch (player->currentState)
    {
    case moving://to pressed state
        if (pin_get_level(HW_BTN_A) == PRESSED)
        {
            player->currentState = A_PRESSED;
            player->flashlight = true;
        }
        break;
    case A_PRESSED:
        if (pin_get_level(HW_BTN_A) != PRESSED)
        {//to moving state
            player->flashlight = false;
            player->currentState = moving;
        }
        if (player->collision)
        {//to endgame
            player->flashlight = false;
            player->currentState = endGame;
        }
        break;
    case endGame://to moving state
            if (player->type == PLAYER_TYPE_HUNTER)
            {
                hunter_init(player);
            }
            else
            {
                ghost_init(player);
            }
            player->currentState = moving;
        break;
    }
    // Action
    switch (player->currentState)
    {
    case moving://update location
        if (player->x_previous != player->x_current)
        {
            player->x_previous = player->x_current;
        }
        if (player->y_previous != player->y_current)
        {
            player->y_previous = player->y_current;
        }

        coord_t target_x, target_y;
        cursor_get_pos(&target_x, &target_y);

        move_player(player, target_x, target_y);
        cursor_set_pos(player->x_current, player->y_current);
        break;
    case A_PRESSED://update location
        if (player->x_previous != player->x_current)
        {
            player->x_previous = player->x_current;
        }
        if (player->y_previous != player->y_current)
        {
            player->y_previous = player->y_current;
        }
        cursor_get_pos(&target_x, &target_y);

        move_player(player, target_x, target_y);
        cursor_set_pos(player->x_current, player->y_current);
        break;
    case endGame://no action
        break;
    }
}

/**************************Additional Functions**********************/

// // Sets the other players coordinates and visibility
void position_set(player_t *player, coord_t *x, coord_t *y)
{
    // Sets the previous movement to be the current movement
    player->y_previous = player->y_current;
    player->x_previous = player->x_current;
    // Sets the current movement to be the new position
    player->x_current = *x;
    player->y_current = *y;
}

// // Check for a collision between the ghost and player
bool collision(player_t *player, coord_t x, coord_t y)
{
    // Calculate squared distance between the characters
    int32_t x_dif = player->x_current - x;
    int32_t y_dif = player->y_current - y;
    int32_t dist_sq = x_dif * x_dif + y_dif * y_dif;

    int32_t rad = CHARACTER_RAD + CHARACTER_RAD;
    int32_t rad_sq = rad * rad;

    // Check if squared distance is less than or equal to squared radius
    return dist_sq <= rad_sq;
}

// Check for the ghost in the hunter flashlight beam
bool in_beam(player_t *player, coord_t x, coord_t y)
{
    if (!player->flashlight)
        return false;

    // Calculate direction vector
    float facing_x = (float)(player->x_current - player->x_previous);
    float facing_y = (float)(player->y_current - player->y_previous);

    // Calculate delta to target
    float dif_x = (float)(player->x_current - x);
    float dif_y = (float)(player->y_current - y);

    // Distance check (squared)
    float rad_sq = (float)(player->flashlight_length * player->flashlight_length);
    if ((dif_x * dif_x + dif_y * dif_y) > rad_sq)
        return false;

    // Angle check with wrapping
    float theta = atan2f(facing_y, facing_x);
    float between_theta = atan2f(-dif_y, -dif_x); // Direction TO target
    float angle_diff = fmodf(between_theta - theta + PI, 2 * PI) - PI;

    // check that a wall clocks flashlight
    for(size_t i = 0; i < sizeof(walls)/ sizeof(walls[0]);i++){
        if(line_intersects_wall(player->x_current, player->y_current, x, y, &walls[i])){
            return false;
        }
    }     

    return fabsf(angle_diff) < player->flashlight_rad;
}

//checks if in pressed state
bool a_is_pressed(player_t *player)
{
    return player->currentState == A_PRESSED;
}

//sets flashlight to true
void set_a(player_t *player)
{
    player->flashlight = true;
}

//set flashlight to false
void unset_a(player_t *player)
{
    player->flashlight = false;
}

//sets collision to true
void set_collision(player_t *player)
{
    player->collision = true;
}

//gets player location
void player_get_loc(coord_t *x, coord_t *y, player_t *player)
{
    *x = player->x_current;
    *y = player->y_current;
}