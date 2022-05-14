#ifndef PCG_MATRICESUBO_H
#define PCG_MATRICESUBO_H

#include "UniformBuffer.h"
#include <mat4x4.hpp>
#include <memory>

namespace pcg
{
    class MatricesUbo
    {
    private:
        static std::unique_ptr<UniformBuffer> ubo;
        MatricesUbo() = delete;
    public:
        static void Initialize();
        static void SetViewProjection(const glm::mat4& viewProjection);
        static void SetAbsoluteOrtho(const glm::mat4& ortho);
        static void SetRelativeOrtho(const glm::mat4& ortho);
        static void SetCameraPosition(const glm::vec3& position);
    };
}

#endif
