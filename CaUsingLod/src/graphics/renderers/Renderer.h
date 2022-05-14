#ifndef PCG_RENDERER_H
#define PCG_RENDERER_H

#include <vector>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/ShaderProgram.h"
#include <memory>

namespace pcg
{
    template<typename Shape>
    class Renderer
    {
    private:
        std::vector<Shape> shapes;
        VertexBuffer* vbo;
        std::unique_ptr<VertexArray> vao;
        const ShaderProgram* shaderProgram;
        size_t shapeCount = 0;
        size_t oldShapeCount = 0;
        GLenum glPrimitive;
    public:
        Renderer(
            std::unique_ptr<VertexArray> vao,
            const ShaderProgram* shaderProgram,
            GLenum primitive);
        void Draw(const Shape& shape);
        void Clear();
        void Render();
    };

    template<typename Shape>
    inline Renderer<Shape>::Renderer(
        std::unique_ptr<VertexArray> vao,
        const ShaderProgram* shaderProgram, GLenum primitive)
        :
        vbo(vao->GetVbo(0)),
        vao(std::move(vao)),
        shaderProgram(shaderProgram),
        glPrimitive(primitive) { }

    template<typename Shape>
    inline void Renderer<Shape>::Draw(const Shape& shape)
    {
        shapes.push_back(shape);
    }

    template<typename Shape>
    inline void Renderer<Shape>::Clear()
    {
        vbo->Clear();
        shapes.clear();
    }

    template<typename Shape>
    inline void Renderer<Shape>::Render()
    {
        if (shapes.size())
        {
            size_t dataSize = shapes.size() * sizeof(Shape);
            if (dataSize > vbo->GetSize())
                vbo->SetData(&shapes[0], shapes.size());
            else
                vbo->Write(0, &shapes[0], shapes.size());
        }
        vao->Bind();
        shaderProgram->Use();
        GLsizei vertexCount = static_cast<GLsizei>(shapes.size() * Shape::vertexCount);
        glDrawArrays(glPrimitive, 0, vertexCount);
        VertexArray::Unbind();
    }
}

#endif
