#if !defined (GAME_H)
#define GAME_H

#include <stdbool.h>

#include "board.h"
#include "../gfx/renderer.h"
#include "../gfx/texture.h"

typedef enum {
    MENU_STATE,
    LEVEL_SELECT_STATE,
    GAMEPLAY_STATE 
} GameState;

typedef struct {
    Renderer* renderer;
    Texture board_texture;
    Font *primary_font;

    GameState state;

    ChessBoard board;
} ChessGame;

void game_init(ChessGame* self, Renderer* r);
void game_start(ChessGame* self);

#endif