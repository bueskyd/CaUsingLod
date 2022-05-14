/*
* Class representing a window. Essentially just a wrapper around a GLFWwindow object with extra functionality added.
* Provides functionality for working with user input.
*/

#ifndef PCG_WINDOW_H
#define PCG_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "input/Input.h"
#include "input/Key.h"
#include "input/MouseButton.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "Broadcaster.h"
#include "Callback.h"

namespace pcg
{
    class Window
    {
    public:
        struct Options
        {
            bool debug;
        };
        GLFWwindow* glfwWindow;
    private:
        std::string name;
        float defaultDiagonal = 0.0f;
        uint32_t width = 0u;
        uint32_t height = 0u;
        float diagonal = 0.0f;
        float diagonalRatio = 0.0f;
        Input input;
        using ResizeCallback = Callback<void()>;
        ActionBroadcaster resizeBroadcaster;
        bool cursorIsHidden = false;
        float totalMouseScroll = 0.0f;

        Window(
            uint32_t width, uint32_t height,
            const std::string& title, const std::string& name, const Options& options);
        float CalculateDiagonal(uint32_t width, uint32_t height) const;
        void UpdateDiagonal();
        void UpdateDiagonalRatio();
        void MoveFrom(Window&& other) noexcept;
        void UpdateKeyStates();
        void UpdateMouseButtonStates();
        void UpdateMousePosition();
        void UpdateSizeData(uint32_t width, uint32_t height);
        void UpdateMouseScroll();
    public:
        Window(const Window&) = delete;
        Window(Window&& other) noexcept;
        void NextFrame();
        glm::vec2 GetSize() const;
        int GetWidth() const;
        int GetHeight() const;
        void SetSize(uint32_t width, uint32_t height);
        bool ShouldClose();
        void SwapBuffers();
        void HideCursor(bool hide);
        bool IsCursorHidden() const;
        void RegisterResizeCallback(
            ActionBroadcaster::CallbackType& callback);
        void DeregisterResizeCallback(
            ActionBroadcaster::CallbackType& callback);
        
        [[nodiscard]] bool GetKeyDown(Key key) const;
        [[nodiscard]] bool GetKeyUp(Key key) const;
        [[nodiscard]] bool GetKeyPressed(Key key) const;
        [[nodiscard]] bool GetKeyReleased(Key key) const;

        [[nodiscard]] bool GetMouseDown(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMouseUp(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMousePressed(MouseButton mouseButton) const;
        [[nodiscard]] bool GetMouseReleased(MouseButton mouseButton) const;

        [[nodiscard]] glm::vec2 GetMousePosition() const;
        [[nodiscard]] glm::vec2 GetMousePosition(const Camera2d& camera) const;
        [[nodiscard]] glm::vec2 GetMouseDelta() const;
        [[nodiscard]] float GetMouseScroll() const;

        Window& operator=(const Window&) = delete;
        Window& operator=(Window&& other) noexcept;
        ~Window();

    private:
        static std::unordered_map<std::string, std::unique_ptr<Window>> windows;
        static std::unordered_map<GLFWwindow*, Window*> glfwWindowToWindow;
        static Window* focusedWindow;

        static Window* FindWindow(GLFWwindow* glfwWindow);
        static void OnWindowResize(GLFWwindow* glfwWindow, int width, int height);
        static void OnFocusChange(GLFWwindow* glfwWindow, int focused);
        static void OnMouseScroll(GLFWwindow* glfwWindow, double xOffset, double yOffset);
    public:
        static constexpr int defaultWidth = 1920;
        static constexpr int defaultHeight = 1080;
        static Window* InitWindow(
            uint32_t width,
            uint32_t height,
            const std::string& title,
            const std::string& name, 
            const Options& options);
        static void NextFrames();
        static Window* GetFocusedWindow();
        static void DestroyWindows();
    };
}

#endif
