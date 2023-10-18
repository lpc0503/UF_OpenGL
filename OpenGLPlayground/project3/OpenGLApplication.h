#pragma once
#ifndef OPENGLPLAYGROUND_OPENGLAPPLICATION_H
#define OPENGLPLAYGROUND_OPENGLAPPLICATION_H

#include "Application.h"

class GLFWwindow;

class OpenGLApplication : public Application
{
public:
    explicit OpenGLApplication(int argc, char* argv[]);
    ~OpenGLApplication() override;

    bool Init() override;
    void Shutdown() override;
    void ProcessInput() override;
    void Update(float dt) override;
    void ImGuiUpdate() override;
    void Render() override;

    bool IsWindowRunning() override;

    friend class GLFWCallbackWrapper;
    class GLFWCallbackWrapper {
    public:
        GLFWCallbackWrapper() = delete;
        GLFWCallbackWrapper(const GLFWCallbackWrapper&) = delete;
        GLFWCallbackWrapper(GLFWCallbackWrapper&&) = delete;
        ~GLFWCallbackWrapper() = delete;

        static void SetApplication(Application *app);
        static void OnFrameBufferResize(GLFWwindow* window, int width, int height);
        static void OnMouseEvent(GLFWwindow* window, int button, int action, int mods);
        static void OnKeyboardEvent(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseWheelEvent(GLFWwindow* window, double xoffset, double yoffset);
    private:
        static OpenGLApplication* GetApp();
        static Application* s_App;
    };

    bool GetMouseButton(MouseButton btn) override;
    int GetKey(int key) override;

    float GetCurrentTime() override;
    glm::vec2 GetCursorPos() override;
    glm::vec2 GetWindowSize() override;

private:
    bool InitGlfwWindow();
    bool InitOpenGL();
    bool InitImGui();

    GLFWwindow *m_Window = nullptr;

#if defined(__APPLE__)
    const char* m_glsl_version = "#version 330";
#elif defined(_WIN32)
    const char* m_glsl_version = "#version 460";
#endif
};

#endif //OPENGLPLAYGROUND_OPENGLAPPLICATION_H
