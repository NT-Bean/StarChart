#include "texture.h"
#include <iostream>

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);
    if (!bytes) {
        std::cerr << "Failed to load texture: " << image << std::endl;
        ID = 0;
        return;
    }

    GLenum srcFormat = GL_RGB;
    if (numColCh == 1) srcFormat = GL_RED;
    else if (numColCh == 3) srcFormat = GL_RGB;
    else if (numColCh == 4) srcFormat = GL_RGBA;
    else {
        std::cerr << "Unexpected channels: " << numColCh << " in " << image << std::endl;
    }

    std::cout << "Loaded texture " << image << " (" << widthImg << "x" << heightImg << "), channels=" << numColCh << std::endl;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(texType, 0, srcFormat, widthImg, heightImg, 0, srcFormat, pixelType, bytes);
    glGenerateMipmap(texType);

    stbi_image_free(bytes);

    // Unbind texture
    glBindTexture(texType, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
    GLuint texUni = glGetUniformLocation(shader.ID, uniform);

    shader.Activate();
    glUniform1i(texUni, unit);
}

void Texture::Bind()
{
    if (ID != 0) glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    if (ID != 0) glDeleteTextures(1, &ID);
}