/*
* Enum class representing a button on the mouse.
*/

#ifndef PCG_MOUSEBUTTON_H
#define PCG_MOUSEBUTTON_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <type_traits>

namespace pcg
{
    enum class MouseButton
    {
        Left,
        Right,
        Middle,
        Button4,
        Button5,
        Button6,
        Button7,
        Button8,
        Last,
        Unknown
    };

    using MouseButtonIndexType = std::underlying_type<MouseButton>::type;

    MouseButtonIndexType GetMouseButtonIndex(MouseButton mouseButton);
}

#endif
