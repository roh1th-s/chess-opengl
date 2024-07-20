#include <GLFW/glfw3.h>

#include "../../gfx/renderer.h"
#include "../../ui/imagebox.h"
#include "../../ui/textbox.h"
#include "../../ui/ui.h"
#include "../game.h"
#include "../movegen.h"
#include "../piece.h"
#include "gameplay_state.h"

static Vec2i calc_board_relative_pos(Vec2i board_top_left, Vec2i board_size, Vec2i square,
                                     ChessColor plr_color)
{
    return (Vec2i){board_top_left.x + (plr_color == BLACK ? (7 - square.x) : square.x) * board_size.x / 8,
                   board_top_left.y - (plr_color == WHITE ? (7 - square.y) : square.y) * board_size.y / 8};
}

static void empty_move_list(ChessGame *game)
{
    free(game->current_move_list.moves);
    game->current_move_list = (MoveList){NULL, 0};
}

GameState *gameplay_state_init()
{
    GameState *self = (GameState *)malloc(sizeof(GameState));

    self->type = GAMEPLAY_STATE;
    self->setup = gameplay_state_setup;
    self->update = gameplay_state_update;
    self->render = gameplay_state_render;
    self->cleanup = gameplay_state_cleanup;

    return self;
};

void gameplay_state_setup(ChessGame *game)
{
    game->current_turn = WHITE;
    game->player_color = WHITE;
    game->selected_piece = NULL;
    game->selected_square = (Vec2i){-1, -1};
    game->current_move_list = (MoveList){NULL, 0};
    game->mouse_down = false;
    game->mouse_pos = (Vec2f){0, 0};

    Window *w = game->renderer->window;
    int width = w->width;
    int height = w->height;
    Vec2i center = {width / 2, height / 2};

    game->board_size = (Vec2i){670, 670};
    game->board_pos = (Vec2i){36, center.y + game->board_size.y / 2};

    chess_board_init(&game->board);

    // background
    imagebox_create(game->ui, (Vec2i){0, height}, (Vec2i){width, height}, game->bg_texture,
                    (Color3i){255, 255, 255});

    // board
    imagebox_create(game->ui, game->board_pos, game->board_size, game->board_texture,
                    (Color3i){255, 255, 255});

    // player icon
    imagebox_create(game->ui, (Vec2i){36, 52}, (Vec2i){45, 45}, game->player_icon_texture,
                    (Color3i){255, 255, 255});
    textbox_create(game->ui, (Vec2i){81, 52}, (Vec2i){70, 15}, (Padding){0}, "You", game->secondary_font);

    // opponent icon
    imagebox_create(game->ui, (Vec2i){661, 789}, (Vec2i){45, 45}, game->player_icon_texture,
                    (Color3i){255, 255, 255});
    textbox_create(game->ui, (Vec2i){610, 789}, (Vec2i){50, 15}, (Padding){0}, "AI", game->secondary_font);
};

void gameplay_state_update(ChessGame *game)
{
    Window *w = game->renderer->window;

    double xpos, ypos;
    glfwGetCursorPos(w->glfw_window, &xpos, &ypos);
    ypos = w->height - ypos; // glfw has origin at top left
    game->mouse_pos = (Vec2f){xpos, ypos};

    bool left_btn_pressed = glfwGetMouseButton(w->glfw_window, GLFW_MOUSE_BUTTON_LEFT);

    Vec2i board_pos = game->board_pos;
    Vec2i board_size = game->board_size;
    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    if (xpos > board_pos.x && xpos < board_pos.x + board_size.x && ypos > board_pos.y - board_size.y &&
        ypos < board_pos.y)
    {
        // mouse is inside board

        if (!game->mouse_down && left_btn_pressed)
        {
            // mouse btn down
            game->mouse_down = true;

            int x = ((xpos - (float)board_pos.x) / square_size.x);
            int y = 7 - (int)(((float)board_pos.y - ypos) / square_size.y);

            if (game->player_color == BLACK)
            {
                x = 7 - x;
                y = 7 - y;
            }

            if (game->board.squares[x][y])
            {
                game->selected_square = (Vec2i){x, y};
                game->selected_piece = game->board.squares[x][y];
                game->current_move_list = generate_moves(game->selected_piece, (Vec2i){x, y}, &game->board);
            }
            else
            {
                game->selected_square = (Vec2i){-1, -1};
                game->selected_piece = NULL;
            }
        }
        else if (game->mouse_down && !left_btn_pressed)
        {
            // clicked and released
            game->mouse_down = false;
        }
    }
    else
    {
        if (!game->mouse_down && left_btn_pressed)
        {
            // user clicked outside board
            game->selected_square = (Vec2i){-1, -1};
            game->selected_piece = NULL;
            empty_move_list(game);
        }
        else if (game->mouse_down && !left_btn_pressed)
        {
            // user released mouse outside board
            game->mouse_down = false;
        }
    }
};

void gameplay_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;

    int width = w->width;
    int height = w->height;

    Vec2i board_size = game->board_size;
    Vec2i board_pos = game->board_pos;

    ChessColor plr_color = game->player_color;

    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    // selected square
    if (game->selected_square.x != -1)
    {

        Vec2i square_pos = calc_board_relative_pos(board_pos, board_size, game->selected_square, plr_color);

        renderer_draw_rect(r, (Color3i){255, 0, 0}, square_pos,
                           (Vec2i){ceil(square_size.x), ceil(square_size.y)});
    }

    // pieces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece *piece = game->board.squares[i][j];
            if (piece != NULL)
            {
                if (game->mouse_down && game->selected_piece == piece)
                    continue;

                Texture tex = game->piece_textures[piece->type + (piece->color == BLACK ? 6 : 0)];
                Vec2i piece_pos = calc_board_relative_pos(board_pos, board_size, (Vec2i){i, j}, plr_color);

                renderer_draw_rect_tex(r, tex, piece_pos, (Vec2i){square_size.x, square_size.y});
            }
        }
    }

    ChessPiece *selected_piece = game->selected_piece;

    // selected piece moves
    if (selected_piece && game->current_move_list.n_moves > 0)
    {
        for (int i = 0; i < game->current_move_list.n_moves; i++)
        {
            ChessMove move = game->current_move_list.moves[i];
            Vec2i square_pos = calc_board_relative_pos(board_pos, board_size, move.to, plr_color);

            renderer_draw_rect(r, (Color3i){0, 255, 0}, square_pos,
                               (Vec2i){ceil(square_size.x), ceil(square_size.y)});
        }
    }

    // dragged piece
    if (game->mouse_down && selected_piece)
    {
        Texture tex = game->piece_textures[selected_piece->type + (selected_piece->color == BLACK ? 6 : 0)];
        Vec2i piece_pos =
            (Vec2i){game->mouse_pos.x - square_size.x / 2, game->mouse_pos.y + square_size.y / 2};

        renderer_draw_rect_tex(r, tex, piece_pos, (Vec2i){square_size.x, square_size.y});
    }
};

void gameplay_state_cleanup(ChessGame *game) { ui_destroy_all(game->ui); };