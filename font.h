#ifndef FONT_CLASS_H
#define FONT_CLASS_H

#include <ft2build.h>
#include <map>
#include <glm/glm.hpp>
#include <iostream>

#include FT_FREETYPE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Font
{
public:

    struct Character
    {
        unsigned int TextureID;  // ID handle of the glyph texture
        glm::ivec2   Size;       // Size of glyph
        glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
        unsigned int Advance;    // Offset to advance to next glyph
    };

    const char* fontPath;
    int height;

    FT_Face face;

    std::map<char, Character> Characters;

    Font(const char* fontPath, int height);
    Font();

    int loadGlyph(char character);
};

#endif