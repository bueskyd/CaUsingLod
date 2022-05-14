#include "CellContext.h"
#include "graphics/renderers/CellRenderer.h"
#include "graphics/VertexArrayBuilder.h"
#include "graphics/ShaderProgram.h"
#include <gtc/matrix_transform.hpp>
#include "graphics/uniformBuffers/UniformBuffer.h"
#include "graphics/Camera2d.h"
#include "graphics/uniformBuffers/UboBindings.h"
#include "external/stb_image/stb_image_write.h"
#include "graphics/RenderToFile.h"
#include <iostream>
#include <vec3.hpp>
#include <fstream>

namespace pcg
{
    CellContext::CellContext(Window* window)
        : window(window)
    {
        Initialize();
    }

    void CellContext::Initialize()
    {
        glEnable(GL_DEPTH_TEST);

        std::vector<std::pair<GLenum, std::string>> shaderPaths = {
            { GL_VERTEX_SHADER, "shaders/CellVertexShader.glsl" },
            { GL_FRAGMENT_SHADER, "shaders/CellFragmentShader.glsl" }
        };
        shaderProgram = std::make_unique<ShaderProgram>(shaderPaths);
        shaderProgram->BindUniformBuffer("Matrices");

        uint32_t size = std::pow(3u, 6u);
        window->SetSize(size, size);
        glm::vec2 position = window->GetSize() / 2.0f;
        camera = std::make_unique<Camera2d>(position, 1.0f, window);

        uint32_t cells = std::pow(3u, 3u);
        renderer = std::make_unique<CellRenderer>(
            size * size, 1u, shaderProgram.get());

        generator0 = std::make_unique<GeneratorType0>(cells * 3, cells * 3);
        generator1 = std::make_unique<GeneratorType1>(cells, cells);

        heatMap = std::make_unique<HistogramHeatMap>(
            std::vector<glm::vec3>{ glm::vec3(1.0f) },
            window->GetWidth(),
            81);
    }

    void CellContext::Update()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static int32_t r = 0;

        if (window->GetKeyPressed(Key::G))
        {
            auto& generator = *generator1.get();
            generator.Generate();
            DrawGenerated(generator);
            r = 1;
        }
        else if (window->GetKeyPressed(Key::H))
        {
            auto& generator = *generator0.get();
            generator.Generate();
            DrawGenerated(generator);
            r = 1;
        }
        else if (window->GetKeyPressed(Key::P))
        {
            renderer->Render();
            renderToFile("result.png", window->GetWidth(), window->GetHeight());
        }
        else if (window->GetKeyPressed(Key::A))
        {
            Analyze(GameplayType::TopDown);
            Analyze(GameplayType::SideScroller);
            r = 2;
        }
        if (r == 1)
            renderer->Render();
        else if (r == 2)
            heatMap->Draw();
    }

    void CellContext::ClearCells()
    {
        renderer->Clear();
    }

    template<typename GeneratorType>
    std::vector<std::pair<DataComponent, std::string>> calculators(GameplayType gameplayType)
    {
        //Top-down:
        //1. Generator::PercentageOfType
        //2. Generator::PercentageReachable
        //3. Generator::DistinctGroupSizes, Generator::DistinctLineLengths, Generator::DistinctBorderDistancePeaks
        //4. Generator::OptimalPathPercentageAverage
        //5. Generator::GroupCount, Generator::BorderDistances
        //6. Generator::StraightLines
        //7. Generator::OptimalPathLineFit
        //8. Generator::AverageBorderDistancePeaks

        //Side-scroller:
        //1. Generator::PercentageOfType
        //2. Generator:.AverageWalkablePercentage
        //3. Generator::DistinctGroupSizes, Generator::DistinctLineLengths, Generator::DistinctBorderDistancePeaks
        //4. Generator::OptimalPathPercentageAverage
        //5. Generator::GroupCount, Generator::GapCount
        //6. Generator::AverageHeightDifference
        //7. Generator::OptimalPathLineFit
        //8. Generator::OnSurfacePercentage

        auto variety = [](const auto& generator, const auto& analysis)
        {
            return
                Generator::DistinctGroupSizes()(generator, analysis) +
                Generator::DistinctLineLengths()(generator, analysis) +
                Generator::DistinctBorderDistancePeaks(0u)(generator, analysis);
        };

        if (gameplayType == GameplayType::TopDown)
        {
            auto leniency = [](const auto& generator, const auto& analysis)
            {
                return
                    Generator::GroupCount(1u, 1u, 10)(generator, analysis) +
                    Generator::AverageBorderDistancePeaks(0u)(generator, analysis);
            };

            std::vector<std::pair<DataComponent, std::string>> calculators
            {
                { Generator::PercentageOfType(0u), "1" },
                { Generator::PercentageReachable(0u), "2" },
                { variety, "3" },
                { Generator::OptimalPathPercentageAverage(), "4" },
                { leniency, "5" },
                { Generator::StraightLines(), "6" },
                { Generator::OptimalPathLineFit(), "7" },
                { Generator::AverageBorderDistancePeaks(1u), "8" }
            };
            return calculators;
        }
        else
        {
            auto leniency = [](const auto& generator, const auto& analysis)
            {
                return
                    Generator::GroupCount(1u, 1u, 10u)(generator, analysis) +
                    Generator::GapCount(5u, 1u)(generator, analysis);
            };

            std::vector<std::pair<DataComponent, std::string>> calculators
            {
                { Generator::PercentageOfType(0u), "1" },
                { Generator::AverageWalkablePercentage(1u) , "2"},
                { variety , "3" },
                { Generator::OptimalPathPercentageAverage() , "4" },
                { leniency , "5" },
                { Generator::AverageHeightDifference(1u) , "6"},
                { Generator::OptimalPathLineFit() , "7"},
                { Generator::OnSurfacePercentage(0u) , "8"}
            };
            return calculators;
        }
    }

    void CellContext::Analyze(GameplayType gameplayType)
    {
        auto topDownCostFunction = [](
            const auto& ca, const auto& from, const auto& to)
        {
            static uint32_t cellCosts[]
            {
                1u, 2u
            };
            uint32_t cost = cellCosts[ca.GetCell(to.x, to.y).type];
            uint32_t toCost = from.cost + cost;
            return toCost;
        };

        auto sideScrollerCostFunction = [](
            const auto& ca, const auto& from, const auto& to)
        {
            const auto& toCell = ca.GetCell(to.x, to.y);
            uint32_t cost = from.cost;
            if (toCell.type == 1u)
                cost += 2u; //Cost of digging through rock is 2
            else if (toCell.type == 0u)
            {
                if (to.y > 0u && ca.GetCell(to.x, to.y - 1u).type == 1u)
                    cost += 1u; //Cost of walking on a surface is 1
                else
                    cost += 3u; //Cost of builing is 3
            }
            return cost;
        };

        std::string folder;

        if (gameplayType == GameplayType::TopDown)
        {
            folder = "topDown";
            generator0->SetCostFunction(topDownCostFunction);
            generator1->SetCostFunction(topDownCostFunction);
        }
        else
        {
            folder = "sideScroller";
            generator0->SetCostFunction(sideScrollerCostFunction);
            generator1->SetCostFunction(sideScrollerCostFunction);
        }

        auto calculators0 = calculators<GeneratorType0>(gameplayType);
        auto calculators1 = calculators<GeneratorType1>(gameplayType);

        static std::vector<GeneratorType0::Options> options0
        {
            GeneratorType0::Options{ },
            GeneratorType0::Options{ .r = 35u },
            GeneratorType0::Options{ .r = 60u },
            GeneratorType0::Options{ .n = 2u },
            GeneratorType0::Options{ .t = 13u, .m = 2u },
            GeneratorType0::Options{ .n = 2, .t = 13u, .m = 2u },
            GeneratorType0::Options{ .n = 5, .t = 13u, .m = 2u },
            GeneratorType0::Options{ .n = 1, .t = 13u, .m = 2u },
            GeneratorType0::Options{ .r = 60u, .n = 5, .t = 13u, .m = 2u },
            GeneratorType0::Options{ .r = 60u, .n = 2, .t = 13u, .m = 2u },
            GeneratorType0::Options{ .n = 1, .t = 15u, .m = 2u },
            GeneratorType0::Options{ .n = 4, .t = 24u, .m = 3u },
            GeneratorType0::Options{ .n = 1, .t = 24u, .m = 3u }
        };

        static std::vector<GeneratorType1::Options> options1
        {
            GeneratorType1::Options{ },
            GeneratorType1::Options{ .r = 35u },
            GeneratorType1::Options{ .r = 60u },
            GeneratorType1::Options{ .n = 2u },
            GeneratorType1::Options{ .t = 13u, .m = 2u },
            GeneratorType1::Options{ .n = 2, .t = 13u, .m = 2u },
            GeneratorType1::Options{ .n = 5, .t = 13u, .m = 2u },
            GeneratorType1::Options{ .n = 1, .t = 13u, .m = 2u },
            GeneratorType1::Options{ .r = 60u, .n = 5, .t = 13u, .m = 2u },
            GeneratorType1::Options{ .r = 60u, .n = 2, .t = 13u, .m = 2u },
            GeneratorType1::Options{ .n = 1, .t = 15u, .m = 2u },
            GeneratorType1::Options{ .n = 4, .t = 24u, .m = 3u },
            GeneratorType1::Options{ .n = 1, .t = 24u, .m = 3u }
        };

        static std::vector<GeneratorType1::Options> options1_1
        {
            GeneratorType1::Options{ },
            GeneratorType1::Options{.r = 35u },
            GeneratorType1::Options{.r = 60u },
            GeneratorType1::Options{.n = 2u },
            GeneratorType1::Options{.t = 13u, .m = 2u },
            GeneratorType1::Options{.n = 2, .t = 13u, .m = 2u },
            GeneratorType1::Options{.n = 5, .t = 13u, .m = 2u },
            GeneratorType1::Options{.n = 1, .t = 13u, .m = 2u },
            GeneratorType1::Options{.r = 60u, .n = 5, .t = 13u, .m = 2u },
            GeneratorType1::Options{.r = 60u, .n = 2, .t = 13u, .m = 2u },
            GeneratorType1::Options{.n = 1, .t = 15u, .m = 2u },
            GeneratorType1::Options{.n = 4, .t = 24u, .m = 3u },
            GeneratorType1::Options{.n = 1, .t = 24u, .m = 3u },

            GeneratorType1::Options{.r = 85u, .n = 2u, .t = 3u, .m = 1u },
            GeneratorType1::Options{.r = 40u, .n = 1u, .t = 25u, .m = 3u },
            GeneratorType1::Options{.r = 85u, .n = 2u, .t = 13u, .m = 2u },
            GeneratorType1::Options{.r = 75u, .n = 2u, .t = 4u, .m = 1u },
            GeneratorType1::Options{.r = 75u, .n = 2u, .t = 6u, .m = 1u }
        };

        std::ofstream combined(folder + "/analysisData/combined.txt");
        for (size_t i = 0ull; i < calculators0.size(); i++)
        {
            const auto& [calculateX0, xName0] = calculators0[i];
            const auto& [calculateX1, xName1] = calculators1[i];
            for (size_t j = i + 1ull; j < calculators0.size(); j++)
            {
                const auto& [calculateY0, yName0] = calculators0[j];
                const auto& [calculateY1, yName1] = calculators1[j];

                std::cout << "Analysis " << xName0 << ", " << yName0 << "\n";
                combined << "Analysis " << xName0 << ", " << yName0 << "\n";

                auto optionSetter0 = [](auto& generator, uint32_t index)
                {
                    generator.SetOptions(options0[index]);
                };

                auto optionSetter1 = [](auto& generator, uint32_t index)
                {
                    generator.SetOptions(options1[index / options1_1.size()], 0);
                    generator.SetOptions(options1_1[index % options1_1.size()], 1);
                };

                uint32_t iterationMultiplier = 20u;

                auto data0 = dataPoints<GeneratorType0>(
                    *generator0.get(),
                    options0.size(),
                    calculateX0,
                    calculateY0,
                    iterationMultiplier * options1_1.size(),
                    optionSetter0,
                    false);

                auto data1 = dataPoints<GeneratorType1>(
                    *generator1.get(),
                    options1.size() * options1_1.size(),
                    calculateX1,
                    calculateY1,
                    iterationMultiplier,
                    optionSetter1,
                    false);

                CompType minX = std::min(data0.minX, data1.minX);
                CompType maxX = std::max(data0.maxX, data1.maxX);
                CompType minY = std::min(data0.minY, data1.minY);
                CompType maxY = std::max(data0.maxY, data1.maxY);

                heatMap->Clear();

                std::cout << "minX: " << minX << "\n";
                std::cout << "maxX: " << maxX << "\n";
                std::cout << "minY: " << minY << "\n";
                std::cout << "maxY: " << maxY << "\n";
                std::cout << "Clustering 0: " << clustering(data0) << "\n";
                std::cout << "Clustering 1: " << clustering(data1) << "\n";
                auto sc0 = spaceCoverage(data0, minX, minY, maxX, maxY);
                auto sc1 = spaceCoverage(data1, minX, minY, maxX, maxY);
                std::cout << "Space coverage 0: " << sc0 << "\n";
                std::cout << "Space coverage 1: " << sc1 << "\n";
                std::cout << "\n";

                combined << "minX: " << minX << "\n";
                combined << "maxX: " << maxX << "\n";
                combined << "minY: " << minY << "\n";
                combined << "maxY: " << maxY << "\n";
                combined << "Clustering 0: " << clustering(data0) << "\n";
                combined << "Clustering 1: " << clustering(data1) << "\n";
                combined << "Space coverage 0: " << sc0 << "\n";
                combined << "Space coverage 1: " << sc1 << "\n";
                combined << "\n";

                heatMap->SetMinDataValues(minX, minY);
                heatMap->SetMaxDataValues(maxX, maxY);

                for (const auto& dataPoint : data0.dataPoints)
                    heatMap->IncrementPoint(0, dataPoint.x, dataPoint.y);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                std::string fileId = xName0;
                fileId += yName0;
                heatMap->SetMinBrightness(0.0f);
                heatMap->Draw();
                renderToFile(folder + "/analysisHeatMaps0/" + fileId + "_0.png", window->GetWidth(), window->GetHeight());
                heatMap->SetMinBrightness(0.05f);
                heatMap->Draw();
                renderToFile(folder + "/analysisHeatMaps1/" + fileId + "_0.png", window->GetWidth(), window->GetHeight());

                heatMap->Clear();
                for (const auto& dataPoint : data1.dataPoints)
                    heatMap->IncrementPoint(0, dataPoint.x, dataPoint.y);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                heatMap->SetMinBrightness(0.0f);
                heatMap->Draw();
                renderToFile(folder + "/analysisHeatMaps0/" + fileId + "_1.png", window->GetWidth(), window->GetHeight());
                heatMap->SetMinBrightness(0.05f);
                heatMap->Draw();
                renderToFile(folder + "/analysisHeatMaps1/" + fileId + "_1.png", window->GetWidth(), window->GetHeight());

                std::ofstream out(folder + "/analysisData/" + fileId + ".txt");
                out <<
                    "minX: " << minX << "\n" <<
                    "maxX: " << maxX << "\n" <<
                    "minY: " << minY << "\n" <<
                    "maxY: " << maxY << "\n" <<
                    "Clustering 0: " << clustering(data0) << "\n" <<
                    "Clustering 1: " << clustering(data1) << "\n" <<
                    "Space coverage 0: " << sc0 << "\n" <<
                    "Space coverage 1: " << sc1 << "\n";
            }
        }
    }

    void CellContext::DrawGenerated(const Generator& generator)
    {
        const auto& cells = generator.GetResult();
        auto colors = generator.GetCellColors();
        uint32_t width = generator.GetWidth();
        uint32_t height = generator.GetHeight();

        renderer->Clear();
        uint32_t cellSize = window->GetWidth() / width;
        renderer->SetCellSize(cellSize);
        for (size_t y = 0; y < height; y++)
            for (size_t x = 0; x < width; x++)
            {
                glm::vec3 color = colors[cells[x + y * width].type];
                pcg::CellShape cell
                {
                    glm::vec2(x * cellSize, y * cellSize),
                    color
                };
                renderer->Draw(cell);
            }
    }
}
