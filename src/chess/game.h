#if !defined(GAME_H)
#define GAME_H

#include <stdbool.h>

#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../ui/ui.h"
#include "board.h"
#include "movegen.h"
#include "piece.h"

#define MAX_PIECE_ANIMATIONS 2

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

typedef enum {
    CHECKMATE,
    STALEMATE,
    DRAW_BY_AGREEMENT,
    RESIGNATION,
} GameEndReason;

typedef void (*GameStateCB)(ChessGame *);
typedef void (*GameStateUpdateCB)(ChessGame *, double);

typedef struct
{
    GameStateType type;

    GameStateCB setup;
    GameStateUpdateCB update;
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
    Texture menu_bg_texture;
    Texture piece_textures[12];
    Texture player_icon_texture;
    Texture circle_texture;

    Font *primary_font;
    Font *secondary_font;
    Font *tertiary_font;

    GameStateType current_state;
    GameState *states[2];

    struct ChessData
    {
        ChessBoard board;
        ChessColor player_color; // white = 1, black = 0
        ChessColor current_turn;
        MoveList current_move_list;
        bool is_in_check : 1;
        bool is_game_over : 1;
        GameEndReason game_end_reason; 
    } chess_data;

    struct UIData
    {
        Vec2i board_pos;
        Vec2i board_size;
        Vec2f mouse_pos;
        bool mouse_down;

        struct
        {
            ChessPiece *animating_piece;
            Vec2i animating_from;
            Vec2i animating_to;
            float animation_time;
        } piece_animations[MAX_PIECE_ANIMATIONS];

        ChessPiece *selected_piece;
        Vec2i selected_square;
        bool promotion_menu_open;
        ChessPiece *pawn_to_promote;
        ChessMove *pending_promotion_move;
        Vec2i promotion_menu_pos;
        Vec2i promotion_menu_size;
    } ui_data;
} ChessGame;

void game_init(ChessGame *self, Renderer *r);
void game_start(ChessGame *self);
void game_switch_to_state(ChessGame *self, GameStateType state);
void game_update(ChessGame *self, double delta_time);
void game_render(ChessGame *self);

#endif