/*
* Implementations of heuristics for pathfinding algorithms.
*/

#ifndef PCG_HEURISTIC_H
#define PCG_HEURISTIC_H

#include <vec2.hpp>

namespace pcg
{
    [[nodiscard]]
    uint32_t manhattan(glm::ivec2 first, glm::ivec2 second);
}

#endif
