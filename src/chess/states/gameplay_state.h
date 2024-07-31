#if !defined (GAMEPLAY_STATE_H)
#define GAMEPLAY_STATE_H

#include "../game.h"

GameState *gameplay_state_init();
void gameplay_state_setup(ChessGame *game);
void gameplay_state_update(ChessGame *game, double delta_time);
void gameplay_state_render(ChessGame *game);
void gameplay_state_cleanup(ChessGame *game);

#endif