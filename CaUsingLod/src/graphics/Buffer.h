/*
* Class representing a OpenGL buffer object.
* Used for working with buffers in a type safe manner.
*/

#ifndef PCG_BUFFER_H
#define PCG_BUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

namespace pcg
{
    class Buffer
    {
    private:
        GLuint id;
        size_t size;
        GLenum target;
        GLenum usage;
    public:
        Buffer(size_t size, GLenum usage, GLenum target);
        Buffer(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        void Bind() const;
        GLuint GetId() const;
        GLenum GetTarget() const;
        size_t GetSize() const;
        template<typename T>
        void Write(size_t offset, const T* items, size_t itemCount);
        template<typename T>
        void SetData(const T* items, size_t itemCount);
        void Clear();
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&) = delete;
    };

    template<typename T>
    inline void Buffer::Write(size_t offset, const T* items, size_t itemCount)
    {
        glBindBuffer(target, id);
        size_t offsetInBytes = sizeof(T) * offset;
        size_t sizeInBytes = sizeof(T) * itemCount;
        glBufferSubData(target, offsetInBytes, sizeInBytes, &items[0]);
        glBindBuffer(target, 0);
    }

    template<typename T>
    inline void Buffer::SetData(const T* items, size_t itemCount)
    {
        this->size = size;
        glBindBuffer(target, id);
        size_t size = sizeof(T) * itemCount;
        glBufferData(target, size, &items[0], usage);
        glBindBuffer(target, 0);
    }
}

#endif
