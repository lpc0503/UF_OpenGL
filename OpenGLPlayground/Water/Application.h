#pragma once
#ifndef OPENGLPLAYGROUND_APPLICATION_H
#define OPENGLPLAYGROUND_APPLICATION_H

#include <string>
#include "glm/vec2.hpp"

class Application
{
public:
    explicit Application(int argc, char* argv[]);
    virtual ~Application();

    virtual bool Init();
    virtual void Shutdown();
    virtual void ProcessInput() = 0;
    virtual void Update(float dt) = 0;
    virtual void ImGuiUpdate() = 0;
    virtual void Render() = 0;

    virtual bool IsWindowRunning() = 0;

    void Run();

    void SetWindowSize(int w, int h) { m_Width = w; m_Height = h; }

    // API
    virtual bool OnInit() = 0;
    virtual void OnShutdown() = 0;
    virtual void OnProcessInput() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnImGuiUpdate() = 0;
    virtual void OnRender() = 0;

    // TODO: remove after impl event class
    virtual void OnMouseEvent(int button, int action, int mods) {}
    virtual void OnKeyboardEvent(int key, int scancode, int action, int mods) {}
    virtual void OnMouseWheelEvent(double xoffset, double yoffset) {}

    virtual float GetCurrentTime() = 0;
    float GetElapsedTime();

    float GetFrameTime() const { return m_FrameTime; }
    float GetFps() const { return 1000.f / m_FrameTime; }
    int GetFrameCount() const { return m_FrameCount; }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    const std::string& GetWindowTitle() const { return m_WindowTitle; }
    void SetWindowTitle(const std::string& title) { m_WindowTitle = title; }

    virtual glm::vec2 GetCursorPos() = 0;
    enum MouseButton : uint8_t
    {
        Left = 1,
        Right,
        Middle,
        Mouse4,
        Mouse5,
        Mouse6,
        Mouse7,
        Mouse8,
    };
    virtual bool GetMouseButton(MouseButton btn) = 0;
    virtual int GetKey(int key) = 0; // TODO: Make custom keycode
    virtual glm::vec2 GetWindowSize() = 0;
    glm::vec2 GetFrameBufferSize() { return {m_Width, m_Height}; }

protected:
    enum Type : uint8_t
    {
        OpenGL,
        Count,
        Invalid = 0xFF,
    };
    Type m_ApType = Invalid;

    int m_Argc;
    char **m_Argv;

    float m_LastTime = 0.f;
    float m_FrameTime;
    int m_FrameCount = 0;

    int m_Width, m_Height; // TODO: Remove

    std::string m_WindowTitle;
};

#endif //OPENGLPLAYGROUND_APPLICATION_H
