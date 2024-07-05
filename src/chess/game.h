#if !defined(GAME_H)
#define GAME_H

#include <stdbool.h>

#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../ui/ui.h"
#include "board.h"

#define LOAD_TEXTURE(var, path)                                                                              \
    {                                                                                                        \
        Texture tex;                                                                                         \
        texture_create(&tex, path);                                                                          \
        var = tex;                                                                                        \
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
    Font *primary_font;
    Font *secondary_font;

    GameStateType current_state;
    GameState *states[2];

    ChessBoard board;
} ChessGame;

void game_init(ChessGame *self, Renderer *r);
void game_start(ChessGame *self);
void game_switch_to_state(ChessGame *self, GameStateType state);
void game_update(ChessGame *self);
void game_render(ChessGame *self);

#endif