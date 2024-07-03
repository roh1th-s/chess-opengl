#include "game.h"
#include "../types.h"
#include "../gfx/window.h"
#include "../gfx/renderer.h"
#include "../gfx/font.h"
#include "../gfx/texture.h"

void menu_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;

    int width = w->width;
    int height = w->height;

    int text_width = 500;
    Vec2i center = {width / 2 - text_width / 2, height / 2 + 100};

    renderer_draw_text_with_width(r, "chess-opengl", game->primary_font, center, text_width, (Color3i){255, 255, 255});
}

void game_init(ChessGame *self, Renderer *r)
{
    self->renderer = r;
    self->state = MENU_STATE;

    Texture t;
    texture_create(&t, "res/textures/board.png");
    self->board_texture = t;

    Font *inter = (Font *)malloc(sizeof(Font));
    font_init(inter, "res/fonts/Inter-Regular.ttf", 96);
    self->primary_font = inter;
}

void game_start(ChessGame *self)
{
    Renderer *r = self->renderer;
    Window *w = r->window;

    // game loop
    while (!window_should_close(w))
    {
        renderer_clear_window(r, (Color3i){0, 0, 0});
        game_update(self);
        game_render(self);
    }
}

void game_update(ChessGame *self)
{
    switch (self->state)
    {
    case MENU_STATE:
        break;
    case LEVEL_SELECT_STATE:
        break;
    case GAMEPLAY_STATE:
        break;
    }
}

void game_render(ChessGame *self)
{
    Renderer *r = self->renderer;
    Window *w = r->window;

    switch (self->state)
    {
    case MENU_STATE:
        menu_state_render(self);
        break;
    case LEVEL_SELECT_STATE:
        break;
    case GAMEPLAY_STATE:
        break;
    }

    renderer_update(r);
    window_update(w);
}