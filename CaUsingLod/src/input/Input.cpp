#include "Input.h"
#include "graphics/Camera2d.h"
#include "InputHelper.h"
#include "graphics/Window.h"

namespace pcg
{
    Input::Input()
    {
        InitializeKeyStates();
        InitializeMouseButtonStates();
    }

    void Input::InitializeKeyStates()
    {
        auto max = getLastKeyIndex();
        keyStates.resize(max);
    }

    void Input::InitializeMouseButtonStates()
    {
        auto max = getLastMouseButtonIndex();
        mouseButtonStates.resize(max);
    }

    void Input::SetKeyState(Key key, bool pressed)
    {
        auto index = GetKeyIndex(key);
        keyStates[index].SetState(pressed);
    }

    void Input::SetMouseButtonState(MouseButton mouseButton, bool pressed)
    {
        auto index = GetMouseButtonIndex(mouseButton);
        mouseButtonStates[index].SetState(pressed);
    }

    void Input::SetMousePosition(glm::vec2 mousePosition)
    {
        mouseDelta = mousePosition - this->mousePosition;
        this->mousePosition = mousePosition;
    }

    void Input::SetMouseScroll(float scroll)
    {
        mouseScroll = scroll;
    }

    bool Input::GetKeyDown(Key key) const
    {
        auto index = GetKeyIndex(key);
        return keyStates[index].IsDown();
    }

    bool Input::GetKeyUp(Key key) const
    {
        return !GetKeyDown(key);
    }
    
    bool Input::GetKeyPressed(Key key) const
    {
        auto index = GetKeyIndex(key);
        return keyStates[index].IsJustPressed();
    }
    
    bool Input::GetKeyReleased(Key key) const
    {
        return !GetKeyPressed(key);
    }
    
    bool Input::GetMouseDown(MouseButton mouseButton) const
    {
        auto index = GetMouseButtonIndex(mouseButton);
        return mouseButtonStates[index].IsDown();
    }
    
    bool Input::GetMouseUp(MouseButton mouseButton) const
    {
        return !GetMouseDown(mouseButton);
    }
    
    bool Input::GetMousePressed(MouseButton mouseButton) const
    {
        auto index = GetMouseButtonIndex(mouseButton);
        return mouseButtonStates[index].IsJustPressed();
    }
    
    bool Input::GetMouseReleased(MouseButton mouseButton) const
    {
        return !GetMousePressed(mouseButton);
    }
    
    glm::vec2 Input::GetMousePosition() const
    {
        return mousePosition;
    }
    
    glm::vec2 Input::GetMousePosition(const Camera2d& camera) const
    {
        glm::vec2 result = mousePosition + camera.GetPosition();
        return result;
    }

    glm::vec2 Input::GetMouseDelta() const
    {
        return mouseDelta;
    }

    float Input::GetMouseScroll() const
    {
        return mouseScroll;
    }
}
