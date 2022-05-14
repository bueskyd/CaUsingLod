/*
* A class representing the current and previous states of either a key on the keyboard or a button on the mouse.
*/

#ifndef PCG_USERINPUT_H
#define PCG_USERINPUT_H

namespace pcg
{
    class UserInput
    {
    private:
        bool currentState = false;
        bool previousState = false;
    public:
        UserInput() = default;
        bool IsDown() const;
        void SetState(bool pressed);
        bool IsJustPressed() const;
        bool IsJustReleased() const;
    };
}

#endif
