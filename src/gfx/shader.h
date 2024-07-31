#if !defined(SHADER_H)
#define SHADER_H

#include <GLEW/glew.h>

#include "../types.h"

GLuint shader_new(const char *vertex_shader_path, const char *frag_shader_path);
void shader_use(GLuint shader);
void shader_delete(GLuint shader);

void shader_uniform_int(GLuint shader, const char *name, int value);
void shader_uniform_vec3f(GLuint shader, const char *name, Vec3f vec);
void shader_uniform_vec4f(GLuint shader, const char *name, Vec4f vec);

#endif // SHADER_H