#if !defined(FONT_H)
#define FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "../types.h"
#include "texture.h"

typedef struct
{
    Texture texture;
    Vec2i size;
    Vec2i bearing;
    unsigned int advance;
} Glyph;

typedef struct
{
    const char *name;
    unsigned int size;
    Glyph glyphs[128];
} Font;

int font_init(Font *font, const char *font_path, unsigned int font_size);
Vec2i get_text_dimensions(const Font *font, const char *text);
unsigned int get_height_from_baseline(const Font *font, const char* text);

#endif