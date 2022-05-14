#include "UniformBuffer.h"

namespace pcg
{
    UniformBuffer::UniformBuffer(
        size_t size, GLenum usage, GLuint binding)
        : buffer(size, usage, GL_UNIFORM_BUFFER)
    {
        buffer.Bind();
        glBindBufferBase(buffer.GetTarget(), binding, buffer.GetId());
    }

    void UniformBuffer::Bind() const
    {
        buffer.Bind();
    }

    size_t UniformBuffer::GetSize() const
    {
        return buffer.GetSize();
    }
}
