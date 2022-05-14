#ifndef PCG_INDEXEDRENDERER_H
#define PCG_INDEXEDRENDERER_H

#include <vector>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/ShaderProgram.h"
#include <memory>
#include "graphics/Texture.h"

namespace pcg
{
    template<typename Vertex>
    class IndexedRenderer
    {
    private:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        VertexBuffer* vbo;
        ElementBuffer* ebo;
        std::unique_ptr<VertexArray> vao;
        const ShaderProgram* shaderProgram;
        GLenum glPrimitive;
        const Texture* texture;
    public:
        IndexedRenderer(
            std::unique_ptr<VertexArray> vao,
            ShaderProgram* shaderProgram, 
            GLenum primitive);
        void SetTexture(const Texture* texture);
        void Draw(
            const Vertex* vertices,
            const uint32_t* indices,
            size_t vertexCount,
            size_t indexCount);
        void Clear();
        void Render();
    };

    template<typename Vertex>
    inline IndexedRenderer<Vertex>::IndexedRenderer(
        std::unique_ptr<VertexArray> vao,
        ShaderProgram* shaderProgram, GLenum primitive)
        :
        vbo(vao->GetVbo(0)),
        ebo(vao->GetEbo()),
        vao(std::move(vao)),
        shaderProgram(shaderProgram),
        glPrimitive(primitive),
        texture(nullptr) { }

    template<typename Vertex>
    inline void IndexedRenderer<Vertex>::SetTexture(const Texture* texture)
    {
        this->texture = texture;
    }

    template<typename Vertex>
    inline void IndexedRenderer<Vertex>::Draw(
        const Vertex* vertices,
        const uint32_t* indices,
        size_t vertexCount,
        size_t indexCount)
    {
        uint32_t* offsetIndices = new uint32_t[indexCount];
        for (size_t i = 0; i < indexCount; i++)
            offsetIndices[i] = indices[i] + this->vertices.size();
        this->indices.insert(this->indices.end(), offsetIndices, offsetIndices + indexCount);
        delete[] offsetIndices;
        this->vertices.insert(this->vertices.end(), vertices, vertices + vertexCount);
    }

    template<typename Vertex>
    inline void IndexedRenderer<Vertex>::Clear()
    {
        vbo->Clear();
        ebo->Clear();
        vertices.clear();
        indices.clear();
    }

    template<typename Vertex>
    inline void IndexedRenderer<Vertex>::Render()
    {
        if (vertices.size())
        {
            size_t dataSize = vertices.size() * sizeof(Vertex);
            if (dataSize > vbo->GetSize())
                vbo->SetData(&vertices[0], vertices.size());
            else
                vbo->Write(0, &vertices[0], vertices.size());
        }
        if (indices.size())
        {
            if (indices.size() > ebo->GetCount())
                ebo->SetData(&indices[0], indices.size());
            else
                ebo->Write(0, &indices[0], indices.size());
        }
        vao->Bind();
        shaderProgram->Use();
        if (texture)
            texture->Bind();
        glDrawElements(glPrimitive, indices.size(), GL_UNSIGNED_INT, 0);
        VertexArray::Unbind();
    }
}

#endif
