#include "gameplay_state.h"
#include "../../gfx/renderer.h"
#include "../../ui/imagebox.h"
#include "../../ui/textbox.h"
#include "../../ui/ui.h"
#include "../game.h"
#include "../piece.h"

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
    game->player_color = BLACK;
    game->selected_piece = NULL;
    game->selected_square = (Vec2i){-1, -1};
    game->mouse_down = false;
    game->mouse_pos = (Vec2f){0, 0};

    Window *w = game->renderer->window;
    int width = w->width;
    int height = w->height;
    Vec2i center = {width / 2, height / 2};

    game->board_size = (Vec2i){670, 670};
    game->board_pos = (Vec2i){36, center.y + game->board_size.y / 2};

    chess_board_init(&game->board);

    //background
    imagebox_create(game->ui, (Vec2i){0, height}, (Vec2i){width, height}, game->bg_texture, (Color3i){255, 255, 255});

    //board
    imagebox_create(game->ui, game->board_pos, game->board_size, game->board_texture, (Color3i){255, 255, 255});

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
    game->mouse_pos = (Vec2f){xpos, ypos};
    ypos = w->height - ypos; // glfw has origin at top left

    bool left_btn_pressed = glfwGetMouseButton(w->glfw_window, GLFW_MOUSE_BUTTON_LEFT);

    Vec2i board_pos = game->board_pos;
    Vec2i board_size = game->board_size;

    if (xpos >= board_pos.x && xpos <= board_pos.x + board_size.x && ypos >= board_pos.y - board_size.y &&
        ypos <= board_pos.y)
    {
        if (!game->mouse_down && left_btn_pressed)
        {
            game->mouse_down = true;

            int x = (int)((xpos - board_pos.x) / (board_size.x / 8));
            int y = 7 - (int)((board_pos.y - ypos) / (board_size.y / 8));

            if (game->player_color == BLACK)
            {
                x = 7 - x;
                y = 7 - y;
            }
            game->selected_square = (Vec2i){x, y};
            printf("Clicked on square %d, %d\n", x, y);
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
            game->selected_square = (Vec2i){-1, -1};
        }
        else if (game->mouse_down && !left_btn_pressed)
        {
            // user released mouse outside board
            game->mouse_down = false;
            game->selected_square = (Vec2i){-1, -1};
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

    // selected square
    if (game->selected_square.x != -1)
    {
        Vec2i square_pos = {
            board_pos.x + (plr_color == BLACK ? (7 - game->selected_square.x) : game->selected_square.x) *
                              ((float)board_size.x / 8),
            board_pos.y - (plr_color == WHITE ? (7 - game->selected_square.y) : game->selected_square.y) *
                              ((float)board_size.y / 8)};
        renderer_draw_rect(r, (Color3i){255, 0, 0}, square_pos, (Vec2i){board_size.x / 8, board_size.y / 8});
    }

    // pieces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece *piece = game->board.squares[i][j];
            if (piece != NULL)
            {
                Texture tex = game->piece_textures[piece->type + (piece->color == BLACK ? 6 : 0)];
                Vec2i piece_size = {board_size.x / 8, board_size.y / 8};
                Vec2i piece_pos = {board_pos.x + (plr_color == WHITE ? (7 - j) : j) * piece_size.x,
                                   board_pos.y - (plr_color == WHITE ? (7 - i) : i) * piece_size.y};
                renderer_draw_rect_tex(r, tex, piece_pos, piece_size);
            }
        }
    }
};

void gameplay_state_cleanup(ChessGame *game) {

};