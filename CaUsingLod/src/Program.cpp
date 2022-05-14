#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "contexts/CellContext.h"
#include "graphics/Window.h"
#include "GLDebug.h"
#include <iostream>
#include "graphics/uniformBuffers/MatricesUbo.h"
#include "graphics/uniformBuffers/UboBindings.h"
#include "input/InputHelper.h"

int main()
{
    if (!glfwInit())
        return -1;

    bool debug = false;
#ifdef _DEBUG
    debug = true;
#endif

    pcg::Window* mainWindow = pcg::Window::InitWindow(1000u, 1000u, "PCG", "main", { true });

    if (glewInit() != GLEW_OK)
        return -1;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (debug)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&pcg::glDebugOutput, nullptr);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    }

    pcg::initializeUboBindingPoints();
    pcg::MatricesUbo::Initialize();

    pcg::CellContext context(mainWindow);

    while (!mainWindow->ShouldClose())
    {
        context.Update();

        mainWindow->SwapBuffers();

        pcg::Window::NextFrames();

        if (mainWindow->GetKeyDown(pcg::Key::Escape))
            break;
    }

    pcg::Window::DestroyWindows();
    glfwTerminate();
    return 0;
}
