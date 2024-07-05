#if !defined(BUTTON_H)
#define BUTTON_H

#include <stdbool.h>

#include "../gfx/font.h"
#include "../gfx/texture.h"
#include "../types.h"
#include "ui.h"

typedef struct
{
    float top;
    float right;
    float bottom;
    float left;
} Padding;

typedef void (*ButtonCB)(UIComponent *, void *);
typedef struct
{
    Vec2i position;
    Vec2i size;
    Padding padding;
    Color3i color;
    Texture texture;
    bool use_texture;
    const char *text;
    Font *font;

    bool mouse_down;
    bool is_hovered;

    void *on_click_data;
    ButtonCB on_click;
} Button;

UIComponent *button_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding, Color3i color,
                           const char *text, Font *font);
UIComponent *button_create_with_texture(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding,
                                        Color3i color, Texture texture, const char *text, Font *font);
void button_set_on_click(UIComponent *self, ButtonCB on_click, void *data);
void button_render(UIComponent *self, UIManager *ui_manager);
void button_update(UIComponent *self, UIManager *ui_manager);
void button_destroy(UIComponent *self, UIManager *ui_manager);

#endif