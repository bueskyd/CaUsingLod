#include "VertexArrayBuilder.h"
#include <vector>
#include <memory>

namespace pcg
{
    VertexArrayBuilder::VertexArrayBuilder(
        std::vector<std::unique_ptr<VertexBuffer>> vbos)
        : isBuilt(false), index(0)
    {
        vao = std::make_unique<VertexArray>(std::move(vbos));
        this->vbos = vao->GetVbos();
        InitOffsets();
    }
    
    VertexArrayBuilder::VertexArrayBuilder(
        std::unique_ptr<VertexBuffer> vbo)
        : isBuilt(false), index(0)
    {
        vao = std::make_unique<VertexArray>(std::move(vbo));
        vbos = vao->GetVbos();
        InitOffsets();
    }
    
    void VertexArrayBuilder::InitOffsets()
    {
        for (size_t i = 0; i < vbos->size(); i++)
            offsets.push_back(0);
    }
    
    void VertexArrayBuilder::SetAttributeDivisor(
        unsigned int divisor, unsigned int startAttrib, unsigned int endAttrib)
    {
        vao->Bind();
        if (endAttrib < startAttrib)
            throw std::exception(
                "startAttrib must be less than or equal to endAttrib");
        for (unsigned int i = startAttrib; i < endAttrib + 1; i++)
        {
            glVertexAttribDivisor(i, divisor);
        }
    }
    
    std::unique_ptr<VertexArray> VertexArrayBuilder::Build()
    {
        isBuilt = true;
        return std::move(vao);
    }
}
