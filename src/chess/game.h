#if !defined(GAME_H)
#define GAME_H

#include <stdbool.h>

#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../ui/ui.h"
#include "board.h"
#include "piece.h"
#include "movegen.h"

#define LOAD_TEXTURE(var, path)                                                                              \
    {                                                                                                        \
        Texture tex;                                                                                         \
        texture_create(&tex, path);                                                                          \
        var = tex;                                                                                           \
    }

#define LOAD_FONT(var, path, size)                                                                           \
    {                                                                                                        \
        Font *f = (Font *)malloc(sizeof(Font));                                                              \
        font_init(f, path, size);                                                                            \
        var = f;                                                                                             \
    }

typedef struct ChessGame ChessGame;

typedef enum
{
    INIT_STATE = -1,
    MENU_STATE,
    GAMEPLAY_STATE
} GameStateType;

typedef void (*GameStateCB)(ChessGame *);

typedef struct
{
    GameStateType type;

    GameStateCB setup;
    GameStateCB update;
    GameStateCB render;
    GameStateCB cleanup;
} GameState;

typedef struct ChessGame
{
    Renderer *renderer;
    UIManager *ui;

    Texture board_texture;
    Texture bg_texture;
    Texture bg_texture2;
    Texture piece_textures[12];
    Texture player_icon_texture;

    Font *primary_font;
    Font *secondary_font;

    GameStateType current_state;
    GameState *states[2];

    ChessBoard board;
    ChessColor player_color; // white = 1, black = 0
    ChessColor current_turn;
    MoveList current_move_list;

    Vec2i board_pos;
    Vec2i board_size;
    Vec2f mouse_pos;
    bool mouse_down;
    ChessPiece *selected_piece;
    Vec2i selected_square;
} ChessGame;

void game_init(ChessGame *self, Renderer *r);
void game_start(ChessGame *self);
void game_switch_to_state(ChessGame *self, GameStateType state);
void game_update(ChessGame *self);
void game_render(ChessGame *self);

#endif