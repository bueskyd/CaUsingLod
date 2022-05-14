#ifndef PCG_CELLRENDERER_H
#define PCG_CELLRENDERER_H

#include <vector>
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/CellVertex.h"
#include "graphics/CellShape.h"

namespace pcg
{
    class ShaderProgram;

    class CellRenderer
    {
    private:
        std::vector<CellShape> clearData;
        std::vector<CellShape> newCells;
        VertexBuffer* vbo;
        VertexBuffer* instanceVbo;
        std::unique_ptr<VertexArray> vao;
        const ShaderProgram* shaderProgram;
        size_t vertexCount;
        size_t oldInstanceCount;
        size_t instanceCount;
    public:
        CellRenderer(
            uint32_t cellCount, uint32_t cellSize, 
            const ShaderProgram* shaderProgram);
        void Draw(const CellShape& cell);
        void Clear();
        void SetCellSize(uint32_t size);
        void Render();
    };
}

#endif
