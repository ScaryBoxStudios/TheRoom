#include "Font.hpp"
#include <ft2build.h>
#include <freetype/freetype.h>

Font::Font()
{
}

Font::~Font()
{
    Destroy();
}

auto Font::Load(const BufferType& fontData) -> void
{
    FT_Library ft;
    FT_Face face;

    // Initialize freetype library memory face
    FT_Init_FreeType(&ft);
    FT_New_Memory_Face(ft, fontData.data(), static_cast<FT_Long>(fontData.size()), 0, &face);

    // Set the pixel size
    FT_Set_Pixel_Sizes(face, 0, 28);

    // Store current unpack alignment
    GLint pixelStoreAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixelStoreAlignment);

    // Disable 4 byte alignment because we use one byte per pixel
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load the first 128 glyphs
    for (GLubyte c = 0; c < 128; ++c)
    {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        Glyph glyph = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        mGlyphs.insert({c, glyph});
    };

    // Re-enable byte alignment
    glPixelStorei(GL_UNPACK_ALIGNMENT, pixelStoreAlignment);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

auto Font::operator[](const char c) const -> const Glyph*
{
    const auto& p = mGlyphs.find(c);
    if (p != mGlyphs.end())
        return &p->second;
    else
        return nullptr;
}

void Font::Destroy()
{
    for (const auto& g : mGlyphs)
        glDeleteTextures(1, &g.second.texId);
    mGlyphs.clear();
}

