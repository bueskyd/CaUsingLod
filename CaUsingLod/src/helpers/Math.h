#ifndef PCG_MATH_H
#define PCG_MATH_H

#include <glm.hpp>

namespace pcg
{
    glm::vec3 sphereToCartesianCoords(float yaw, float pitch);
    glm::vec2 cartesianToSphereCoords(const glm::vec3& direction);
}

#endif
