#include "menu_state.h"
#include "../../gfx/renderer.h"
#include "../../ui/button.h"
#include "../../ui/imagebox.h"
#include "../../ui/textbox.h"
#include "../../ui/ui.h"
#include "../game.h"

static void play_button_click(UIComponent *c, void *data)
{
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
    Vec2i center = {width / 2, height / 2};

    // background
    imagebox_create(game->ui, (Vec2i){0, height}, (Vec2i){width, height}, game->bg_texture,
                    (Color3i){255, 255, 255});

    // title text
    Vec2i tb_size = {700, 140};
    textbox_create(game->ui, (Vec2i){center.x - tb_size.x / 2, center.y + tb_size.y / 2 + 100}, tb_size,
                   (Padding){0}, "Chess", game->primary_font);

    // play button
    Vec2i btn_size = {200, 90};
    Color3i btn_color = {190, 190, 190};
    UIComponent *button = button_create_with_texture(
        game->ui, (Vec2i){center.x - btn_size.x / 2, center.y + btn_size.y / 2 - 85}, btn_size,
        (Padding){18, 10, 18, 10}, btn_color, game->bg_texture2, "Play", game->secondary_font);
    button_set_on_click(button, play_button_click, game);
}

void menu_state_update(ChessGame *game, double delta_time) {}

void menu_state_render(ChessGame *game) {}

void menu_state_cleanup(ChessGame *game) { ui_destroy_all(game->ui); }