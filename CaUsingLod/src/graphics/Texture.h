/*
* A class representing a texture. Contains data used by OpenGL.
*/

#ifndef PCG_TEXTURE2D_H
#define PCG_TEXTURE2D_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vec2.hpp>
#include <memory>
#include <vector>

namespace pcg
{
    class Texture
    {
    private:
        GLenum internalFormat;
        int textureSlot = 0;
        GLsizei width;
        GLsizei height;
        GLenum type;
        GLuint id;

        void CheckSize();
        void SetFilters();
    public:
        Texture(
            GLint level,
            GLint internalFormat,
            GLsizei width, GLsizei height,
            GLenum format,
            GLenum type, const void* pixels);

        Texture(const Texture&) = delete;
        Texture(Texture&&) = delete;

        void SetWrapping(GLenum wrapping);
        void SetFilters(GLenum filter);
        void GenerateMipMap();
        GLsizei GetWidth() const;
        GLsizei GetHeight() const;
        GLuint GetId() const;
        void SetSize(GLsizei width, GLsizei height);
        void SetSize(glm::ivec2 size);
        void SetWidth(GLsizei width);
        void SetHeight(GLsizei height);
        void Bind() const;
        void Unbind() const;
        void SetSubTexture(
            uint32_t x, uint32_t y,
            uint32_t width, uint32_t height,
            GLenum format, GLenum type, const void* pixels);
        std::vector<glm::vec2> GetTexCoords(
            glm::vec2 pixelPosition, glm::vec2 pixelSize);

        Texture& operator=(const Texture&) = delete;
        Texture& operator=(Texture&&) = delete;

        ~Texture();
    };
}

#endif
