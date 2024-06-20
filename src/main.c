#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "gfx/window.h"
#include "gfx/renderer.h"
#include "types.h"

int main()
{   
    Window w;
    window_init(&w, 800, 600, "Chess");

    Renderer r;
    renderer_init(&r, &w);

    while (!window_should_close(&w))
    {
        renderer_clear_window(&r, (Color3f){0.2f, 0.3f, 0.3f});

        renderer_draw_rect(&r, (Color3i){209, 146, 0}, (Vec2i){0, 300}, (Vec2i){400, 300});

        window_update(&w);
        renderer_update(&r);
    }

    window_destroy(&w);
    renderer_terminate(&r);

    return 0;
}
