#include "CaveGenerator.h"
#include "Random.h"

namespace pcg
{
    static constexpr uint32_t floor = 0u;
    static constexpr uint32_t rock = 1u;

    CaveGenerator::CaveGenerator(uint32_t width, uint32_t height)
        : Generator(width, height)
    {
        /*options.r = 50u;
        options.n = 4;
        options.m = 4;
        options.t = 40;*/
        ca.SetInitializer(
            [this](const CellularAutomata& ca, uint32_t x, uint32_t y)
            {
                static Random<uint32_t> random(1u, 100u);
                return random.Get() <= options.r;
            });

        ca.SetRule(
            [this](const CellularAutomata& ca, uint32_t x, uint32_t y)
            {
                uint32_t neighbourhood = ca.Moore(x, y, options.m, { rock }).size();
                if (neighbourhood >= options.t)
                    return rock;
                return floor;
            });
    }

    void CaveGenerator::Generate()
    {
        ca.Clear();
        ca.Initialize();
        ca.Generate(options.n);
    }

    std::vector<glm::vec3> CaveGenerator::GetCellColors() const
    {
        return
        {
            glm::vec3(65.0f / 255.0f),
            glm::vec3(1.0f)
        };
    }

    void CaveGenerator::SetOptions(const Options& options)
    {
        this->options = options;
    }
}
