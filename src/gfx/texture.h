#if !defined(TEX_H)
#define TEX_H

GLuint init_texture(const char *file_name);
void bind_texture(GLuint texture, GLenum texture_unit);
void unbind_texture();

#endif