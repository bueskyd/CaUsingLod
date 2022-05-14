/*
* A cellular automata for cave generation. The cellular automata is based on the generator by Lawrence Johnson, Georgios N. Yannakis, and Julian Togelius.
* The cellular automata extends upon their design by adding multiple layers of detail.
* Internally this is done by utilising the Scale function in the CellularAutomata class.
*/

#ifndef PCG_CAVELODGENERATOR_H
#define PCG_CAVELODGENERATOR_H

#include "pcg/CellularAutomata.h"
#include "Generator.h"
#include <vec3.hpp>

namespace pcg
{
    class CaveLodGenerator : public Generator
    {
    public:
        static constexpr uint32_t floor = 0u;
        static constexpr uint32_t rock = 1u;

        struct Options
        {
            uint32_t r = 50u;
            uint32_t n = 4u;
            uint32_t t = 5u;
            uint32_t m = 1u;
            uint32_t multiplier = 3u;
            std::function<uint32_t(const Options&, const CellularAutomata&, uint32_t, uint32_t)> initializer;
            std::function<uint32_t(const Options&, const CellularAutomata&, uint32_t, uint32_t)> rule =
                [](const Options& o, const CellularAutomata& ca, uint32_t x, uint32_t y)
            {
                uint32_t neighbourhood = ca.Moore(x, y, o.m, { rock }).size();
                if (neighbourhood >= o.t)
                    return rock;
                return floor;
            };
        };
    private:
        std::vector<Options> options;
    public:
        CaveLodGenerator(uint32_t width, uint32_t height);
        void Generate();
        [[nodiscard]]
        std::vector<glm::vec3> GetCellColors() const override;
        void SetOptions(Options options, uint32_t index);
    };
}

#endif
