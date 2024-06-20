#if !defined(SHADER_H)
#define SHADER_H

#include <GLEW/glew.h>

GLuint shader_new(const char *vertex_shader_path, const char *frag_shader_path);
void shader_use(GLuint shader);
void shader_delete(GLuint shader);

#endif // SHADER_H