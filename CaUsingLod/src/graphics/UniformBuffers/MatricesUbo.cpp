#include "MatricesUbo.h"
#include "UboBindings.h"

namespace pcg
{
    std::unique_ptr<UniformBuffer> MatricesUbo::ubo;

    static constexpr size_t matrixCount = 3;
    static constexpr size_t floatCount =
        matrixCount * glm::mat4::length() * glm::vec4::length() + 3;
    static constexpr size_t viewProjectionOffset = 0;
    static constexpr size_t absoluteOrthoOffset =
        viewProjectionOffset + glm::mat4::length() * glm::vec4::length();
    static constexpr size_t relativeOrthoOffset =
        absoluteOrthoOffset + glm::mat4::length() * glm::vec4::length();
    static constexpr size_t cameraPositionOffset =
        relativeOrthoOffset + glm::mat4::length() * glm::vec4::length();

    void MatricesUbo::Initialize()
    {
        size_t uboSize = floatCount * sizeof(float);
        ubo = std::make_unique<pcg::UniformBuffer>(
            uboSize, GL_DYNAMIC_DRAW, getBindingPoint("Matrices"));
    }

    void MatricesUbo::SetViewProjection(const glm::mat4& viewProjection)
    {
        ubo->Write<float>(
            viewProjectionOffset, 
            &viewProjection[0][0], 
            viewProjection.length() * glm::vec4::length());
    }

    void MatricesUbo::SetAbsoluteOrtho(const glm::mat4& ortho)
    {
        ubo->Write<float>(
            absoluteOrthoOffset, 
            &ortho[0][0], 
            ortho.length() * glm::vec4::length());
    }

    void MatricesUbo::SetRelativeOrtho(const glm::mat4& ortho)
    {
        ubo->Write<float>(
            relativeOrthoOffset, 
            &ortho[0][0], 
            ortho.length() * glm::vec4::length());
    }

    void MatricesUbo::SetCameraPosition(const glm::vec3& position)
    {
        ubo->Write<float>(
            cameraPositionOffset, 
            &position[0], 
            position.length());
    }
}
