#include "VertexArray.h"

namespace pcg
{
    VertexArray::VertexArray(std::unique_ptr<VertexBuffer> vbo)
        : nextIndex(0)
    {
        vbos.push_back(std::move(vbo));
        glGenVertexArrays(1, &id);
    }

    VertexArray::VertexArray(
        std::vector<std::unique_ptr<VertexBuffer>>&& vbos)
        : nextIndex(0), vbos(std::move(vbos))
    {
        glGenVertexArrays(1, &id);
    }

    VertexBuffer* VertexArray::GetVbo(unsigned int index)
    {
        return vbos[index].get();
    }

    std::vector<std::unique_ptr<VertexBuffer>>* VertexArray::GetVbos()
    {
        return &vbos;
    }

    void VertexArray::Bind()
    {
        glBindVertexArray(id);
    }

    void VertexArray::Unbind()
    {
        glBindVertexArray(0);
    }
}
