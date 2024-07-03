#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#include <GLEW/glew.h>

#include "texture.h"
#include "gl_error.h"

int texture_create(Texture *texture, const char *file_name)
{
    // load image data
    int width, height, n_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(file_name, &width, &height, &n_channels, 0);

    if (!data)
    {
        printf("Error loading texture: %s\n", file_name);
        return -1;
    }

    texture_new_from_buffer(texture, data, width, height, n_channels);

    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(data);

    return 0;
}

int texture_new_from_buffer(Texture *texture, const unsigned char *data, int width, int height, int n_channels)
{
    // gen and bind texture
    GL_CALL(glGenTextures(1, texture));
    texture_bind(*texture, GL_TEXTURE0);

    // set wrapping and filtering options
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLint format;
    switch (n_channels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        printf("Error: unsupported number of channels: %d\n", n_channels);
        return -1;
    }

    // add image data to texture
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));

    texture_unbind(GL_TEXTURE0);
    
    return 0;
}

void texture_bind(Texture texture, GLenum texture_unit)
{
    GL_CALL(glActiveTexture(texture_unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
}

void texture_unbind(GLenum texture_unit)
{
    GL_CALL(glActiveTexture(texture_unit));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
