#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "GLEW/glew.h"

#ifdef _WIN32
#define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
#endif

#include "font.h"
#include "../types.h"
#include "gl_error.h"
#include "texture.h"

int font_init(Font *font, const char *font_path, unsigned int font_size)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Could not init FreeType Library\n");
        return -1;
    }

    char absolute_path[256];
    realpath(font_path, absolute_path);

    FT_Face face;
    if (FT_New_Face(ft, absolute_path, 0, &face))
    {
        printf("Failed to load font\n");
        return -1;
    }

    // extract font name
    char path_delimiter;
#ifdef _WIN32
    path_delimiter = '\\';
#else
    path_delimiter = '/';
#endif

    char *font_name = strrchr(absolute_path, path_delimiter) + 1;
    font->name = strcpy(malloc(strlen(font_name) + 1), font_name);

    FT_Set_Pixel_Sizes(face, 0, font_size);
    font->size = font_size;

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Failed to load Glyph : %c\n", c);
            continue;
        }

        Texture texture;
        texture_new_from_buffer(&texture, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);

        Glyph g = {
            .texture = texture,
            .size = {face->glyph->bitmap.width, face->glyph->bitmap.rows},
            .bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top},
            .advance = face->glyph->advance.x};

        font->glyphs[c] = g;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return 0;
}

Vec2i get_text_dimensions(const Font *font, const char *text)
{
    Vec2i dimensions = {0, 0};
    Vec2i pen = {0, 0};

    for (const char *c = text; *c; c++)
    {
        if (*c == '\n')
        {
            pen.x = 0;
            pen.y += font->glyphs['A'].size.y;
            continue;
        }

        Glyph ch = font->glyphs[*c];
        pen.x += ch.advance >> 6;
        dimensions.x = pen.x > dimensions.x ? pen.x : dimensions.x;
        dimensions.y = pen.y + ch.size.y > dimensions.y ? pen.y + ch.size.y : dimensions.y;
    }

    return dimensions;
}

unsigned int get_height_from_baseline(const Font *font, const char* text) {
    unsigned int height = 0;
    for (const char *c = text; *c; c++)
    {
        if (*c == '\n')
        {
            height += font->glyphs['A'].size.y;
            continue;
        }

        Glyph ch = font->glyphs[*c];
        height = ch.bearing.y > height ? ch.bearing.y : height;
    }

    return height;
}