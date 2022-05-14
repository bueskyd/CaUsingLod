/*
* An implementation of the cellular automata for cave generator by Lawrence Johnson, Georgios N. Yannakis, and Julian Togelius.
* The implementation is described inn their paper "Cellular automata for real-time generation of infinite cave levels".
* Note that this implementation does not allow for infinite generation of caves.
*/

#ifndef PCG_CAVEGENERATOR_H
#define PCG_CAVEGENERATOR_H

#include "pcg/CellularAutomata.h"
#include "Generator.h"
#include <vec3.hpp>

namespace pcg
{
    class CaveGenerator : public Generator
    {
    public:
        struct Options
        {
            uint32_t r = 50u;
            uint32_t n = 4u;
            uint32_t t = 5u;
            uint32_t m = 1u;
        };
    private:
        Options options;
    public:
        CaveGenerator(uint32_t width, uint32_t height);
        void Generate() override;
        [[nodiscard]]
        std::vector<glm::vec3> GetCellColors() const override;
        void SetOptions(const Options& options);
    };
}

#endif
