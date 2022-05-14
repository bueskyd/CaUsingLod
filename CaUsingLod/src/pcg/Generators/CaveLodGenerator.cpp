#include "CaveLodGenerator.h"
#include "Random.h"
#include <iostream>

namespace pcg
{
    CaveLodGenerator::CaveLodGenerator(uint32_t width, uint32_t height)
        : Generator(width, height)
    {
        std::vector<Options> options
        {
            {
                /*.r = 50u,
                .n = 4,
                .t = 40,
                .m = 4,*/
                .initializer = [](const Options& o, const CellularAutomata& ca, uint32_t x, uint32_t y)
                {
                    static Random<uint32_t> random(1u, 100u);
                    return random.Get() <= o.r ? rock : floor;
                }
            },
            {
                /*.r = 85u,
                .n = 2u,
                .t = 3u,
                .m = 1u,*/

                /*.r = 40u,
                .n = 1u,
                .t = 25u,
                .m = 3u,*/

                /*.r = 85u,
                .n = 2u,
                .t = 13u,
                .m = 2u,*/

                /*.r = 75u,
                .n = 2u,
                .t = 4u,
                .m = 1u,*/

                .r = 75u,
                .n = 2u,
                .t = 6u,
                .m = 1u,

                .initializer = [](const Options& o, const CellularAutomata& ca, uint32_t x, uint32_t y)
                {
                    static Random<uint32_t> random(1u, 100u);
                    uint32_t cell = ca.GetCell(x, y).type;
                    /*if (cell == rock)
                        return random.Get() <= o.r ? rock : floor;
                    return floor;*/

                    if (random.Get() <= o.r)
                        return cell == rock ? floor : rock;
                    return cell;
                }
            }/*,
            {
                /*.r = 85u,
                .n = 2u,
                .t = 3u,
                .m = 1u,*/

                /*.r = 40u,
                .n = 1u,
                .t = 25u,
                .m = 3u,

                /*.r = 85u,
                .n = 2u,
                .t = 13u,
                .m = 2u,*/

                /*.r = 75u,
                .n = 2u,
                .t = 4u,
                .m = 1u,*/

                /*.r = 75u,
                .n = 2u,
                .t = 6u,
                .m = 1u,*/

                /*.initializer = [](const Options& o, const CellularAutomata& ca, uint32_t x, uint32_t y)
                {
                    static Random<uint32_t> random(1u, 100u);
                    uint32_t cell = ca.GetCell(x, y).type;
                    /*if (cell == rock)
                        return random.Get() <= o.r ? rock : floor;
                    return floor;*//*

                    if (random.Get() <= o.r)
                        return cell == rock ? floor : rock;
                    return cell;
                }
            }*/
        };
        this->options = options;
    }

    void CaveLodGenerator::Generate()
    {
        ca.Clear();

        for (size_t i = 0; i < options.size(); i++)
        {
            const auto& o = options[i];
            ca.SetInitializer(
                [&o](const CellularAutomata& ca, uint32_t x, uint32_t y)
                {
                    return o.initializer(o, ca, x, y);
                });

            ca.SetRule(
                [&o](const CellularAutomata& ca, uint32_t x, uint32_t y)
                {
                    return o.rule(o, ca, x, y);
                });

            ca.Initialize();
            ca.Generate(o.n);
            if (i < options.size() - 1ull)
                ca.Scale(o.multiplier);
        }
    }

    std::vector<glm::vec3> CaveLodGenerator::GetCellColors() const
    {
        return
        {
            glm::vec3(65.0f / 255.0f),
            glm::vec3(1.0f)
        };
    }

    void CaveLodGenerator::SetOptions(Options options, uint32_t index)
    {
        if (options.initializer == nullptr)
            options.initializer = this->options[index].initializer;
        if (options.rule == nullptr)
            options.rule = this->options[index].rule;
        this->options[index] = options;
    }
}
