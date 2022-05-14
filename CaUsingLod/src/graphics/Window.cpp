#include "Window.h"
#include "input/InputHelper.h"
#include "input/Key.h"
#include "input/MouseButton.h"
#include "input/InputHelper.h"
#include <iostream>
#include "StringConversions.h"

namespace pcg
{
    Window::Window(
        uint32_t width, uint32_t height,
        const std::string& title, 
        const std::string& name, 
        const Options& options)
        : 
        width(width), 
        height(height), 
        name(name)
    {
        if (options.debug)
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindow = glfwCreateWindow(
            static_cast<int>(width),
            static_cast<int>(height),
            title.c_str(),
            nullptr, nullptr);

        if (!glfwWindow)
            std::cerr << "Window creation failed!\n";

        glfwMakeContextCurrent(glfwWindow);
        glfwSwapInterval(1);

        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        glfwSetWindowSizeCallback(glfwWindow, OnWindowResize);
        glfwSetWindowFocusCallback(glfwWindow, OnFocusChange);

        glfwSetScrollCallback(glfwWindow, OnMouseScroll);

        defaultDiagonal = CalculateDiagonal(defaultWidth, defaultHeight);
        UpdateDiagonal();
        UpdateDiagonalRatio();
        if (focusedWindow == nullptr)
            focusedWindow = this;
    }

    Window::Window(Window&& other) noexcept
    {
        MoveFrom(std::move(other));
    }

    void Window::UpdateKeyStates()
    {
        auto max = GetKeyIndex(Key::Last);
        for (KeyIndexType keyIndex = 0; keyIndex < max; ++keyIndex)
        {
            auto key = static_cast<Key>(keyIndex);
            int glfwKey = keyToGLFWKey(key);
            int pressed = glfwGetKey(glfwWindow, glfwKey);
            input.SetKeyState(key, pressed);
        }
    }

    void Window::UpdateMouseButtonStates()
    {
        auto max = GetMouseButtonIndex(MouseButton::Last);
        for (
            MouseButtonIndexType mouseButtonIndex = 0;
            mouseButtonIndex < max;
            ++mouseButtonIndex)
        {
            auto mouseButton =
                static_cast<MouseButton>(mouseButtonIndex);
            int glfwMouseButton = mouseButtonToGLFWMouseButton(
                mouseButton);
            int pressed = glfwGetMouseButton(glfwWindow, glfwMouseButton);
            input.SetMouseButtonState(mouseButton, pressed);
        }
    }

    void Window::UpdateMousePosition()
    {
        double x, y;
        glfwGetCursorPos(glfwWindow, &x, &y);
        y = height - y;
        input.SetMousePosition(glm::vec2(x, y));
    }

    void Window::NextFrame()
    {
        UpdateKeyStates();
        UpdateMouseButtonStates();
        UpdateMousePosition();
        UpdateMouseScroll();
    }

    glm::vec2 Window::GetSize() const
    {
        return glm::vec2(width, height);
    }

    int Window::GetWidth() const
    {
        return width;
    }

    int Window::GetHeight() const
    {
        return height;
    }

    void Window::SetSize(uint32_t width, uint32_t height)
    {
        glfwSetWindowSize(glfwWindow, width, height);
        glViewport(0, 0, width, height);
    }

    void Window::UpdateSizeData(uint32_t width, uint32_t height)
    {
        this->width = width;
        this->height = height;
        UpdateDiagonal();
        UpdateDiagonalRatio();
    }

    void Window::UpdateMouseScroll()
    {
        input.SetMouseScroll(totalMouseScroll);
        totalMouseScroll = 0.0f;
    }

    float Window::CalculateDiagonal(uint32_t width, uint32_t height) const
    {
        return std::sqrt(
            static_cast<float>(width * width) +
            static_cast<float>(height * height));
    }

    void Window::UpdateDiagonal()
    {
        diagonal = CalculateDiagonal(width, height);
    }

    void Window::UpdateDiagonalRatio()
    {
        diagonalRatio = diagonal / defaultDiagonal;
    }

    void Window::MoveFrom(Window&& other) noexcept
    {
        glfwWindow = other.glfwWindow;
        other.glfwWindow = nullptr;
        name = std::move(other.name);
        width = other.width;
        other.width = 0;
        height = other.height;
        other.height = 0;
        diagonal = other.diagonal;
        other.diagonal = 0.0f;
        diagonalRatio = other.diagonalRatio;
        other.diagonalRatio = 0.0f;
        input = std::move(other.input);
    }

    bool Window::ShouldClose()
    {
        return glfwWindowShouldClose(glfwWindow);
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(glfwWindow);
    }

    void Window::HideCursor(bool hide)
    {
        int cursorMode = hide ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, cursorMode);
        cursorIsHidden = hide;
    }

    bool Window::IsCursorHidden() const
    {
        return cursorIsHidden;
    }

    void Window::RegisterResizeCallback(
        ActionBroadcaster::CallbackType& callback)
    {
        resizeBroadcaster.Register(callback);
    }

    void Window::DeregisterResizeCallback(
        ActionBroadcaster::CallbackType& callback)
    {
        resizeBroadcaster.Deregister(callback);
    }

    bool Window::GetKeyDown(Key key) const
    {
        return input.GetKeyDown(key);
    }

    bool Window::GetKeyUp(Key key) const
    {
        return input.GetKeyUp(key);
    }

    bool Window::GetKeyPressed(Key key) const
    {
        return input.GetKeyPressed(key);
    }

    bool Window::GetKeyReleased(Key key) const
    {
        return input.GetKeyReleased(key);
    }

    bool Window::GetMouseDown(MouseButton mouseButton) const
    {
        return input.GetMouseDown(mouseButton);
    }

    bool Window::GetMouseUp(MouseButton mouseButton) const
    {
        return input.GetMouseUp(mouseButton);
    }

    bool Window::GetMousePressed(MouseButton mouseButton) const
    {
        return input.GetMousePressed(mouseButton);
    }

    bool Window::GetMouseReleased(MouseButton mouseButton) const
    {
        return input.GetMouseReleased(mouseButton);
    }

    glm::vec2 Window::GetMousePosition() const
    {
        return input.GetMousePosition();
    }

    glm::vec2 Window::GetMousePosition(const Camera2d& Camera2d) const
    {
        return input.GetMousePosition(Camera2d);
    }

    glm::vec2 Window::GetMouseDelta() const
    {
        return input.GetMouseDelta();
    }

    float Window::GetMouseScroll() const
    {
        return input.GetMouseScroll();
    }

    Window& Window::operator=(Window&& other) noexcept
    {
        if (this != &other)
            MoveFrom(std::move(other));
        return *this;
    }

    Window::~Window()
    {
        glfwWindowToWindow.erase(glfwWindow);
        glfwDestroyWindow(glfwWindow);
    }

    std::unordered_map<std::string, std::unique_ptr<Window>> Window::windows;
    std::unordered_map<GLFWwindow*, Window*> Window::glfwWindowToWindow;
    Window* Window::focusedWindow = nullptr;

    Window* Window::InitWindow(
        uint32_t width,
        uint32_t height,
        const std::string& title,
        const std::string& name, const Options& options)
    {
        std::unique_ptr<Window> window(
            new Window(width, height, title, name, options));
        Window* windowPtr = window.get();
        glfwWindowToWindow.insert({ window->glfwWindow, windowPtr });
        windows.insert({ name, std::move(window) });
        return windowPtr;
    }

    void Window::NextFrames()
    {
        glfwPollEvents();
        for (auto& [name, window] : windows)
            window->NextFrame();
    }

    Window* Window::GetFocusedWindow()
    {
        return focusedWindow;
    }

    Window* Window::FindWindow(GLFWwindow* glfwWindow)
    {
        auto it = glfwWindowToWindow.find(glfwWindow);
        if (it == glfwWindowToWindow.end())
            std::cerr << "GLFWwindow could not be found in map of window\n";
        return it->second;
    }

    void Window::OnWindowResize(GLFWwindow* glfwWindow, int width, int height)
    {
        Window* window = FindWindow(glfwWindow);
        window->UpdateSizeData(width, height);
        window->resizeBroadcaster.Broadcast();
    }

    void Window::OnFocusChange(GLFWwindow* glfwWindow, int focused)
    {
        if (focused == GLFW_TRUE)
        {
            Window* window = FindWindow(glfwWindow);
            focusedWindow = window;
        }
    }

    void Window::OnMouseScroll(
        GLFWwindow* glfwWindow, double xOffset, double yOffset)
    {
        auto it = glfwWindowToWindow.find(glfwWindow);
        if (it != glfwWindowToWindow.end())
        {
            auto& window = it->second;
            window->totalMouseScroll += yOffset;
        }
    }

    void Window::DestroyWindows()
    {
        for (auto& [name, window] : windows)
            glfwWindowToWindow.erase(window->glfwWindow);
        windows.clear();
    }
}
