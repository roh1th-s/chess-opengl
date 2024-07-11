#include <GLFW/glfw3.h>

#include "../gfx/font.h"
#include "../gfx/renderer.h"
#include "textbox.h"
#include "ui.h"

UIComponent *textbox_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding, const char *text,
                            Font *font)
{
    UIComponent *self = (UIComponent *)malloc(sizeof(UIComponent));

    Textbox *textbox = (Textbox *)malloc(sizeof(Textbox));
    textbox->position = pos;
    textbox->size = size;
    textbox->padding = padding;
    textbox->text = text;
    textbox->font = font;

    self->component = textbox;
    self->type = TEXTBOX;
    self->enabled = true;

    self->render = textbox_render;
    self->update = textbox_update;
    self->destroy = textbox_destroy;

    ui_add_component(ui_manager, self);

    return self;
}

void textbox_render(UIComponent *self, UIManager *ui_manager)
{
    Textbox *textbox = (Textbox *)self->component;
    Renderer *renderer = ui_manager->renderer;

    Vec2i tb_size = textbox->size;
    Vec2i tb_pos = textbox->position;
    Padding tb_padding = textbox->padding;

    // center text
    Vec2i text_dim = get_text_dimensions(textbox->font, textbox->text);
    unsigned int textWidth, textHeight;
    int textX, textY;

    if (tb_size.x <= tb_size.y)
    {
        textWidth = tb_size.x - (tb_padding.right + tb_padding.left);
        textHeight = text_dim.y * ((float)textWidth / text_dim.x);
        // textHeight = textHeight > tb_size.y ? tb_size.y : textHeight; // squish text
        textX = tb_pos.x + tb_padding.left;
        textY =
            tb_pos.y - tb_padding.top - (tb_size.y - (tb_padding.top + tb_padding.bottom) - textHeight) / 2;
    }
    else
    {
        textHeight = tb_size.y - (tb_padding.top + tb_padding.bottom);
        textWidth = text_dim.x * ((float)textHeight / text_dim.y);
        // textWidth = textWidth > tb_size.x ? tb_size.x : textWidth; // squish text
        textX =
            tb_pos.x + tb_padding.left + (tb_size.x - (tb_padding.right + tb_padding.left) - textWidth) / 2;
        textY = tb_pos.y - tb_padding.top;
    }

    textX = (textX - tb_pos.x) < 0 ? tb_pos.x : textX; // overflow
    textY = (textY - tb_pos.y) < 0 ? tb_pos.y : textY;

    renderer_draw_text(renderer, textbox->text, textbox->font, (Vec2i){textX, textY},
                       (Vec2i){textWidth, textHeight}, (Color3i){255, 255, 255});
}

void textbox_update(UIComponent *self, UIManager *ui_manager) {}

void textbox_destroy(UIComponent *self, UIManager *ui_manager) { free(self->component); }
