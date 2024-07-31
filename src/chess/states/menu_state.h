#if !defined (MENU_STATE_H)
#define MENU_STATE_H

#include "../game.h"

GameState *menu_state_init();
void menu_state_setup(ChessGame *game);
void menu_state_update(ChessGame *game, double delta_time);
void menu_state_render(ChessGame *game);
void menu_state_cleanup(ChessGame *game);

#endif