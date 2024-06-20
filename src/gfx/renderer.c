#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "renderer.h"
#include "window.h"
#include "vertex.h"
#include "shader.h"
#include "../types.h"
#include "../gl_error.h"

int renderer_init(Renderer *r, Window *window)
{
    if (glewInit() != GLEW_OK)
    {
        printf("Error!\n");
        return -1;
    }
    // setup buffers
    GL_CALL(glGenVertexArrays(1, &r->vao));
    GL_CALL(glBindVertexArray(r->vao));

    GL_CALL(glGenBuffers(1, &r->vertexBufferId));              // create a buffer
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, r->vertexBufferId)); // select (bind) it

    // tell opengl how to interpret the vertex attributes
    GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0));
    GL_CALL(glEnableVertexAttribArray(0)); // enable this vertex attribute
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void *)(2 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (const void *)(5 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(2));

    GL_CALL(glGenBuffers(1, &r->indexBufferId));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->indexBufferId));

    // shaders
    GLuint shader_program = shader_new("res/shaders/vertex.glsl", "res/shaders/fragment.glsl");
    shader_use(shader_program);

    r->window = window;
    r->shaderProgram = shader_program;
    
    return 0;
}

void renderer_clear_window(Renderer *r, Color3f color)
{
    GL_CALL(glClearColor(color.r, color.g, color.b, 1.0));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void renderer_draw_rect(Renderer *r, Color3i color, Vec2i pos, Vec2i size)
{
    int w = r->window->width;
    int h = r->window->height;

    Vec2f topLeft = {((float)pos.x / w) * 2 - 1, ((float)pos.y / h) * 2 - 1}; // normalize position
    Vec2f normalizedSize = {((float)size.x / w) * 2, ((float)size.y / h) * 2};
    Color3f normalizedColor = {(float)color.r / 255, (float)color.g / 255, (float)color.b / 255};

    Vertex vertices[] = {
        {topLeft, normalizedColor, {0.0f, 0.0f}},
        {{topLeft.x + normalizedSize.x, topLeft.y}, normalizedColor, {0.0f, 0.0f}},
        {{topLeft.x + normalizedSize.x, topLeft.y - normalizedSize.y}, normalizedColor, {0.0f, 0.0f}},
        {{topLeft.x, topLeft.y - normalizedSize.y}, normalizedColor, {0.0f, 0.0f}}};
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0};
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));

    GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
}

void renderer_update(Renderer *r)
{
    // TODO (Batch renderer?) 
}

void renderer_terminate(Renderer *r)
{
    GL_CALL(glDeleteBuffers(1, &r->vertexBufferId));
    GL_CALL(glDeleteBuffers(1, &r->indexBufferId));
    GL_CALL(glDeleteVertexArrays(1, &r->vao));
}