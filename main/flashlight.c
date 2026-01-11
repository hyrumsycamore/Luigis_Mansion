#include "flashlight.h"
#include "config.h"
#include "graphic.h"
#include <stdio.h>

enum flash_st_t
{
    off_st,
    full_st,
    low_st,
    collision_st,
    dead_st
};
static enum flash_st_t currentState;
static player_t *flash_player;
bool player_flash;
bool player_collide;
uint16_t battery;
static bool change_battery;
coord_t prev_x, prev_y;
coord_t curr_x, curr_y;
coord_t batt_x, batt_y;

/******************************************Flashlight Init *************************/

// Initialize state machine and flags
void flashlight_init(player_t *player)
{
    currentState = off_st;
    battery = CONFIG_FLASHLIGHT_LIFE_TICK;
    flash_player = player;
    player_flash = player->flashlight;
    player_collide = player->collision;
    change_battery = false;
    prev_x = player->x_previous;
    prev_y = player->y_previous;
    curr_x = player->x_current;
    curr_y = player->y_current;
}

/***************************************Flashlight Control ****************************/

// state machine tick function
void flashlight_tick(void)
{
    player_flash = flash_player->flashlight;
    // Process state transitions first.
    switch (currentState)
    {
    case off_st:
        if (change_battery)
        {//to dead state
            srand(curr_x + curr_y);
            batt_x = (rand() % (LCD_W- (BATT_WIDTH+BATT_RAND))) + BATT_RAND;
            batt_y = (rand() % (LCD_H - (BATT_HEIGHT+BATT_RAND))) + BATT_RAND;
            change_battery = false;
            currentState = dead_st;
        }
        if (player_flash && battery > (CONFIG_FLASHLIGHT_LIFE_TICK / 2))
        {// to full state
            currentState = full_st;
            flash_player->flashlight_length = FLASHLIGHT_RAD;
        }
        else if (player_flash && (battery > 0 && battery <= (CONFIG_FLASHLIGHT_LIFE_TICK / 2)))
        {// to low state
            currentState = low_st;
        }
        break;
    case full_st:
        if (!player_flash)
        {// to off state
            currentState = off_st;
        }
        else if (player_flash && (battery > 0 && battery <= (CONFIG_FLASHLIGHT_LIFE_TICK / 2)))
        {// to low state
            currentState = low_st;
            flash_player->flashlight_length = FLASHLIGHT_LOW_RAD;
        }
        else if (player_collide)
        { //to collision state
            currentState = collision_st;
        }
        break;
    case low_st:
       if (change_battery)
        {//to dead state
            srand(curr_x + curr_y);
            batt_x = (rand() % (LCD_W- (BATT_WIDTH+BATT_RAND))) + BATT_RAND;
            batt_y = (rand() % (LCD_H - (BATT_HEIGHT+BATT_RAND))) + BATT_RAND;
            change_battery = false;
            currentState = dead_st;
            flash_player->flashlight_length = 0;
        }
        if (!player_flash)
        {
            currentState = off_st;
        }
        else if (player_flash && battery == 0)
        {
            change_battery = true;
        }
        else if (player_collide)
        {// to collision st
            currentState = collision_st;
        }
        break;
    case collision_st:
        currentState = off_st;
        break;
    case dead_st:
        if (close_to_battery())
        {//to off state
            currentState = off_st;
            flash_player->flashlight_length = FLASHLIGHT_RAD;
            battery = CONFIG_FLASHLIGHT_LIFE_TICK;
        }
    default:
        break;
    }

    // Process state actions next.
    switch (currentState)
    {
    case off_st://no action
        break;
    case full_st://draw full flashlight
        curr_x = flash_player->x_current;
        curr_y = flash_player->y_current;
        graphic_draw_flashlight(prev_x, prev_y, curr_x, curr_y, FLASHLIGHT_RAD);
        battery--;
        prev_x = curr_x;
        prev_y = curr_y;
        break;
    case low_st://draw low flashlight
        curr_x = flash_player->x_current;
        curr_y = flash_player->y_current;
        graphic_draw_flashlight(prev_x, prev_y, curr_x, curr_y, FLASHLIGHT_LOW_RAD);
        battery--;
        prev_x = curr_x;
        prev_y = curr_y;
        break;
    case collision_st: //no action
        break;
    case dead_st://draw battery
        curr_x = flash_player->x_current;
        curr_y = flash_player->y_current;
        graphic_draw_battery();
        break;
    default:
        break;
    }
}

/**************************************Additional Functions****************************/

// drawing helper function
void graphic_draw_flashlight(coord_t old_x, coord_t old_y, coord_t new_x, coord_t new_y, uint8_t RAD)
{
    float dif_x = new_x - old_x;
    float dif_y = new_y - old_y;
    float theta = atan2f(dif_y, dif_x);
    coord_t top_x = RAD * cosf(theta + FIFTEEN_DEGREES) + new_x;
    coord_t top_y = RAD * sinf(theta + FIFTEEN_DEGREES) + new_y;
    coord_t bottom_x = RAD * cosf(theta - FIFTEEN_DEGREES) + new_x;
    coord_t bottom_y = RAD * sinf(theta - FIFTEEN_DEGREES) + new_y;
    coord_t side_x = CHARACTER_RAD * cosf(theta) + new_x;
    coord_t side_y = CHARACTER_RAD * sinf(theta) + new_y;
    lcd_drawTriangle(side_x, side_y, top_x, top_y, bottom_x, bottom_y, YELLOW);
}

//draw battery helper
void graphic_draw_battery()
{
    lcd_fillRoundRect(batt_x, batt_y, BATT_WIDTH, BATT_HEIGHT, BATT_RAD, BATT_COLOR);
}

//checks if close to battery
bool close_to_battery()
{
    coord_t diff_x = batt_x - curr_x;
    coord_t diff_y = batt_y - curr_y;
    coord_t rad = 400;
    if ((diff_x * diff_x) + (diff_y * diff_y) <= rad)
    {
        return true;
    }
    return false;
}
