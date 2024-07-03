#include <GLEW/glew.h>

#include "vao.h"
#include "gl_error.h"

VAO vao_create()
{
    VAO self;
    glGenVertexArrays(1, &self.handle);
    return self;
}

void vao_delete(VAO self)
{
    glDeleteVertexArrays(1, &self.handle);
}

void vao_bind(VAO self)
{
    glBindVertexArray(self.handle);
}

void vao_attr(
    VAO self, GLuint index, GLint size, GLenum type,
    GLsizei stride, size_t offset)
{
    glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *)offset);
    glEnableVertexAttribArray(index);
}