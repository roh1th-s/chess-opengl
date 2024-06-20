#if !defined(RENDERER_H)
#define RENDERER_H

#include "GLFW/glfw3.h"
#include "window.h"
#include "types.h"

typedef enum
{
    RECTANGLE,
    TRIANGLE
} EntityType;

typedef struct
{
    EntityType type;
    int width, height;
    Color3i color;
    Vec2i pos;
} DrawableEntity;

typedef struct
{
    Window *window;
    GLuint vertexBufferId;
    GLuint indexBufferId;
    GLuint vao;
    GLuint shaderProgram;
    DrawableEntity entities[];
} Renderer;

int renderer_init(Renderer *r, Window *window);
void renderer_clear_window(Renderer *r, Color3f color);
void renderer_draw_rect(Renderer *r, Color3i color, Vec2i pos, Vec2i size);
void renderer_update(Renderer *r);
void renderer_terminate(Renderer *r);

#endif