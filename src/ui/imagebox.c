#include <GLFW/glfw3.h>

#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "imagebox.h"
#include "ui.h"

UIComponent *imagebox_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Texture tex, Color3i color)
{
    UIComponent *self = (UIComponent *)malloc(sizeof(UIComponent));

    Imagebox *imagebox = (Imagebox *)malloc(sizeof(Imagebox));
    imagebox->position = pos;
    imagebox->size = size;
    imagebox->texture = tex;
    imagebox->color = color;

    self->component = imagebox;
    self->type = IMAGEBOX;
    self->enabled = true;

    self->render = imagebox_render;
    self->update = imagebox_update;
    self->destroy = imagebox_destroy;

    ui_add_component(ui_manager, self);

    return self;
}

void imagebox_render(UIComponent *self, UIManager *ui_manager)
{
    Imagebox *imagebox = (Imagebox *)self->component;
    Renderer *renderer = ui_manager->renderer;

    Vec2i ib_size = imagebox->size;
    Vec2i ib_pos = imagebox->position;

    Color4i ib_color = {imagebox->color.r, imagebox->color.g, imagebox->color.b, 255};
    renderer_draw_rect_tex_colored(renderer, ib_color, imagebox->texture, ib_pos, ib_size);
}

void imagebox_update(UIComponent *self, UIManager *ui_manager) {}

void imagebox_destroy(UIComponent *self, UIManager *ui_manager) { free(self->component); }
