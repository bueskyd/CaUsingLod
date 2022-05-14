#include "HistogramHeatMap.h"
#include <gtc/matrix_transform.hpp>

namespace pcg
{
    static glm::mat4 ortho(
        glm::vec2 position, glm::vec2 viewSize, float zoom)
    {
        float lowX = -viewSize.x / (2.0f * zoom);
        lowX += position.x;
        float highX = viewSize.x / (2.0f * zoom);
        highX += position.x;
        float lowY = -viewSize.y / (2.0f * zoom);
        lowY += position.y;
        float highY = viewSize.y / (2.0f * zoom);
        highY += position.y;
        return glm::ortho(lowX, highX, lowY, highY, 0.0f, 1.0f);
    }

    HistogramHeatMap::HistogramHeatMap(
        const std::vector<glm::vec3>& colors,
        uint32_t sizeInPixels,
        uint32_t sizeInPoints)
        :
        colors(colors),
        sizeInPixels(sizeInPixels),
        pointSize(sizeInPixels / sizeInPoints),
        sizeInPoints(sizeInPoints),
        minDataX(0u),
        minDataY(0u),
        maxDataX(sizeInPoints),
        maxDataY(sizeInPoints)
    {
        std::vector<std::pair<GLenum, std::string>> shaderPaths = {
                { GL_VERTEX_SHADER, "shaders/CellVertexShader.glsl" },
                { GL_FRAGMENT_SHADER, "shaders/CellFragmentShader.glsl" }
        };
        cellShader = std::make_unique<pcg::ShaderProgram>(shaderPaths);

        glm::mat4 screenOrtho = ortho(
            glm::vec2(sizeInPixels, sizeInPixels) / 2.0f,
            glm::vec2(sizeInPixels, sizeInPixels),
            1.0f);
        cellShader->BindUniformBuffer("Matrices");

        cellRenderer = std::make_unique<pcg::CellRenderer>(
            sizeInPixels * sizeInPixels * colors.size(),
            pointSize,
            cellShader.get());

        cellRenderer->SetCellSize(sizeInPixels / sizeInPoints);
        values.resize(colors.size());
        for (size_t i = 0; i < values.size(); i++)
            values[i].resize(sizeInPoints * sizeInPoints);
        Draw();
    }

    std::vector<CompType> HistogramHeatMap::NormalizedValues(uint32_t colorId) const
    {
        const auto& colorValues = values[colorId];
        std::vector<CompType> normalized;
        normalized.resize(colorValues.size());
        CompType max{};
        for (size_t i = 0; i < colorValues.size(); i++)
            max = std::max(max, colorValues[i]);
        for (size_t i = 0; i < colorValues.size(); i++)
            normalized[i] = colorValues[i] / max;
        return normalized;
    }

    CompType HistogramHeatMap::ScaleX(CompType x)
    {
        if (xBase)
            return std::log2(x) / std::log2(xBase);
        return x;
    }

    CompType HistogramHeatMap::ScaleY(CompType y)
    {
        if (xBase)
            return std::log2(y) / std::log2(yBase);
        return y;
    }

    static float mapBrightness(
        float brightness,
        float fromMin,
        float fromMax,
        float toMin,
        float toMax)
    {
        brightness -= fromMin;
        brightness /= fromMax - fromMin;
        brightness *= toMax - toMin;
        brightness += toMin;
        return brightness;
    }

    void HistogramHeatMap::Draw()
    {
        cellRenderer->Clear();
        for (size_t colorId = 0; colorId < values.size(); colorId++)
        {
            auto normalizedValues = NormalizedValues(colorId);
            for (size_t y = 0; y < sizeInPoints; y++)
            {
                for (size_t x = 0; x < sizeInPoints; x++)
                {
                    float brightness = normalizedValues[x + y * sizeInPoints];
                    //glm::vec3 color(brightness);
                    if (brightness == 0.0f)
                        continue;
                    float mappedBrighness = brightness / (1.0f - minBrightness);
                    mappedBrighness += minBrightness;
                    glm::vec3 color = colors[colorId] * mappedBrighness;
                    /*glm::vec3 color(0.0f);
                    if (brightness > 0.0f)
                    {
                        if (brightness <= 0.5f)
                        {
                            brightness = mapBrightness(brightness, 0.0f, 0.5f, minBrightness, 1.0f);
                            color.g = brightness;
                            color.b = 1.0f - color.g;
                        }
                        else
                        {
                            brightness = mapBrightness(brightness, 0.5f, 1.0f, 0.5f + minBrightness, 1.0f);
                            color.r = brightness;
                            color.g = 1.0f - color.r;
                        }
                    }*/
                    cellRenderer->Draw(
                        {
                            .position = glm::vec2(x * pointSize, y * pointSize),
                            .color = color
                        });
                }
            }
        }

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBlendFunc(GL_ONE, GL_ONE);
        glBlendEquation(GL_FUNC_ADD);
        cellRenderer->Render();
        glDisable(GL_BLEND);
    }

    CompType HistogramHeatMap::MapCoord(CompType coord, CompType minData, CompType maxData)
    {
        CompType f = coord - minData;
        CompType range = maxData - minData;
        CompType a = f /= range;
        f *= sizeInPoints - 1;
        return static_cast<CompType>(f);
    }

    CompType HistogramHeatMap::MapXCoord(CompType x)
    {
        x = ScaleX(x);
        return MapCoord(x, minDataX, maxDataX);
    }

    CompType HistogramHeatMap::MapYCoord(CompType y)
    {
        y = ScaleY(y);
        return MapCoord(y, minDataY, maxDataY);
    }

    void HistogramHeatMap::IncrementPoint(uint32_t colorId, CompType x, CompType y)
    {
        CompType mx = MapXCoord(x);
        CompType my = MapYCoord(y);
        values[colorId][mx + my * sizeInPoints]++;
    }

    void HistogramHeatMap::SetPointValue(uint32_t colorId, CompType x, CompType y, CompType value)
    {
        CompType mx = MapXCoord(x);
        CompType my = MapYCoord(y);
        values[colorId][mx + my * sizeInPoints] = value;
    }

    void HistogramHeatMap::SetMinDataValues(CompType minDataX, CompType minDataY)
    {
        this->minDataX = ScaleX(minDataX);
        this->minDataY = ScaleY(minDataY);
    }

    void HistogramHeatMap::SetMaxDataValues(CompType maxDataX, CompType maxDataY)
    {
        this->maxDataX = ScaleX(maxDataX);
        this->maxDataY = ScaleY(maxDataY);
    }

    void HistogramHeatMap::Clear()
    {
        cellRenderer->Clear();
        for (size_t i = 0; i < values.size(); i++)
        {
            memset(&values[i][0], 0, sizeof(CompType) * values[i].size());
        }
    }

    void HistogramHeatMap::SetXLog(uint32_t base)
    {
        this->xBase = base;
    }

    void HistogramHeatMap::SetYLog(uint32_t base)
    {
        this->yBase = base;
    }

    void HistogramHeatMap::SetMinBrightness(float minBrightness)
    {
        this->minBrightness = minBrightness;
    }
}
