#include "VertexBuffer.h"

namespace pcg
{
    VertexBuffer::VertexBuffer(size_t size, GLenum usage)
        : buffer(size, usage, GL_ARRAY_BUFFER) { }

    void VertexBuffer::Bind() const
    {
        buffer.Bind();
    }

    size_t VertexBuffer::GetSize() const
    {
        return buffer.GetSize();
    }

    void VertexBuffer::Clear()
    {
        buffer.Clear();
    }
}
