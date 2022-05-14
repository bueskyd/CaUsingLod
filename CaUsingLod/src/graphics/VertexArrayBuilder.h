/*
* Class used for building an instance of class VertexArray in a type safe manner.
*/

#ifndef PCG_VERTEXARRAYBUILDER_H
#define PCG_VERTEXARRAYBUILDER_H

#include <memory>
#include "VertexArray.h"
#include "VertexArray.h"
#include <vector>

namespace pcg
{
    class VertexArrayBuilder
    {
    private:
        std::unique_ptr<VertexArray> vao;
        std::vector<std::unique_ptr<VertexBuffer>>* vbos;
        std::vector<size_t> offsets;
        bool isBuilt;
        unsigned int index;
        void InitOffsets();
    public:
        VertexArrayBuilder(std::vector<std::unique_ptr<VertexBuffer>> vbos);
        VertexArrayBuilder(std::unique_ptr<VertexBuffer> vbo);
        template<typename AttribType, typename VertexType>
        void AddAttribute(
            unsigned int vboIndex, 
            unsigned int attribTypeCount, 
            GLenum glType);
        template<typename AttribType, typename VertexType>
        void AddAttribute(
            unsigned int vboIndex, 
            unsigned int attribTypeCount);
        void SetAttributeDivisor(
            unsigned int divisor, 
            unsigned int startAttrib, 
            unsigned int endAttrib);
        [[nodiscard]] std::unique_ptr<VertexArray> Build();
    };

    template<typename AttribType, typename VertexType>
    inline void VertexArrayBuilder::AddAttribute(
        unsigned int vboIndex, unsigned int attribTypeCount, GLenum glType)
    {
        if (isBuilt)
            throw std::exception("Cannot add attributes to already built VAO");
        vao->Bind();
        (*vbos)[vboIndex]->Bind();
        glVertexAttribPointer(
            index, attribTypeCount, glType,
            GL_FALSE, sizeof(VertexType), reinterpret_cast<void*>(offsets[vboIndex]));
        glEnableVertexAttribArray(index);
        offsets[vboIndex] += sizeof(AttribType) * attribTypeCount;
        ++index;
    }

    template<typename AttribType, typename VertexType>
    inline void VertexArrayBuilder::AddAttribute(
        unsigned int vboIndex, unsigned int attribTypeCount)
    {
        AddAttribute<AttribType, VertexType>(
            vboIndex, attribTypeCount, GL_FLOAT);
    }
}

#endif
