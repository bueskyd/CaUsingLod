#include "Texture.h"
#include <iostream>

namespace pcg
{
    Texture::Texture(
        GLint level,
        GLint internalFormat,
        GLsizei width, GLsizei height,
        GLenum format,
        GLenum type, const void* pixels)
        :
        width(width), height(height), type(type), internalFormat(internalFormat)
    {
        glGenTextures(1, &id);
        CheckSize();
        Bind();
        glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height,
            0, format, type, pixels);
        SetFilters();
    }

    void Texture::CheckSize()
    {
        if (width < 1 || height < 1)
        {
            throw std::exception("Invalid input.");
        }
    }

    void Texture::SetFilters()
    {
        SetFilters(GL_NEAREST);
    }

    void Texture::SetFilters(GLenum filter)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    }

    void Texture::SetWrapping(GLenum wrapping)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);
    }

    void Texture::GenerateMipMap()
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    GLsizei Texture::GetWidth() const
    {
        return width;
    }

    GLsizei Texture::GetHeight() const
    {
        return height;
    }

    GLuint Texture::GetId() const
    {
        return id;
    }

    void Texture::SetSize(GLsizei width, GLsizei height)
    {
        this->width = width;
        this->height = height;
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, internalFormat,
            width, height, 0, internalFormat, GL_UNSIGNED_BYTE, nullptr);
    }

    void Texture::SetSize(glm::ivec2 size)
    {
        SetSize(size.x, size.y);
    }

    void Texture::SetWidth(GLsizei width)
    {
        SetSize(glm::ivec2(width, this->height));
    }

    void Texture::SetHeight(GLsizei height)
    {
        SetSize(glm::ivec2(this->width, height));
    }

    void Texture::Bind() const
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void Texture::Unbind() const
    {
        glActiveTexture(GL_TEXTURE0 + textureSlot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::SetSubTexture(
        uint32_t x, uint32_t y,
        uint32_t width, uint32_t height,
        GLenum format, GLenum type, const void* pixels)
    {
        glTextureSubImage2D(
            id,
            0,
            x,
            y,
            width,
            height,
            format,
            type,
            pixels);
    }

    std::vector<glm::vec2> Texture::GetTexCoords(
        glm::vec2 pixelPosition, glm::vec2 pixelSize)
    {
        std::vector<glm::vec2> texCoords
        {
            pixelPosition,
            glm::vec2(pixelPosition.x + pixelSize.x, pixelPosition.y),
            pixelPosition + pixelSize,
            glm::vec2(pixelPosition.x, pixelPosition.y + pixelSize.y)
        };
        for (unsigned int i = 0; i < texCoords.size(); i++)
        {
            texCoords[i].x /= width;
            texCoords[i].y /= height;
        }
        return texCoords;
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }
}
