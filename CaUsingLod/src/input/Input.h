/*
* Class which provides user input.
*/

#ifndef PCG_INPUT_H
#define PCG_INPUT_H

#include "UserInput.h"
#include "Key.h"
#include "MouseButton.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <vec2.hpp>

namespace pcg
{
    class Camera2d;

    class Input
    {
    private:
        std::vector<UserInput> keyStates;
        std::vector<UserInput> mouseButtonStates;
        std::vector<char> characters;
        glm::vec2 mousePosition;
        glm::vec2 mouseDelta;
        float mouseScroll;

        void InitializeKeyStates();
        void InitializeMouseButtonStates();
    public:
        Input();
        void SetKeyState(Key key, bool pressed);
        void SetMouseButtonState(MouseButton mouseButton, bool pressed);
        void SetMousePosition(glm::vec2 mousePosition);
        void SetMouseScroll(float scroll);

        [[nodiscard]] bool GetKeyDown(Key key) const;
        [[nodiscard]] bool GetKeyUp(Key key) const;
        [[nodiscard]] bool GetKeyPressed(Key key) const;
        [[nodiscard]] bool GetKeyReleased(Key key) const;

        [[nodiscard]] bool GetMouseDown(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMouseUp(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMousePressed(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMouseReleased(MouseButton mouseButton) const;

        [[nodiscard]] glm::vec2 GetMousePosition() const;
        [[nodiscard]] glm::vec2 GetMousePosition(const Camera2d& Camera2d) const;
        [[nodiscard]] glm::vec2 GetMouseDelta() const;
        [[nodiscard]] float GetMouseScroll() const;
    };
}

#endif
