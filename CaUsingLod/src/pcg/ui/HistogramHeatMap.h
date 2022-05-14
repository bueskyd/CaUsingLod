/*
* A class representing a histogram heat map.
* Displays grayscale colors in a specified range from x to 1.
*/

#ifndef PCG_HISTOGRAMHEATMAP_H
#define PCG_HISTOGRAMHEATMAP_H

#include <memory>
#include "graphics/renderers/CellRenderer.h"
#include "graphics/ShaderProgram.h"
#include "pcg/Generators/Generator.h"

namespace pcg
{
    class HistogramHeatMap
    {
    private:
        std::unique_ptr<pcg::CellRenderer> cellRenderer;
        std::unique_ptr<pcg::ShaderProgram> cellShader;
        std::vector<std::vector<CompType>> values;
        std::vector<glm::vec3> colors;
        uint32_t sizeInPixels;
        uint32_t pointSize;
        uint32_t sizeInPoints;
        CompType minDataX;
        CompType minDataY;
        CompType maxDataX;
        CompType maxDataY;
        uint32_t xBase = 0u;
        uint32_t yBase = 0u;
        float minBrightness = 0.0f;

        std::vector<CompType> NormalizedValues(uint32_t colorId) const;
        CompType ScaleX(CompType x);
        CompType ScaleY(CompType y);
        CompType MapCoord(CompType x, CompType minData, CompType maxData);
        CompType MapXCoord(CompType x);
        CompType MapYCoord(CompType y);
    public:
        HistogramHeatMap(
            const std::vector<glm::vec3>& colors,
            uint32_t sizeInPixels,
            uint32_t sizeInPoints);

        void Draw();
        void IncrementPoint(uint32_t colorId, CompType x, CompType y);
        void SetPointValue(uint32_t colorId, CompType x, CompType y, CompType value);
        void SetMinDataValues(CompType minDataX, CompType minDataY);
        void SetMaxDataValues(CompType maxDataX, CompType maxDataY);
        void Clear();
        void SetXLog(uint32_t base);
        void SetYLog(uint32_t base);
        void SetMinBrightness(float minBrightness);
        virtual ~HistogramHeatMap() = default;
    };
}

#endif
