/*
* Functions for helping with managing user input.
* Many of these functions are used for working with the GLFW input system.
*/

#ifndef PCG_INPUTHELPER_H
#define PCG_INPUTHELPER_H

#include "Key.h"
#include "MouseButton.h"

namespace pcg
{
    Key glfwKeyToKey(int glfwKey);
    int keyToGLFWKey(Key key);
    MouseButton glfwMouseButtonToMouseButton(int glfwMouseButton);
    int mouseButtonToGLFWMouseButton(MouseButton mouseButton);
    KeyIndexType getLastKeyIndex();
    MouseButtonIndexType getLastMouseButtonIndex();
    char keyToChar(Key key);
}

#endif
