#include "config.h"
#include "com.h"
#include "flashlight.h"
#include "game.h"
#include "sound.h"
#include "graphic.h"
#include "player.h"
#include <stdio.h>
#include "mixkit_arcade_retro.h"
#include "short-beep-tone-47916.h"

enum gameControl_st_t
{
    init_st,     // initalize
    new_game_st, // create display and wait for next player
    cont_game_st,
    game_st,
    wait_restart_st
};

static enum gameControl_st_t currentState;
player_t ghost;
player_t hunter;
static player_type_t game_type;
player_type_t last_win;
bool ghost_in_beam;
bool player_collision;
bool starting_game;
bool continuing;
static int8_t hunter_win;
static int8_t ghost_win;
static int16_t timer;

char ghost_win_message[] = "Spooky, the Ghost Won!";
char hunter_win_message[] = "Fantastic, the Hunter Won!";
char next_round_message[] = "Next Round, Press B";
char new_game_message[] = "Press Start";

/****************************Initialize *******************************/

//check if the players are close to each other
int32_t player_close(player_t player1, player_t player2)
{
    int32_t x_diff = (player1.x_current - player2.x_previous);
    int32_t y_diff = (player1.y_current - player2.y_previous);
    int32_t distance = (x_diff * x_diff) + (y_diff * y_diff);
    if (distance <= 700)
    {
        return 12;
    }
    else if (distance <= 2500)
    {
        return 50;
    }
    else
    {
        return 150;
    }
}

// initialize game logic
void game_init(void)
{
    currentState = init_st;
    hunter_win = 0;
    ghost_win = 0;
    player_collision = false;
    starting_game = false;
    continuing = false;
    cursor_init(PER_MS);

    sound_set_volume(40);
}

/****************************GAME TICK *******************************/

// update the game logic
void game_tick(void)
{
    coord_t x, y;
    coord_t gx, gy;
    // Process state transitions first.
    switch (currentState)
    {
    case init_st: // transition to new_game_st
        currentState = new_game_st;
        break;
    case new_game_st:
        if (starting_game)
        {//to game_st
            currentState = game_st;
            hunter_init(&hunter);
            ghost_init(&ghost);
            if (game_type == PLAYER_TYPE_HUNTER)
            {
                cursor_set_sensitivity(.1);
                cursor_set_pos(hunter.x_current, hunter.y_current);
            }
            else if (game_type == PLAYER_TYPE_GHOST)
            {
                cursor_set_sensitivity(.05);
                cursor_set_pos(ghost.x_current, ghost.y_current);
            }
            flashlight_init(&hunter);
            timer = 1200;
            starting_game = false;
        }
        break;
    case cont_game_st:
        if (continuing)
        {//to game_st
            hunter_init(&hunter);
            ghost_init(&ghost);
            flashlight_init(&hunter);
            currentState = game_st;
            if (game_type == PLAYER_TYPE_HUNTER)
            {
                cursor_set_sensitivity(.1);
                cursor_set_pos(hunter.x_current, hunter.y_current);
            }
            else if (game_type == PLAYER_TYPE_GHOST)
            {
                cursor_set_sensitivity(.05);
                cursor_set_pos(ghost.x_current, ghost.y_current);
            }
            timer = 1200;
        }
        break;
    case game_st:
        if (ghost_in_beam)
        {//to restart_st
            last_win = PLAYER_TYPE_HUNTER;
            hunter_win++;
            ghost_in_beam = false;
            set_collision(&ghost);
            set_collision(&hunter);
            currentState = wait_restart_st;
        }
        if (player_collision)
        {//to restart_st
            last_win = PLAYER_TYPE_GHOST;
            ghost_win++;
            player_collision = false;
            set_collision(&ghost);
            set_collision(&hunter);
            currentState = wait_restart_st;
        }
        break;
    case wait_restart_st: // transition to new game if start
        if (pin_get_level(HW_BTN_START) == 0 && (hunter_win == WIN_MAX || ghost_win == WIN_MAX))
        {//to new_game
            hunter_win =0;
            ghost_win = 0;
            currentState = new_game_st;
        }
        if (pin_get_level(HW_BTN_B) == 0 && hunter_win < WIN_MAX && ghost_win < WIN_MAX)
        {//to continuing
            currentState = cont_game_st;
            continuing = true;
        }
        break;
    default:
        break;
    }

    // Process state actions next.
    switch (currentState)
    {
    case init_st://no action
        break;
    case new_game_st://clear buffer and draw board
        graphic_start_menu();
        uint64_t temp;
        while (com_read(&temp, sizeof(temp)) > 0)
        {
            ;
        } // flush buffer
        if (pin_get_level(HW_BTN_A) == 0)
        {
            game_type = PLAYER_TYPE_HUNTER;
            lcd_fillCircle(CIRCLE_XC, HUNTER_YC, PLAYER_R, GRAPHIC_PLAYER_COLOR);
            starting_game = true;
        }
        else if (pin_get_level(HW_BTN_B) == 0)
        {
            game_type = PLAYER_TYPE_GHOST;
            lcd_fillCircle(CIRCLE_XC, GHOST_YC, PLAYER_R, GRAPHIC_GHOST_COLOR);
            starting_game = true;
        }
        lcd_writeFrame();
        break;
    case cont_game_st://clear buffer
        while (com_read(&temp, sizeof(temp)) > 0)
        {
            ;
        } // flush buffer
        break;
    case game_st: // game itself
        cursor_tick();
        if (game_type == PLAYER_TYPE_HUNTER)
        {//hunter
            player_tick(&hunter);
            player_get_loc(&x, &y, &hunter);
            com_send_pos(x, y); // Transmit
            if (com_recieve_pos(&gx, &gy))
            { // recieve
                position_set(&ghost, &gx, &gy);
            }
            com_send_bool(a_is_pressed(&hunter));
            bool button_state;
            if (com_recieve_bool(&button_state))
            {//button state
                if (button_state)
                {
                    set_a(&ghost);
                }
                else
                {
                    unset_a(&ghost);
                }
            }
        }
        if (game_type == PLAYER_TYPE_GHOST)
        {//ghost 
            if (a_is_pressed(&ghost))
            {
                cursor_set_sensitivity(.1);
            }
            else
            {
                cursor_set_sensitivity(.05);
            }

            player_tick(&ghost);
            player_get_loc(&gx, &gy, &ghost);
            com_send_pos(gx, gy); // transmit
            if (com_recieve_pos(&x, &y))
            { // recieve
                position_set(&hunter, &x, &y);
            }
            bool button_state;
            if (com_recieve_bool(&button_state))
            {
                if (button_state)
                {
                    set_a(&hunter);
                }
                else
                {
                    unset_a(&hunter);
                }
            }
            com_send_bool(a_is_pressed(&ghost));
        }
        // Check for collisions or in beam
        ghost_in_beam = false;
        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            float angle = (2 * PI / NUM_SAMPLES) * i;
            coord_t sample_x = ghost.x_current + (coord_t)(CHARACTER_RAD * cosf(angle));
            coord_t sample_y = ghost.y_current + (coord_t)(CHARACTER_RAD * sinf(angle));

            if (in_beam(&hunter, sample_x, sample_y))
            {//check if ghost in beam
                ghost_in_beam = true;
                sound_start(mixkit_arcade_retro, sizeof(mixkit_arcade_retro), false);
                break;
            }

            if (!ghost_in_beam)
            {//check for collision if not in beam
                player_collision = collision(&hunter, ghost.x_current, ghost.y_current);
                if (player_collision)
                {
                    sound_start(mixkit_arcade_retro, sizeof(mixkit_arcade_retro), false);
                }
            }
        }
        if (timer <= 0)
        {//if timer gets to 0 hunter wins
            ghost_in_beam = true;
            sound_start(mixkit_arcade_retro, sizeof(mixkit_arcade_retro), false);
        }
        if (game_type == PLAYER_TYPE_HUNTER)
        {//send in beam bool
            com_send_bool(ghost_in_beam);
            com_recieve_bool(&player_collision);
        }
        else
        {//receive in beam bool
            com_recieve_bool(&ghost_in_beam);
            com_send_bool(player_collision);
        }
        static int beep_cooldown = 0;
        int32_t proximity_level = player_close(hunter, ghost);

        // Only beep when players are close (proximity_level < 100 means within range)
        if (proximity_level < PROX_MAX)
        {
            if (beep_cooldown <= 0)
            {
                sound_start(short_beep_tone_47916, sizeof(short_beep_tone_47916), false);
                beep_cooldown = proximity_level; // Closer = faster beeping
            }
        }
        else
        {
            beep_cooldown = 0; // Reset when far away
        }

        // Always decrement cooldown
        if (beep_cooldown > 0)
        {
            beep_cooldown--;
        }

        timer--;
        // Display
        graphic_print_wall();
        flashlight_tick();
        if (ghost_in_beam || player_collision || game_type == PLAYER_TYPE_GHOST || a_is_pressed(&ghost) || ghost.flashlight || timer <= 0)
        {
            graphic_draw_ghost(ghost.x_current, ghost.y_current);
        }
        graphic_draw_player(hunter.x_current, hunter.y_current);
        graphic_draw_timer(timer);
        graphic_draw_score(hunter_win, ghost_win);
        lcd_writeFrame();
        break;
    case wait_restart_st: // messages
        if (hunter_win >= WIN_MAX || ghost_win >= WIN_MAX)
        {
            wait_restart_display(last_win);
        }
        if (hunter_win < WIN_MAX && ghost_win < WIN_MAX)
        {
            lcd_setFontSize(2);
            lcd_drawString(MESS_X, MESS_Y, next_round_message, WHITE);
            lcd_writeFrame();
        }
        break;
    default:
        break;
    }
}

/****************************Additional function *******************************/
 
//endgame display
void wait_restart_display(player_type_t last_win)
{
    lcd_setFontSize(2);

    if (last_win == PLAYER_TYPE_GHOST)
    {// ghost win
        lcd_drawString(INIT_WIDTH, LCD_H / 2, ghost_win_message, WHITE);
        lcd_drawString(WIN_X, MESS_Y, new_game_message, WHITE);
    }
    if (last_win == PLAYER_TYPE_HUNTER)
    {//hunter win
        lcd_drawString(NEW_X, LCD_H / 2, hunter_win_message, WHITE);
        lcd_drawString(WIN_X, MESS_Y, new_game_message, WHITE);
    }
    lcd_writeFrame();
}
