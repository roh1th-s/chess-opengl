#if !defined(VAO_H)
#define VAO_H

#include <GL/glew.h>

typedef struct
{
    GLuint handle;
} VAO;

VAO vao_create();
void vao_delete(VAO self);
void vao_bind(VAO self);
void vao_attr(
    VAO self, GLuint index, GLint size, GLenum type,
    GLsizei stride, size_t offset);

#endif