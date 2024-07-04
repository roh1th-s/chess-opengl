#include "ui.h"

void ui_init(UIManager *self, Renderer *r)
{
    self->renderer = r;
    self->n_components = 0;
}

void ui_add_component(UIManager *self, UIComponent *component)
{
    if (self->n_components < MAX_UI_COMPONENTS)
    {
        self->components[self->n_components++] = component;
    }
}

void ui_destroy(UIManager *self)
{
    for (int i = 0; i < self->n_components; i++)
    {
        self->components[i]->destroy(self->components[i], self);
        free(self->components[i]);
    }
    self->n_components = 0;
}

void ui_render(UIManager *self)
{
    for (int i = 0; i < self->n_components; i++)
    {
        if (self->components[i]->enabled)
        {
            self->components[i]->render(self->components[i], self);
        }
    }
}

void ui_update(UIManager *self)
{
    Window *w = self->renderer->window;

    // update mouse state
    bool left = glfwGetMouseButton(w->glfw_window, GLFW_MOUSE_BUTTON_LEFT);
    bool right = glfwGetMouseButton(w->glfw_window, GLFW_MOUSE_BUTTON_RIGHT);
    double xpos, ypos;
    glfwGetCursorPos(w->glfw_window, &xpos, &ypos);
    self->mouse_state.left_button = left;
    self->mouse_state.right_button = right;
    self->mouse_state.pos = (Vec2f){xpos, w->height - ypos}; // glfw has origin at top left
    
    for (int i = 0; i < self->n_components; i++)
    {
        if (self->components[i]->enabled)
        {
            self->components[i]->update(self->components[i], self);
        }
    }
}
