#include "Heuristic.h"
#include <cmath>

namespace pcg
{
    uint32_t manhattan(glm::ivec2 from, glm::ivec2 to)
    {
        uint32_t dx = static_cast<uint32_t>(std::abs(from.x - to.x));
        uint32_t dy = static_cast<uint32_t>(std::abs(from.y - to.y));
        return dx + dy;
    }
}
