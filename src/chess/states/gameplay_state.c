#include <GLFW/glfw3.h>
#include <math.h>

#include "../../gfx/renderer.h"
#include "../../ui/button.h"
#include "../../ui/imagebox.h"
#include "../../ui/textbox.h"
#include "../../ui/ui.h"
#include "../board.h"
#include "../game.h"
#include "../movegen.h"
#include "../piece.h"
#include "gameplay_state.h"

#define PIECE_ANIMATION_DURATION 0.13

static void empty_move_list(ChessGame *game);
static void check_chess_state(ChessGame *game);
static Vec2i calc_board_relative_pos(Vec2i board_top_left, Vec2i board_size, Vec2i square,
                                     ChessColor plr_color);
static void handle_promotion_menu(ChessGame *game, bool left_btn_pressed);
static void handle_board_interaction(ChessGame *game, bool left_btn_pressed);
static void update_piece_animations(ChessGame *game, double delta_time);
static void on_resign_btn_clicked(UIComponent *c, void *data);

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
    struct ChessData *chess_data = &game->chess_data;
    struct UIData *ui_data = &game->ui_data;

    Window *w = game->renderer->window;
    int width = w->width;
    int height = w->height;
    Vec2i center = {width / 2, height / 2};

    ui_data->board_size = (Vec2i){670, 670};
    ui_data->board_pos = (Vec2i){36, center.y + ui_data->board_size.y / 2};
    ui_data->mouse_down = false;
    ui_data->mouse_pos = (Vec2f){0, 0};
    ui_data->selected_piece = NULL;
    ui_data->selected_square = (Vec2i){-1, -1};
    ui_data->promotion_menu_open = false;
    ui_data->promotion_menu_size = (Vec2i){335, 90};
    ui_data->promotion_menu_pos =
        (Vec2i){center.x - ui_data->promotion_menu_size.x / 2, center.y + ui_data->promotion_menu_size.y / 2};
    ui_data->pending_promotion_move = NULL;

    for (int i = 0; i < MAX_PIECE_ANIMATIONS; i++)
    {
        ui_data->piece_animations[i].animating_piece = NULL;
        ui_data->piece_animations[i].animation_time = 0;
    }

    chess_board_init(&chess_data->board);
    chess_data->current_turn = WHITE;
    chess_data->player_color = WHITE;
    chess_data->current_move_list = (MoveList){NULL, 0};
    chess_data->is_in_check = false;

    Color4i text_color = {255, 255, 255, 255};

    // background
    imagebox_create(game->ui, (Vec2i){0, height}, (Vec2i){width, height}, game->bg_texture,
                    (Color3i){255, 255, 255});

    // board
    imagebox_create(game->ui, ui_data->board_pos, ui_data->board_size, game->board_texture,
                    (Color3i){255, 255, 255});

    // player icon
    imagebox_create(game->ui, (Vec2i){36, 52}, (Vec2i){45, 45}, game->player_icon_texture,
                    (Color3i){255, 255, 255});
    textbox_create(game->ui, (Vec2i){81, 52}, (Vec2i){70, 15}, text_color, (Padding){0}, "You",
                   game->tertiary_font);

    // opponent icon
    imagebox_create(game->ui, (Vec2i){661, 789}, (Vec2i){45, 45}, game->player_icon_texture,
                    (Color3i){255, 255, 255});
    textbox_create(game->ui, (Vec2i){555, 789}, (Vec2i){50, 15}, text_color, (Padding){0}, "Opponent",
                   game->tertiary_font);

    // resign button
    UIComponent *resign_btn = button_create_with_texture(game->ui, (Vec2i){733, 600}, (Vec2i){80, 80},
                                                         (Padding){5, 5, 5, 5}, (Color4i){190, 190, 190, 255},
                                                         game->bg_texture2, "Resign", game->tertiary_font);
    button_set_on_click(resign_btn, on_resign_btn_clicked, game);
};

void gameplay_state_update(ChessGame *game, double delta_time)
{
    Window *w = game->renderer->window;

    struct ChessData *chess_data = &game->chess_data;
    struct UIData *ui_data = &game->ui_data;

    double xpos, ypos;
    glfwGetCursorPos(w->glfw_window, &xpos, &ypos);
    ypos = w->height - ypos; // glfw has origin at top left
    ui_data->mouse_pos = (Vec2f){xpos, ypos};

    bool left_btn_pressed = glfwGetMouseButton(w->glfw_window, GLFW_MOUSE_BUTTON_LEFT);

    Vec2i board_pos = ui_data->board_pos;
    Vec2i board_size = ui_data->board_size;
    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    if (!chess_data->is_game_over)
    {
        // promotion menu
        if (ui_data->promotion_menu_open)
        {
            handle_promotion_menu(game, left_btn_pressed);
        }
        else if (xpos > board_pos.x && xpos < board_pos.x + board_size.x &&
                 ypos > board_pos.y - board_size.y && ypos < board_pos.y)
        {
            // mouse is inside board
            handle_board_interaction(game, left_btn_pressed);
        }
        else
        {
            if (!ui_data->mouse_down && left_btn_pressed)
            {
                // user clicked outside board
                ui_data->mouse_down = true;
                ui_data->selected_square = (Vec2i){-1, -1};
                ui_data->selected_piece = NULL;
                empty_move_list(game);
            }
            else if (ui_data->mouse_down && !left_btn_pressed)
            {
                // user released mouse outside board
                ui_data->mouse_down = false;
            }
        }
    }

    update_piece_animations(game, delta_time);
};

void gameplay_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;
    struct ChessData *chess_data = &game->chess_data;
    struct UIData *ui_data = &game->ui_data;

    int width = w->width;
    int height = w->height;

    Vec2i board_size = ui_data->board_size;
    Vec2i board_pos = ui_data->board_pos;
    Vec2i center = {width / 2, height / 2};

    ChessColor plr_color = chess_data->player_color;

    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    // selected square
    if (ui_data->selected_square.x != -1)
    {

        Vec2i square_pos =
            calc_board_relative_pos(board_pos, board_size, ui_data->selected_square, plr_color);

        renderer_draw_rect(r, (Color4i){255, 210, 111, 225}, square_pos,
                           (Vec2i){ceil(square_size.x), ceil(square_size.y)});
    }

    ChessPiece *selected_piece = ui_data->selected_piece;

    // check indicator
    if (chess_data->is_in_check)
    {
        Vec2i king_pos = chess_data->current_turn == WHITE ? chess_data->board.white_king_pos
                                                           : chess_data->board.black_king_pos;
        Vec2i square_pos = calc_board_relative_pos(board_pos, board_size, king_pos, plr_color);

        renderer_draw_rect(r, (Color4i){255, 40, 40, 255}, square_pos,
                           (Vec2i){ceil(square_size.x), ceil(square_size.y)});
    }

    // selected piece moves
    if (selected_piece && chess_data->current_move_list.n_moves > 0)
    {
        for (int i = 0; i < chess_data->current_move_list.n_moves; i++)
        {
            ChessMove move = chess_data->current_move_list.moves[i];
            bool is_capture = chess_data->board.squares[move.to.x][move.to.y] != NULL;
            bool is_en_passant = move.type == EN_PASSANT;

            Vec2i square_pos = calc_board_relative_pos(board_pos, board_size, move.to, plr_color);
            Vec2i circle_size = (Vec2i){ceil(square_size.x * 0.3), ceil(square_size.y * 0.3)};
            Vec2i circle_pos = (Vec2i){square_pos.x + square_size.x / 2 - circle_size.x / 2,
                                       square_pos.y - square_size.y / 2 + circle_size.y / 2};

            if (is_capture)
            {
                renderer_draw_rect(r, (Color4i){245, 51, 51, 255}, square_pos,
                                   (Vec2i){ceil(square_size.x), ceil(square_size.y)});
            }
            else
            {
                renderer_draw_rect_tex_colored(
                    r, is_en_passant ? (Color4i){245, 51, 51, 150} : (Color4i){16, 16, 16, 50},
                    game->circle_texture, circle_pos, circle_size);
            }
        }
    }

    // pieces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece *piece = chess_data->board.squares[i][j];

            bool is_currently_animating = false;
            for (int k = 0; k < MAX_PIECE_ANIMATIONS; k++)
            {
                if (piece == ui_data->piece_animations[k].animating_piece)
                {
                    is_currently_animating = true;
                    break;
                }
            }

            if (piece != NULL)
            {
                if ((ui_data->mouse_down && piece == ui_data->selected_piece) || is_currently_animating)
                    continue;

                Texture tex = game->piece_textures[piece->type + (piece->color == BLACK ? 6 : 0)];
                Vec2i piece_pos = calc_board_relative_pos(board_pos, board_size, (Vec2i){i, j}, plr_color);

                renderer_draw_rect_tex(r, tex, piece_pos, (Vec2i){square_size.x, square_size.y});
            }
        }
    }

    // currently moving piece
    for (int i = 0; i < MAX_PIECE_ANIMATIONS; i++)
    {
        if (ui_data->piece_animations[i].animating_piece)
        {
            Vec2i from = calc_board_relative_pos(board_pos, board_size,
                                                 ui_data->piece_animations[i].animating_from, plr_color);
            Vec2i to = calc_board_relative_pos(board_pos, board_size,
                                               ui_data->piece_animations[i].animating_to, plr_color);

            Vec2i piece_pos = (Vec2i){from.x + (to.x - from.x) * ui_data->piece_animations[i].animation_time /
                                                   PIECE_ANIMATION_DURATION,
                                      from.y + (to.y - from.y) * ui_data->piece_animations[i].animation_time /
                                                   PIECE_ANIMATION_DURATION};

            Texture tex =
                game->piece_textures[ui_data->piece_animations[i].animating_piece->type +
                                     (ui_data->piece_animations[i].animating_piece->color == BLACK ? 6 : 0)];

            renderer_draw_rect_tex(r, tex, piece_pos, (Vec2i){square_size.x, square_size.y});
        }
    }

    // dragged piece
    if (ui_data->mouse_down && selected_piece)
    {
        Texture tex = game->piece_textures[selected_piece->type + (selected_piece->color == BLACK ? 6 : 0)];
        Vec2i piece_pos =
            (Vec2i){ui_data->mouse_pos.x - square_size.x / 2, ui_data->mouse_pos.y + square_size.y / 2};

        renderer_draw_rect_tex(r, tex, piece_pos, (Vec2i){square_size.x, square_size.y});
    }

    // promotion menu
    if (ui_data->promotion_menu_open)
    {
        Vec2i menu_size = ui_data->promotion_menu_size;
        Vec2i menu_pos = ui_data->promotion_menu_pos;
        renderer_draw_rect_tex(r, game->menu_bg_texture, menu_pos, menu_size);

        PieceType promotions[] = {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};
        for (int i = 0; i < 4; i++)
        {
            int piece_tex_idx = promotions[i];
            Vec2i pos = (Vec2i){menu_pos.x + i * square_size.x, menu_pos.y - 3};
            Texture tex = game->piece_textures[piece_tex_idx + (plr_color == BLACK ? 6 : 0)];
            renderer_draw_rect_tex(r, tex, pos, (Vec2i){square_size.x, square_size.y});
        }
    }

    // game over modal
    if (chess_data->is_game_over)
    {
        GameEndReason end_reason = chess_data->game_end_reason;
        bool is_draw = end_reason == STALEMATE;
        bool is_resignation = end_reason == RESIGNATION;
        bool is_plr_winner =
            !is_draw && !is_resignation && chess_data->player_color != chess_data->current_turn;

        Vec2i menu_size = (Vec2i){335, 130};
        Vec2i menu_pos = (Vec2i){center.x - ui_data->promotion_menu_size.x / 2,
                                 center.y + ui_data->promotion_menu_size.y / 2};
        renderer_draw_rect_tex(r, game->menu_bg_texture, menu_pos, menu_size);

        char *reason_text =
            end_reason == CHECKMATE ? "Checkmate" : (end_reason == RESIGNATION ? "Resignation" : "Stalemate");
        char *end_text = is_plr_winner ? "  You win!  " : (is_draw ? "Draw" : "Game over");

        Color4i text_color = {255, 255, 255, 255};
        renderer_draw_text_with_width(r, end_text, game->primary_font,
                                      (Vec2i){menu_pos.x + 20, menu_pos.y - 16}, menu_size.x - 40,
                                      (Color4i){255, 255, 255, 255});
        renderer_draw_text_with_width(
            r, reason_text, game->secondary_font, (Vec2i){menu_pos.x + 40, menu_pos.y - 73}, menu_size.x - 80,
            is_plr_winner ? (Color4i){255, 210, 111, 225} : (Color4i){245, 51, 51, 255});
    }
};

void gameplay_state_cleanup(ChessGame *game) { ui_destroy_all(game->ui); };

static Vec2i calc_board_relative_pos(Vec2i board_top_left, Vec2i board_size, Vec2i square,
                                     ChessColor plr_color)
{
    return (Vec2i){board_top_left.x + (plr_color == BLACK ? (7 - square.x) : square.x) * board_size.x / 8,
                   board_top_left.y - (plr_color == WHITE ? (7 - square.y) : square.y) * board_size.y / 8};
}

static void empty_move_list(ChessGame *game)
{
    struct ChessData *chess_data = &game->chess_data;
    free(chess_data->current_move_list.moves);
    chess_data->current_move_list = (MoveList){NULL, 0};
}

static void check_chess_state(ChessGame *game)
{
    struct ChessData *chess_data = &game->chess_data;
    // check
    if (chess_board_is_in_check(&chess_data->board, chess_data->current_turn))
    {
        if (chess_board_is_in_checkmate(&chess_data->board, chess_data->current_turn, false))
        {
            // checkmate
            printf("Checkmate\n");
            chess_data->is_game_over = true;
            chess_data->game_end_reason = CHECKMATE;
        }
        else
        {
            // check
            chess_data->is_in_check = true;
            printf("Check\n");
        }
    }
    else
    {
        if (chess_data->is_in_check)
            chess_data->is_in_check = false;

        if (chess_board_is_in_stalemate(&chess_data->board, chess_data->current_turn, false))
        {
            // stalemate
            printf("Stalemate\n");
            chess_data->is_game_over = true;
            chess_data->game_end_reason = STALEMATE;
        }
    }
}

static void handle_promotion_menu(ChessGame *game, bool left_btn_pressed)
{
    struct ChessData *chess_data = &game->chess_data;
    struct UIData *ui_data = &game->ui_data;

    double xpos = ui_data->mouse_pos.x;
    double ypos = ui_data->mouse_pos.y;

    Vec2i menu_size = ui_data->promotion_menu_size;
    Vec2i menu_pos = ui_data->promotion_menu_pos;

    Vec2i board_size = ui_data->board_size;
    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    if (xpos >= menu_pos.x && xpos <= menu_pos.x + menu_size.x && ypos >= menu_pos.y - menu_size.y &&
        ypos <= menu_pos.y && !ui_data->mouse_down && left_btn_pressed)
    {
        int x = (int)((xpos - menu_pos.x) / square_size.x);
        PieceType promoted_types[] = {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};
        PieceType promoted_type = promoted_types[x];

        ChessMove *move = ui_data->pending_promotion_move;
        chess_board_promote_pawn(&chess_data->board, ui_data->pawn_to_promote, move, promoted_type);
        chess_data->current_turn = chess_data->current_turn == WHITE ? BLACK : WHITE;
        check_chess_state(game);

        empty_move_list(game);
        ui_data->promotion_menu_open = false;
        ui_data->pawn_to_promote = NULL;

        ui_data->piece_animations[0].animating_piece = ui_data->selected_piece;
        ui_data->piece_animations[0].animating_from = ui_data->selected_square;
        ui_data->piece_animations[0].animating_to = move->to;
        ui_data->piece_animations[0].animation_time = 0;
    }
    else if (ui_data->mouse_down && !left_btn_pressed)
    {
        ui_data->mouse_down = false;
    }
}

static void handle_board_interaction(ChessGame *game, bool left_btn_pressed)
{
    struct ChessData *chess_data = &game->chess_data;
    struct UIData *ui_data = &game->ui_data;

    double xpos = ui_data->mouse_pos.x;
    double ypos = ui_data->mouse_pos.y;

    Vec2i board_pos = ui_data->board_pos;
    Vec2i board_size = ui_data->board_size;
    Vec2f square_size = {(float)board_size.x / 8, (float)board_size.y / 8};

    int x = ((xpos - (float)board_pos.x) / square_size.x);
    int y = 7 - (int)(((float)board_pos.y - ypos) / square_size.y);

    if (chess_data->player_color == BLACK)
    {
        x = 7 - x;
        y = 7 - y;
    }

    if (!ui_data->mouse_down && left_btn_pressed)
    {
        // mouse btn down
        ui_data->mouse_down = true;

        bool move_made = false;

        if (ui_data->selected_piece)
        {
            for (int i = 0; i < chess_data->current_move_list.n_moves; i++)
            {
                ChessMove *move = &chess_data->current_move_list.moves[i];
                if (move->to.x == x && move->to.y == y)
                {
                    if (move->type == PROMOTION)
                    {
                        ui_data->promotion_menu_open = true;
                        ui_data->pending_promotion_move = move;
                        ui_data->pawn_to_promote = ui_data->selected_piece;
                    }
                    else
                    {
                        chess_board_make_move(&chess_data->board, ui_data->selected_piece, move);
                        chess_data->current_turn = chess_data->current_turn == WHITE ? BLACK : WHITE;
                        check_chess_state(game);

                        // animate piece
                        ui_data->piece_animations[0].animating_piece = ui_data->selected_piece;
                        ui_data->piece_animations[0].animating_from = ui_data->selected_square;
                        ui_data->piece_animations[0].animating_to = move->to;
                        ui_data->piece_animations[0].animation_time = 0;

                        if (move->type == CASTLE_KINGSIDE || move->type == CASTLE_QUEENSIDE)
                        {
                            Vec2i rook_from = move->type == CASTLE_KINGSIDE ? (Vec2i){7, move->to.y}
                                                                            : (Vec2i){0, move->to.y};
                            Vec2i rook_to = move->type == CASTLE_KINGSIDE ? (Vec2i){5, move->to.y}
                                                                          : (Vec2i){3, move->to.y};
                            ChessPiece *rook = chess_data->board.squares[rook_to.x][rook_to.y];

                            // animate rook as well if castling
                            ui_data->piece_animations[1].animating_piece = rook;
                            ui_data->piece_animations[1].animating_from = rook_from;
                            ui_data->piece_animations[1].animating_to = rook_to;
                            ui_data->piece_animations[1].animation_time = 0;
                        }

                        empty_move_list(game);
                    }

                    move_made = true;
                    break;
                }
            }
        }
        ui_data->selected_square = (Vec2i){-1, -1};
        ui_data->selected_piece = NULL;

        if (!move_made && chess_data->board.squares[x][y] &&
            chess_data->board.squares[x][y]->color == chess_data->current_turn)
        {
            // clicked on a piece
            ui_data->selected_square = (Vec2i){x, y};
            ui_data->selected_piece = chess_data->board.squares[x][y];
            chess_data->current_move_list =
                generate_legal_moves(ui_data->selected_piece, (Vec2i){x, y}, &chess_data->board);
        }
    }
    else if (ui_data->mouse_down && !left_btn_pressed)
    {
        // clicked and released
        ui_data->mouse_down = false;

        if (ui_data->selected_piece)
        {
            // move piece
            for (int i = 0; i < chess_data->current_move_list.n_moves; i++)
            {
                ChessMove *move = &chess_data->current_move_list.moves[i];
                if (move->to.x == x && move->to.y == y)
                {
                    if (move->type == PROMOTION)
                    {
                        ui_data->promotion_menu_open = true;
                        ui_data->pending_promotion_move = move;
                        ui_data->pawn_to_promote = ui_data->selected_piece;
                    }
                    else
                    {
                        chess_board_make_move(&chess_data->board, ui_data->selected_piece, move);
                        chess_data->current_turn = chess_data->current_turn == WHITE ? BLACK : WHITE;
                        check_chess_state(game);

                        empty_move_list(game);
                        ui_data->selected_square = (Vec2i){-1, -1};
                        ui_data->selected_piece = NULL;
                    }

                    break;
                }
            }
        }
    }
}

static void update_piece_animations(ChessGame *game, double delta_time)
{
    struct UIData *ui_data = &game->ui_data;

    for (int i = 0; i < MAX_PIECE_ANIMATIONS; i++)
    {
        if (ui_data->piece_animations[i].animating_piece)
        {
            ui_data->piece_animations[i].animation_time += delta_time;

            if (ui_data->piece_animations[i].animation_time >= PIECE_ANIMATION_DURATION)
            {
                ui_data->piece_animations[i].animating_piece = NULL;
                ui_data->piece_animations[i].animation_time = 0;
            }
        }
    }
}

static void on_resign_btn_clicked(UIComponent *c, void *data)
{
    ChessGame *game = (ChessGame *)data;
    struct ChessData *chess_data = &game->chess_data;

    chess_data->is_game_over = true;
    chess_data->game_end_reason = RESIGNATION;

    printf("Player resigned\n");
}