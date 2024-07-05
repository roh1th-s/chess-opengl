#include "game.h"
#include "../gfx/font.h"
#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../gfx/window.h"
#include "../types.h"
#include "states/gameplay_state.h"
#include "states/menu_state.h"
#include "time.h"

void game_init(ChessGame *self, Renderer *r)
{
    self->renderer = r;

    self->ui = (UIManager *)malloc(sizeof(UIManager));
    ui_init(self->ui, r);

    float startTime, endTime;
    startTime = (float)clock() / CLOCKS_PER_SEC;

    LOAD_TEXTURE(self->board_texture, "res/textures/board.png");
    LOAD_TEXTURE(self->bg_texture, "res/textures/walnut.jpg");
    LOAD_TEXTURE(self->bg_texture2, "res/textures/bg.jpg");

    LOAD_FONT(self->primary_font, "res/fonts/Montserrat-SemiBold.ttf", 200);
    LOAD_FONT(self->secondary_font, "res/fonts/Montserrat-Regular.ttf", 200);

    endTime = (float)clock() / CLOCKS_PER_SEC;
    printf("Loaded assets in %fs\n", endTime - startTime);

    self->current_state = INIT_STATE;
    self->states[MENU_STATE] = menu_state_init();
    self->states[GAMEPLAY_STATE] = gameplay_state_init();
}

void game_start(ChessGame *self)
{
    Renderer *r = self->renderer;
    Window *w = r->window;

    game_switch_to_state(self, MENU_STATE);

    // game loop
    while (!window_should_close(w))
    {
        renderer_clear_window(r, (Color3i){0, 0, 0});
        game_update(self);
        game_render(self);
    }
}

void game_switch_to_state(ChessGame *self, GameStateType state)
{
    if (self->current_state == state)
        return;

    if (self->current_state != INIT_STATE)
        self->states[self->current_state]->cleanup(self);

    self->current_state = state;
    self->states[state]->setup(self);
}

void game_update(ChessGame *self)
{
    self->states[self->current_state]->update(self);
    ui_update(self->ui);
}

void game_render(ChessGame *self)
{
    Renderer *r = self->renderer;
    Window *w = r->window;

    self->states[self->current_state]->render(self);

    ui_render(self->ui);

    renderer_update(r);
    window_update(w);
}