#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#include <GLEW/glew.h>

#include "texture.h"
#include "gl_error.h"

GLuint init_texture(const char* file_name)
{   
    // gen and bind texture
    GLuint texture;
    GL_CALL(glGenTextures(1, &texture));
    bind_texture(texture, GL_TEXTURE0);

    // set wrapping and filtering options
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // load image data
    int width, height, n_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_name, &width, &height, &n_channels, 0);
    
    if (!data)
    {
        printf("Error loading texture: %s\n", file_name);
        return 0;
    }
    // add image data to texture
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(data);

    return texture;
}

void bind_texture(GLuint texture, GLenum texture_unit)
{   
    GL_CALL(glActiveTexture(texture_unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
}

void unbind_texture(GLenum texture_unit)
{
    GL_CALL(glActiveTexture(texture_unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
