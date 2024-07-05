#include "menu_state.h"
#include "../../ui/button.h"
#include "../../ui/ui.h"
#include "../../gfx/renderer.h"
#include "../game.h"

static void play_button_click(UIComponent *c, void *data) { 
    printf("Start game\n"); 
    ChessGame *game = (ChessGame *)data;
    game_switch_to_state(game, GAMEPLAY_STATE);
}

GameState *menu_state_init()
{
    GameState *self = (GameState *)malloc(sizeof(GameState));

    self->type = MENU_STATE;
    self->setup = menu_state_setup;
    self->update = menu_state_update;
    self->render = menu_state_render;
    self->cleanup = menu_state_cleanup;

    return self;
}

void menu_state_setup(ChessGame *game)
{
    Window *w = game->renderer->window;
    int width = w->width;
    int height = w->height;
    UIComponent *button = button_create_with_texture(
        game->ui, (Vec2i){width / 2 - 100, height / 2 - 40}, (Vec2i){200, 90}, (Padding){18, 10, 18, 10},
        (Color3i){190, 190, 190}, game->bg_texture2, "Play", game->secondary_font);
    Button *b = (Button *)button->component;
    button_set_on_click(button, play_button_click, game);
}

void menu_state_update(ChessGame *game)
{
    // nothing to update
}

void menu_state_render(ChessGame *game)
{
    Renderer *r = game->renderer;
    Window *w = r->window;

    int width = w->width;
    int height = w->height;

    int text_width = 500;
    Vec2i center = {width / 2 - text_width / 2, height / 2 + 150};

    renderer_draw_rect_tex(r, game->bg_texture, (Vec2i){0, height}, (Vec2i){width, height});

    renderer_draw_text_with_width(r, "Chess", game->primary_font, center, text_width,
                                  (Color3i){255, 255, 255});
}

void menu_state_cleanup(ChessGame *game) { ui_destroy_all(game->ui); }