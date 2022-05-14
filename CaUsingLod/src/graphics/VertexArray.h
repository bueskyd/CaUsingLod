/*
* Class representing an OpenGL vertex array object (VAO).
* Used for working with VAOs in a type safe manner.
*/

#ifndef PCG_VertexArray_H
#define PCG_VertexArray_H

#include <GL/glew.h>
#include <vector>
#include <memory>
#include "VertexBuffer.h"

namespace pcg
{
    class VertexArray
    {
    private:
        GLuint id;
        std::vector<std::unique_ptr<VertexBuffer>> vbos;
        uint32_t nextIndex;
    public:
        VertexArray(std::unique_ptr<VertexBuffer> vbo);
        VertexArray(std::vector<std::unique_ptr<VertexBuffer>>&& vbos);
        VertexArray(const VertexArray&) = delete;
        VertexArray(VertexArray&&) = delete;
        [[nodiscard]] VertexBuffer* GetVbo(unsigned int index);
        [[nodiscard]] std::vector<std::unique_ptr<VertexBuffer>>* GetVbos();
        void Bind();
        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;

        static void Unbind();
    };
}

#endif
