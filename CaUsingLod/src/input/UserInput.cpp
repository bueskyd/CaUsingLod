#include "UserInput.h"

namespace pcg
{
    bool UserInput::IsDown() const
    {
        return currentState;
    }

    void UserInput::SetState(bool pressed)
    {
        previousState = currentState;
        currentState = pressed;
    }

    bool UserInput::IsJustPressed() const
    {
        return currentState && !previousState;
    }

    bool UserInput::IsJustReleased() const
    {
        return !currentState && previousState;
    }
}
