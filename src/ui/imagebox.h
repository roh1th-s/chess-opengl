#if !defined(IMAGEBOX_H)
#define IMAGEBOX_H

#include <stdbool.h>

#include "../gfx/texture.h"
#include "../types.h"
#include "ui.h"
#include "ui_types.h"

typedef struct
{
    Vec2i position;
    Vec2i size;
    Texture texture;
    Color3i color;
} Imagebox;

UIComponent *imagebox_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Texture tex, Color3i color);
void imagebox_render(UIComponent *self, UIManager *ui_manager);
void imagebox_update(UIComponent *self, UIManager *ui_manager);
void imagebox_destroy(UIComponent *self, UIManager *ui_manager);

#endif