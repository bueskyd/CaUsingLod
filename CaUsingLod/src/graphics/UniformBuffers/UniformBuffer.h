#ifndef PCG_UNIFORMBUFFER_H
#define PCG_UNIFORMBUFFER_H

#include "graphics/Buffer.h"

namespace pcg
{
    class UniformBuffer
    {
    private:
        Buffer buffer;
    public:
        UniformBuffer(size_t size, GLenum usage, GLuint binding);
        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        void Bind() const;
        size_t GetSize() const;
        template<typename T>
        void Write(size_t offset, const T* items, size_t itemCount);
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;
    };

    template<typename T>
    void UniformBuffer::Write(size_t offset, const T* items, size_t itemCount)
    {
        buffer.Write<T>(offset, items, itemCount);
    }
}

#endif
