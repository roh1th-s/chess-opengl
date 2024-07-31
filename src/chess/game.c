#include <synchapi.h>
#include <time.h>

#include "../gfx/font.h"
#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "../gfx/window.h"
#include "../types.h"
#include "game.h"
#include "states/gameplay_state.h"
#include "states/menu_state.h"

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
    LOAD_TEXTURE(self->player_icon_texture, "res/textures/player.png");

    char colors[2] = {'w', 'b'};
    char pieces[6] = {'p', 'n', 'b', 'r', 'k', 'q'};
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            char path[50];
            sprintf(path, "res/textures/pieces/%c%c.png", colors[i], pieces[j]);
            LOAD_TEXTURE(self->piece_textures[i * 6 + j], path);
        }
    }

    LOAD_FONT(self->primary_font, "res/fonts/Montserrat-SemiBold.ttf", 200);
    LOAD_FONT(self->secondary_font, "res/fonts/Montserrat-Regular.ttf", 84);

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

    double last_time = glfwGetTime();
    double current_time;
    double delta_time;

    // game loop
    while (!window_should_close(w))
    {
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        game_update(self, delta_time);

        game_render(self);

        // char fps[10];
        // sprintf(fps, "%.2f", 1.0 / deltaTime);
        // renderer_draw_text(r, "FPS: ", self->secondary_font, (Vec2i){0, w->height}, (Vec2i){100, 30},
        //                    (Color3i){255, 255, 255});
        // renderer_draw_text(r, fps, self->secondary_font, (Vec2i){100, w->height}, (Vec2i){150, 30},
        //                    (Color3i){255, 255, 255});

        window_swap_buffers(w);

        window_poll_events();

        Sleep(1);
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

void game_update(ChessGame *self, double delta_time)
{
    self->states[self->current_state]->update(self, delta_time);
    ui_update(self->ui);
}

void game_render(ChessGame *self)
{
    Renderer *r = self->renderer;
    Window *w = r->window;

    renderer_clear_window(r, (Color3i){0, 0, 0});

    ui_render(self->ui);

    self->states[self->current_state]->render(self);

    renderer_update(r);
}