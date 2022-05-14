#ifndef PCG_CAMERA2D_H
#define PCG_CAMERA2D_H

#include <vec2.hpp>
#include "uniformBuffers/UniformBuffer.h"
#include <memory>
#include "Window.h"
#include "Callback.h"

namespace pcg
{
    class Camera2d
    {
    private:
        glm::vec2 position;
        float zoom;
        ActionCallback resizeCallback;
        Window* window;

        void OnWindowResize();
    public:
        Camera2d(glm::vec2 position, float zoom, Window* window);
        void SetPosition(glm::vec2 position);
        void SetZoom(float zoom);
        glm::vec2 GetPosition() const;
    };
}

#endif
