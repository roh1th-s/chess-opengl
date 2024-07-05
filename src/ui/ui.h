#if !defined(UI_H)
#define UI_H

#include <stdbool.h>

#include "../gfx/renderer.h"

typedef struct UIComponent UIComponent;
typedef struct UIManager UIManager;

typedef void (*UIComponentCB)(UIComponent *, UIManager *);

typedef enum
{
    BUTTON,
} UIComponentType;

typedef struct UIComponent
{
    UIComponentCB destroy, render, update;
    void *component;
    UIComponentType type;
    bool enabled;
} UIComponent;

#define MAX_UI_COMPONENTS 10

typedef struct 
{
    bool left_button;
    bool right_button;

    Vec2f pos;
} MouseState;

typedef struct UIManager
{
    Renderer *renderer;
    UIComponent *components[MAX_UI_COMPONENTS];
    int n_components;

    MouseState mouse_state;
} UIManager;

void ui_init(UIManager *self, Renderer *renderer);
void ui_add_component(UIManager *self, UIComponent *component);
void ui_destroy_all(UIManager *self);
void ui_render(UIManager *self);
void ui_update(UIManager *self);

#endif