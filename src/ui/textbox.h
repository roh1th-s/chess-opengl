#if !defined(TEXTBOX_H)
#define TEXTBOX_H

#include <stdbool.h>

#include "../gfx/font.h"
#include "../types.h"
#include "ui.h"
#include "ui_types.h"

typedef struct
{
    Vec2i position;
    Vec2i size;
    Padding padding;
    const char *text;
    Font *font;
} Textbox;

UIComponent *textbox_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding, const char *text,
                            Font *font);
void textbox_render(UIComponent *self, UIManager *ui_manager);
void textbox_update(UIComponent *self, UIManager *ui_manager);
void textbox_destroy(UIComponent *self, UIManager *ui_manager);

#endif