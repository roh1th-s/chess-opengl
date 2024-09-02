#include <GLFW/glfw3.h>

#include "../gfx/font.h"
#include "../gfx/renderer.h"
#include "../gfx/texture.h"
#include "button.h"
#include "ui.h"

UIComponent *button_create(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding, Color4i color,
                           const char *text, Font *font)
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

UIComponent *button_create_with_texture(UIManager *ui_manager, Vec2i pos, Vec2i size, Padding padding,
                                        Color4i color, Texture texture, const char *text, Font *font)
{
    UIComponent *self = button_create(ui_manager, pos, size, padding, color, text, font);
    Button *button = (Button *)self->component;
    button->texture = texture;
    button->use_texture = true;

    return self;
}

void button_set_on_click(UIComponent *self, ButtonCB on_click, void *data)
{
    Button *button = (Button *)self->component;
    button->on_click = on_click;
    button->on_click_data = data;
}

void button_render(UIComponent *self, UIManager *ui_manager)
{
    Button *button = (Button *)self->component;
    Renderer *renderer = ui_manager->renderer;

    Vec2i btn_size = button->size;
    Vec2i btn_pos = button->position;
    Color4i btn_color = button->color;
    Padding btn_padding = button->padding;

    if (button->use_texture)
        renderer_draw_rect_tex_colored(renderer, btn_color, button->texture, btn_pos, btn_size);
    else
        renderer_draw_rect(renderer, btn_color, btn_pos, btn_size);

    // center text inside button
    Vec2i text_dim = get_text_dimensions(button->font, button->text);
    unsigned int textWidth, textHeight, textX, textY;

    if (btn_size.x <= btn_size.y)
    {
        textWidth = btn_size.x - (btn_padding.right + btn_padding.left);
        textHeight = text_dim.y * ((float)textWidth / text_dim.x);
        textX = btn_pos.x + btn_padding.left;
        textY = btn_pos.y - btn_padding.top -
                (btn_size.y - (btn_padding.top + btn_padding.bottom) - textHeight) / 2;
    }
    else
    {
        textHeight = btn_size.y - (btn_padding.top + btn_padding.bottom);
        textWidth = text_dim.x * ((float)textHeight / text_dim.y);
        textX = btn_pos.x + btn_padding.left +
                (btn_size.x - (btn_padding.right + btn_padding.left) - textWidth) / 2;
        textY = btn_pos.y - btn_padding.top;
    }

    textX = (textX - btn_pos.x) < 0 ? btn_pos.x : textX; // overflow
    textY = (textY - btn_pos.y) < 0 ? btn_pos.y : textY;

    renderer_draw_text(renderer, button->text, button->font, (Vec2i){textX, textY},
                       (Vec2i){textWidth, textHeight}, (Color4i){255, 255, 255, 255});
}

void button_update(UIComponent *self, UIManager *ui_manager)
{
    Button *button = (Button *)self->component;
    Renderer *renderer = ui_manager->renderer;
    Window *window = renderer->window;

    Vec2f mouse_pos = ui_manager->mouse_state.pos;
    Vec2f button_pos = (Vec2f){button->position.x, button->position.y};
    Vec2f button_size = (Vec2f){button->size.x, button->size.y};

    // check if mouse is inside button
    if (mouse_pos.x >= button_pos.x && mouse_pos.x <= button_pos.x + button_size.x &&
        mouse_pos.y <= button_pos.y && mouse_pos.y >= button_pos.y - button_size.y)
    {
        if (!button->is_hovered)
        {
            button->is_hovered = true;
            GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
            glfwSetCursor(window->glfw_window, cursor);
        }

        if (!button->mouse_down && ui_manager->mouse_state.left_button)
        {
            // user clicked inside button
            button->mouse_down = true;
        }
        else if (button->mouse_down && !ui_manager->mouse_state.left_button)
        {
            // user released mouse inside button
            button->mouse_down = false;

            if (button->on_click != NULL)
                button->on_click(self, button->on_click_data);
        }
    }
    else
    {
        if (button->is_hovered)
        {
            button->is_hovered = false;
            glfwSetCursor(window->glfw_window, NULL);
        }
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

    if (button->is_hovered)
        glfwSetCursor(ui_manager->renderer->window->glfw_window, NULL);

    free(button);
}
