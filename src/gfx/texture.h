#if !defined(TEX_H)
#define TEX_H

#include <GLEW/glew.h>

typedef GLuint Texture;

int texture_create(Texture *texture, const char *file_name);
int texture_new_from_buffer(Texture *texture, const unsigned char *data, int width, int height, int n_channels);
void texture_bind(Texture texture, GLenum texture_unit);
void texture_unbind(GLenum texture_unit);

#endif