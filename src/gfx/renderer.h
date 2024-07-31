#if !defined(RENDERER_H)
#define RENDERER_H

#include "GLFW/glfw3.h"
#include "GLEW/glew.h"
#include "window.h"
#include "vao.h"
#include "texture.h"
#include "font.h"
#include "../types.h"

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

typedef enum
{
    SHADER_NONE = -1,
    SHADER_BASIC_COLOR_TEX,
    SHADER_BASIC_TEXT
} ShaderType;

typedef enum
{
    VAO_BASIC_COLOR_TEX,
    VAO_BASIC_TEXT
} VAOType;

typedef struct
{
    Window *window;

    GLuint vbo; // vertex buffer object
    GLuint ibo; // index buffer object
    VAO vaos[2];    // vertex array objects

    GLuint shaders[2];
    GLuint current_shader;
    ShaderType current_shader_type;

    DrawableEntity entities[];
} Renderer;

int renderer_init(Renderer *self, Window *window);

void renderer_clear_window(Renderer *self, Color3i color);
void renderer_use_shader(Renderer *self, ShaderType type);
void renderer_use_vao(Renderer *self, VAOType type);

void renderer_draw_rect(Renderer *self, Color4i color, Vec2i pos, Vec2i size);
void renderer_draw_rect_tex_colored(Renderer *self, Color4i color, Texture tex, Vec2i pos, Vec2i size);
void renderer_draw_rect_tex(Renderer *self, Texture tex, Vec2i pos, Vec2i size);

void renderer_draw_text_scaled(Renderer *self, const char *text, Font *font, Vec2i pos, Vec2f scale, Color4i color);
void renderer_draw_text(Renderer *self, const char* text, Font *font, Vec2i pos, Vec2i size, Color4i color);
void renderer_draw_text_with_width(Renderer *self, const char *text, Font *font, Vec2i pos, unsigned int width, Color4i color);

void renderer_update(Renderer *self);
void renderer_terminate(Renderer *self);

#endif