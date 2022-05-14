/*
* Class representing an OpenGL vertex buffer object (VBO).
* Used for working with VBOs in a type safe manner.
*/

#ifndef PCG_VERTEXBUFFER_H
#define PCG_VERTEXBUFFER_H

#include "Buffer.h"

namespace pcg
{
    class VertexBuffer
    {
    private:
        Buffer buffer;
    public:
        VertexBuffer(size_t size, GLenum usage);
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer(VertexBuffer&&) = delete;
        void Bind() const;
        size_t GetSize() const;
        template<typename T>
        void Write(size_t offset, const T* items, size_t itemCount);
        template<typename T>
        void SetData(const T* items, size_t itemCount);
        void Clear();
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        VertexBuffer& operator=(VertexBuffer&&) = delete;
    };

    template<typename T>
    inline void VertexBuffer::Write(size_t offset, const T* items, size_t itemCount)
    {
        buffer.Write(offset, items, itemCount);
    }

    template<typename T>
    inline void VertexBuffer::SetData(const T* items, size_t itemCount)
    {
        buffer.SetData(items, itemCount);
    }
}

#endif
