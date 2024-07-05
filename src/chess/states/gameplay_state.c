#include "gameplay_state.h"
#include "../../gfx/renderer.h"
#include "../game.h"

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

void gameplay_state_setup(ChessGame *game) {

};

void gameplay_state_update(ChessGame *game) {

};

void gameplay_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;

    int width = w->width;
    int height = w->height;
    renderer_draw_rect_tex(r, game->bg_texture, (Vec2i){0, height}, (Vec2i){width, height});

    Vec2i board_size = {700, 700};
    Vec2i center = {(width - board_size.x) / 2, (height + board_size.y) / 2};
    renderer_draw_rect_tex(r, game->board_texture, center, board_size);
};

void gameplay_state_cleanup(ChessGame *game) {

};