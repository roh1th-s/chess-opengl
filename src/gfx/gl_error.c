#include "gl_error.h"

#include <GL/glew.h>
#include <stdbool.h>
#include <stdio.h>

void glClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool glCheckError(const char *function_name, const char *file, int line)
{
    GLenum error;
    while ((error = glGetError()))
    {
        printf("[OpenGL Error] (%d): %s, %s, line %d\n", error, function_name, file, line);
        fflush(stdout);
        return false;
    }
    return true;
}
