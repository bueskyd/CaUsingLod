/*
* Callback function for easier debugging of OpenGL.
*/

#ifndef PCG_GLDEBUG_H
#define PCG_GLDEBUG_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace pcg
{
    void APIENTRY glDebugOutput(
        GLenum source, 
        GLenum type, 
        unsigned int id, 
        GLenum severity, 
        GLsizei length, 
        const char* message, 
        const void* userParam);
}

#endif
