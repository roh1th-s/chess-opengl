#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "../types.h"
#include "gl_error.h"
#include "renderer.h"
#include "shader.h"
#include "vao.h"
#include "window.h"

int renderer_init(Renderer *self, Window *window)
{
    if (glewInit() != GLEW_OK)
    {
        printf("Error!\n");
        return -1;
    }

    printf("Hardware: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

    self->window = window;

    // create buffers
    GL_CALL(glGenBuffers(1, &self->vbo));
    GL_CALL(glGenBuffers(1, &self->ibo));

    VAO vao1 = vao_create();
    vao_bind(vao1);
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, self->vbo)); // bind it
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo));
    GLsizei stride = 8 * sizeof(float);
    vao_attr(vao1, 0, 2, GL_FLOAT, stride, 0);
    vao_attr(vao1, 1, 4, GL_FLOAT, stride, 2 * sizeof(float));
    vao_attr(vao1, 2, 2, GL_FLOAT, stride, 6 * sizeof(float));
    self->vaos[VAO_BASIC_COLOR_TEX] = vao1;

    VAO vao2 = vao_create();
    vao_bind(vao2);
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, self->vbo)); // same buffers for both vaos
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->ibo));
    stride = 4 * sizeof(float);
    vao_attr(vao2, 0, 2, GL_FLOAT, stride, 0);
    vao_attr(vao2, 1, 2, GL_FLOAT, stride, 2 * sizeof(float));
    self->vaos[VAO_BASIC_TEXT] = vao2;

    // Enable blending (for transparency)
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // MSAA Anti-aliasing
    GL_CALL(glEnable(GL_MULTISAMPLE));

    // shaders
    self->current_shader_type = SHADER_NONE;
    self->shaders[SHADER_BASIC_COLOR_TEX] =
        shader_new("res/shaders/basic_color_tex.vs", "res/shaders/basic_color_tex.fs");
    self->shaders[SHADER_BASIC_TEXT] = shader_new("res/shaders/basic_text.vs", "res/shaders/basic_text.fs");
    renderer_use_shader(self, SHADER_BASIC_COLOR_TEX);

    return 0;
}

void renderer_clear_window(Renderer *self, Color3i color)
{
    GL_CALL(glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, 1.0));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void renderer_use_shader(Renderer *self, ShaderType type)
{
    if (self->current_shader_type == type)
        return;

    self->current_shader_type = type;
    self->current_shader = self->shaders[type];
    shader_use(self->current_shader);
}

void renderer_use_vao(Renderer *self, VAOType type) { vao_bind(self->vaos[type]); }

void renderer_draw_rect(Renderer *self, Color4i color, Vec2i pos, Vec2i size)
{
    int w = self->window->width;
    int h = self->window->height;

    Vec2f top_left = {((float)pos.x / w) * 2 - 1, ((float)pos.y / h) * 2 - 1}; // normalize position
    Vec2f normalized_size = {((float)size.x / w) * 2, ((float)size.y / h) * 2};
    Color4f normalized_color = {(float)color.r / 255, (float)color.g / 255, (float)color.b / 255,
                                (float)color.a / 255};

    renderer_use_vao(self, VAO_BASIC_COLOR_TEX);

    // position (vec2), color (vec4), texCoord (vec2)
    float vertices[] = {top_left.x,
                        top_left.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        0.0f,

                        top_left.x + normalized_size.x,
                        top_left.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        0.0f,

                        top_left.x + normalized_size.x,
                        top_left.y - normalized_size.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        0.0f,

                        top_left.x,
                        top_left.y - normalized_size.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        0.0f};

    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));

    renderer_use_shader(self, SHADER_BASIC_COLOR_TEX);
    shader_uniform_int(self->current_shader, "useTexture", 0);

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

void renderer_draw_rect_tex_colored(Renderer *self, Color4i color, Texture tex, Vec2i pos, Vec2i size)
{
    int w = self->window->width;
    int h = self->window->height;

    Vec2f top_left = {((float)pos.x / w) * 2 - 1, ((float)pos.y / h) * 2 - 1}; // normalize position
    Vec2f normalized_size = {((float)size.x / w) * 2, ((float)size.y / h) * 2};
    Color4f normalized_color = {(float)color.r / 255, (float)color.g / 255, (float)color.b / 255,
                                (float)color.a / 255};

    renderer_use_vao(self, VAO_BASIC_COLOR_TEX);

    // position (vec2), color (vec4), texCoord (vec2)
    float vertices[] = {top_left.x,
                        top_left.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        1.0f,

                        top_left.x + normalized_size.x,
                        top_left.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        1.0f,
                        1.0f,

                        top_left.x + normalized_size.x,
                        top_left.y - normalized_size.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        1.0f,
                        0.0f,

                        top_left.x,
                        top_left.y - normalized_size.y,
                        normalized_color.r,
                        normalized_color.g,
                        normalized_color.b,
                        normalized_color.a,
                        0.0f,
                        0.0f};

    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));

    unsigned int indices[] = {0, 1, 2, 2, 3, 0};
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));

    texture_bind(tex, GL_TEXTURE0);

    renderer_use_shader(self, SHADER_BASIC_COLOR_TEX);
    shader_uniform_int(self->current_shader, "useTexture", 1);

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

    texture_unbind(GL_TEXTURE0);
}

void renderer_draw_rect_tex(Renderer *self, Texture tex, Vec2i pos, Vec2i size)
{
    renderer_draw_rect_tex_colored(self, (Color4i){255, 255, 255, 255}, tex, pos, size);
}

void renderer_draw_text_scaled(Renderer *self, const char *text, Font *font, Vec2i pos, Vec2f scale,
                               Color4i color)
{
    int w = self->window->width;
    int h = self->window->height;

    Color4f normalized_color = {(float)color.r / 255, (float)color.g / 255, (float)color.b / 255,
                                (float)color.a / 255};

    unsigned int height_from_baseline = get_height_from_baseline(font, text);
    pos.y -= height_from_baseline * scale.y; // align to baseline

    renderer_use_vao(self, VAO_BASIC_TEXT);

    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];
        Glyph g = font->glyphs[(int)c];

        Vec2i charPos;
        charPos.x = pos.x + g.bearing.x * scale.x;
        charPos.y = pos.y - (g.size.y - g.bearing.y) * scale.y;

        Vec2i charSize = {g.size.x * scale.x, g.size.y * scale.y};

        Vec2f normalized_origin = {((float)charPos.x / w) * 2 - 1,
                                   ((float)charPos.y / h) * 2 - 1}; // normalize position
        Vec2f normalized_size = {((float)charSize.x / w) * 2, ((float)charSize.y / h) * 2};

        float vertices[] = {normalized_origin.x,
                            normalized_origin.y,
                            0.0f,
                            1.0f,
                            normalized_origin.x + normalized_size.x,
                            normalized_origin.y,
                            1.0f,
                            1.0f,
                            normalized_origin.x + normalized_size.x,
                            normalized_origin.y + normalized_size.y,
                            1.0f,
                            0.0f,
                            normalized_origin.x,
                            normalized_origin.y + normalized_size.y,
                            0.0f,
                            0.0f};
        GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));

        unsigned int indices[] = {0, 1, 2, 2, 3, 0};
        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));

        texture_bind(g.texture, GL_TEXTURE0);

        renderer_use_shader(self, SHADER_BASIC_TEXT);
        shader_uniform_vec4f(
            self->current_shader, "textColor",
            (Vec4f){normalized_color.r, normalized_color.g, normalized_color.b, normalized_color.a});

        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

        pos.x += (g.advance >> 6) * scale.x;
    }
}

void renderer_draw_text(Renderer *self, const char *text, Font *font, Vec2i pos, Vec2i size, Color4i color)
{
    Vec2i text_dimensions = get_text_dimensions(font, text);
    float x_scale = (float)size.x / text_dimensions.x;
    float y_scale = (float)size.y / text_dimensions.y;

    renderer_draw_text_scaled(self, text, font, pos, (Vec2f){x_scale, y_scale}, color);
}

void renderer_draw_text_with_width(Renderer *self, const char *text, Font *font, Vec2i pos,
                                   unsigned int width, Color4i color)
{
    Vec2i text_dimensions = get_text_dimensions(font, text);
    float x_scale = (float)width / text_dimensions.x;
    float y_scale = (float)width / text_dimensions.x;

    renderer_draw_text_scaled(self, text, font, pos, (Vec2f){x_scale, y_scale}, color);
}

void renderer_update(Renderer *self)
{
    // TODO (Batch renderer?)
}

void renderer_terminate(Renderer *self)
{
    GL_CALL(glDeleteBuffers(1, &self->vbo));
    GL_CALL(glDeleteBuffers(1, &self->ibo));

    vao_delete(self->vaos[VAO_BASIC_COLOR_TEX]);
    vao_delete(self->vaos[VAO_BASIC_TEXT]);

    shader_delete(self->shaders[SHADER_BASIC_COLOR_TEX]);
    shader_delete(self->shaders[SHADER_BASIC_TEXT]);
}