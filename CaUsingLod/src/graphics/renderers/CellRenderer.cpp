#include "CellRenderer.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include "graphics/ShaderProgram.h"
#include <graphics/VertexArrayBuilder.h>

namespace pcg
{
    CellRenderer::CellRenderer(
        uint32_t cellCount, uint32_t cellSize,
        const ShaderProgram* shaderProgram)
        :
        shaderProgram(shaderProgram),
        oldInstanceCount(0),
        instanceCount(0)
    {
        std::vector<CellVertex> vertices
        {
            { glm::vec2(0.0f, 0.0f) },
            { glm::vec2(0.0f, cellSize) },
            { glm::vec2(cellSize, 0.0f) },

            { glm::vec2(0.0f, cellSize) },
            { glm::vec2(cellSize, cellSize) },
            { glm::vec2(cellSize, 0.0f) }
        };

        vertexCount = 6;
        auto vertexVbo = std::make_unique<VertexBuffer>(
            vertexCount * sizeof(CellVertex), GL_STATIC_DRAW);
        vertexVbo->Write<CellVertex>(0, &vertices[0], vertices.size());
        vbo = vertexVbo.get();

        auto instanceVbo = std::make_unique<VertexBuffer>(
            cellCount * sizeof(CellShape), GL_DYNAMIC_DRAW);
        this->instanceVbo = instanceVbo.get();

        std::vector<std::unique_ptr<VertexBuffer>> vbos;
        vbos.push_back(std::move(vertexVbo));
        vbos.push_back(std::move(instanceVbo));
        VertexArrayBuilder vaoBuilder(std::move(vbos));
        vaoBuilder.AddAttribute<float, CellVertex>(0, 2);
        vaoBuilder.AddAttribute<float, CellShape>(1, 2);
        vaoBuilder.AddAttribute<float, CellShape>(1, 3);
        vaoBuilder.SetAttributeDivisor(1, 1, 2);

        vao = vaoBuilder.Build();

        clearData.resize(cellCount);
    }

    void CellRenderer::Draw(const CellShape& cell)
    {
        newCells.push_back(cell);
        instanceCount++;
    }

    void CellRenderer::Clear()
    {
        instanceVbo->Write(0, &clearData[0], instanceCount);
        newCells.clear();
        instanceCount = 0;
        oldInstanceCount = 0;
    }

    void CellRenderer::SetCellSize(uint32_t size)
    {
        std::vector<CellVertex> vertices
        {
            { glm::vec2(0.0f, 0.0f) },
            { glm::vec2(0.0f, size) },
            { glm::vec2(size, 0.0f) },

            { glm::vec2(0.0f, size) },
            { glm::vec2(size, size) },
            { glm::vec2(size, 0.0f) }
        };
        vbo->Write(0, &vertices[0], vertices.size());
    }

    void CellRenderer::Render()
    {
        if (newCells.size())
            instanceVbo->Write(
                oldInstanceCount, &newCells[0], newCells.size());
        newCells.clear();
        vao->Bind();
        shaderProgram->Use();
        glDrawArraysInstanced(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(vertexCount),
            instanceCount);
        oldInstanceCount = instanceCount;
    }
}
