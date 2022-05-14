#include "Camera2d.h"
#include <iostream>
#include <gtc/matrix_transform.hpp>
#include "graphics/uniformBuffers/MatricesUbo.h"

namespace pcg
{
    static glm::mat4 ortho(
        glm::vec2 position, glm::vec2 viewSize, float zoom)
    {
        float lowX = -viewSize.x / (2.0f * zoom);
        lowX += position.x;
        float highX = viewSize.x / (2.0f * zoom);
        highX += position.x;
        float lowY = -viewSize.y / (2.0f * zoom);
        lowY += position.y;
        float highY = viewSize.y / (2.0f * zoom);
        highY += position.y;
        return glm::ortho(lowX, highX, lowY, highY, 0.0f, 1.0f);
    }

    Camera2d::Camera2d(glm::vec2 position, float zoom, Window* window)
        : position(position), zoom(zoom), window(window)
    {
        glm::vec2 viewSize = window->GetSize();
        glm::mat4 orthoProjection = ortho(position, viewSize, zoom);
        MatricesUbo::SetAbsoluteOrtho(orthoProjection);

        glm::mat4 screenOrtho = ortho(viewSize / 2.0f, viewSize, 1.0f);
        MatricesUbo::SetRelativeOrtho(screenOrtho);

        resizeCallback = ActionCallback([this]() { OnWindowResize(); });
        window->RegisterResizeCallback(resizeCallback);
    }

    void Camera2d::OnWindowResize()
    {
        glm::vec2 viewSize = window->GetSize();
        glm::mat4 orthoProjection = ortho(position, viewSize, zoom);
        MatricesUbo::SetAbsoluteOrtho(orthoProjection);

        glm::mat4 screenOrtho = ortho(viewSize / 2.0f, viewSize, 1.0f);
        MatricesUbo::SetRelativeOrtho(screenOrtho);
    }

    void Camera2d::SetPosition(glm::vec2 position)
    {
        this->position = position;
        glm::mat4 orthoProjection = ortho(position, window->GetSize(), zoom);
        MatricesUbo::SetAbsoluteOrtho(orthoProjection);
    }

    void Camera2d::SetZoom(float zoom)
    {
        this->zoom = zoom;
        glm::mat4 orthoProjection = ortho(position, window->GetSize(), zoom);
        MatricesUbo::SetAbsoluteOrtho(orthoProjection);
    }

    glm::vec2 Camera2d::GetPosition() const
    {
        return position;
    }
}
