#include "Buffer.h"

namespace pcg
{
    Buffer::Buffer(size_t size, GLenum usage, GLenum target)
        : size(size), target(target), usage(usage)
    {
        glGenBuffers(1, &id);
        glBindBuffer(target, id);
        glBufferData(target, size, nullptr, usage);
        glBindBuffer(target, 0);
    }

    void Buffer::Bind() const
    {
        glBindBuffer(target, id);
    }

    GLuint Buffer::GetId() const
    {
        return id;
    }

    GLenum Buffer::GetTarget() const
    {
        return target;
    }

    size_t Buffer::GetSize() const
    {
        return size;
    }

    void Buffer::Clear()
    {
        uint8_t* bytes = new uint8_t[size];
        Write(0, bytes, size);
        delete[] bytes;
    }
}
