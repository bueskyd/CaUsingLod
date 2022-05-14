#include "Math.h"
#include <glm.hpp>

namespace pcg
{
    glm::vec3 sphereToCartesianCoords(float yaw, float pitch)
    {
        glm::vec3 direction;
        direction.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        direction.y = std::sin(glm::radians(pitch));
        direction.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        return direction;
    }

    glm::vec2 cartesianToSphereCoords(const glm::vec3& direction)
    {
        glm::vec3 normalizedDirection = glm::normalize(direction);
        float yaw = std::atan2(normalizedDirection.z, normalizedDirection.x);
        float pitch = std::asin(direction.y);
        return glm::vec2(glm::degrees(yaw), glm::degrees(pitch));
    }
}
