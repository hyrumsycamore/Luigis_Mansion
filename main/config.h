#ifndef CONFIG_H_
#define CONFIG_H_

#define CONFIG_GAME_TIMER_PERIOD 50.0E-3f
#define PER_MS ((uint32_t)(CONFIG_GAME_TIMER_PERIOD * 1000))

#define STR_BUF_LEN 12

#define NUM_SAMPLES 16
#define PI 3.1415

//graphics 
#define GRAPHIC_WALL_COLOR rgb565(40,40,40)
#define GRAPHIC_CARPET_COLOR rgb565(125,25,25)
#define GRAPHIC_GHOST_COLOR WHITE
#define GRAPHIC_PLAYER_COLOR BLUE
#define CHARACTER_RAD 6
#define FIFTEEN_DEGREES .226

//player
#define PRESSED 0
#define MAX_MOVE_PER_UPDATE 3.0f

#define PLAY_AREA_MIN_X 10
#define PLAY_AREA_MAX_X 310
#define PLAY_AREA_MIN_Y 10
#define PLAY_AREA_MAX_Y 230

#define INIT_WIDTH LCD_W/8
#define INIT_HEIGHT LCD_H/8

#define WIN_MAX 3

//flashlight 
#define FLASHLIGHT_RAD 5*CHARACTER_RAD
#define FLASHLIGHT_LOW_RAD 3*CHARACTER_RAD

//flashlight battery
#define BATT_WIDTH 15
#define BATT_HEIGHT 12
#define BATT_RAD 4
#define BATT_COLOR rgb565(255, 125, 0)
#define BATT_RAND 20


//fllashlight life
#define CONFIG_MAX_FLASHLIGHT_LIFE 20.0f
#define CONFIG_FLASHLIGHT_LIFE_TICK \
    ((uint32_t)(CONFIG_MAX_FLASHLIGHT_LIFE / CONFIG_GAME_TIMER_PERIOD))


//game
#define CIRCLE_XC LCD_W * 2 / 3
#define HUNTER_YC 90
#define GHOST_YC 180
#define PLAYER_R 20

#define PROX_MAX 100
#define MESS_X 40
#define MESS_Y LCD_H / 2 + 20
#define WIN_X 50
#define NEW_X 10


#endif //CONFIG_H_