#include "button.h"
#include "ui.h"

UIComponent *button_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding, Color3i color, const char *text,
                           Font *font)
{
    UIComponent *self = (UIComponent *)malloc(sizeof(UIComponent));

    Button *button = (Button *)malloc(sizeof(Button));
    button->position = pos;
    button->size = size;
    button->padding = padding;
    button->color = color;
    button->text = text;
    button->font = font;
    button->use_texture = false;
    button->mouse_down = false;

    self->component = button;
    self->type = BUTTON;
    self->enabled = true;

    self->render = button_render;
    self->update = button_update;
    self->destroy = button_destroy;

    ui_add_component(ui_manager, self);

    return self;
}

void button_set_texture(UIComponent *self, Texture texture)
{
    Button *button = (Button *)self->component;
    button->texture = texture;
    button->use_texture = true;
}

void button_render(UIComponent *self, UIManager *ui_manager)
{
    Button *button = (Button *)self->component;
    Renderer *renderer = ui_manager->renderer;

    renderer_draw_rect(renderer, button->color, button->position, button->size);

    Vec2i text_pos =
        (Vec2i){button->position.x + button->padding.left, button->position.y - button->padding.top};
    Vec2i text_size = (Vec2i){button->size.x - (button->padding.right + button->padding.left),
                              button->size.y - (button->padding.bottom + button->padding.top)};
    renderer_draw_text(renderer, button->text, button->font, text_pos, text_size,
                       (Color3i){255, 255, 255});
}

void button_update(UIComponent *self, UIManager *ui_manager)
{
    Button *button = (Button *)self->component;
    Renderer *renderer = ui_manager->renderer;

    Vec2f mouse_pos = ui_manager->mouse_state.pos;
    Vec2f button_pos = (Vec2f){button->position.x, button->position.y};
    Vec2f button_size = (Vec2f){button->size.x, button->size.y};

    // check if mouse is inside button
    if (mouse_pos.x >= button_pos.x && mouse_pos.x <= button_pos.x + button_size.x &&
        mouse_pos.y <= button_pos.y && mouse_pos.y >= button_pos.y - button_size.y)
    {
        if (!button->mouse_down && ui_manager->mouse_state.left_button)
        {
            // user clicked inside button
            button->mouse_down = true;
        }
        else if (button->mouse_down && !ui_manager->mouse_state.left_button)
        {
            // user released mouse inside button
            button->on_click(self);
            button->mouse_down = false;
        }
    }
    else
    {
        if (button->mouse_down && !ui_manager->mouse_state.left_button)
        {
            // user released mouse outside button
            button->mouse_down = false;
        }
    }
}

void button_destroy(UIComponent *self, UIManager *ui_manager)
{
    Button *button = (Button *)self->component;

    free(button);
}
