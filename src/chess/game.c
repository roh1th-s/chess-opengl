#include "game.h"
#include "../gfx/font.h"
#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../gfx/window.h"
#include "../types.h"
#include "button.h"

static void play_button_click(UIComponent *c) { printf("Start\n"); }

void menu_state_init(ChessGame *game)
{
    Window *w = game->renderer->window;
    int width = w->width;
    int height = w->height;
    UIComponent *button =
        button_create(game->ui, (Vec2i){width / 2 - 100, height / 2 - 40}, (Vec2i){200, 90},
                      (Padding){15, 10, 15, 10}, (Color3i){110, 38, 14}, "Play", game->primary_font);
    Button *b = (Button *)button->component;
    b->on_click = play_button_click;
}

void menu_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;

    int width = w->width;
    int height = w->height;

    int text_width = 500;
    Vec2i center = {width / 2 - text_width / 2, height / 2 + 150};

    renderer_draw_text_with_width(r, "Chess!", game->primary_font, center, text_width,
                                  (Color3i){255, 255, 255});
}

void game_init(ChessGame *self, Renderer *r)
{
    self->renderer = r;

    self->ui = (UIManager *)malloc(sizeof(UIManager));
    ui_init(self->ui, r);

    Texture t;
    texture_create(&t, "res/textures/board.png");
    self->board_texture = t;

    Font *inter = (Font *)malloc(sizeof(Font));
    font_init(inter, "res/fonts/Inter-Regular.ttf", 200);
    self->primary_font = inter;

    self->state = MENU_STATE;
    menu_state_init(self);
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

    ui_update(self->ui);
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

    ui_render(self->ui);

    renderer_update(r);
    window_update(w);
}