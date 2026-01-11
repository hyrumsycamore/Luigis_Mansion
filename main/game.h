#ifndef GAME_H_
#define GAME_H_

/****************************Initialize *******************************/

//check if the players are close to each other
int32_t player_close(player_t player1, player_t player2);

// Initialize the game logic.
void game_init(void);

/****************************GAME TICK *******************************/

// Update the game logic.
void game_tick(void);

/****************************Additional function *******************************/

//endgame display
void wait_restart_display(player_type_t last_win);

#endif // GAME_H_