#pragma once
#ifndef OPENGLPLAYGROUND_APPLICATION_H
#define OPENGLPLAYGROUND_APPLICATION_H

#include <string>

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

protected:
    int m_Argc;
    char **m_Argv;

    virtual float GetCurrentTime() = 0;
    float GetElapsedTime();

    float GetFrameTime() const { return m_FrameTime; }
    float GetFps() const { return 1000.f / m_FrameTime; }
    int GetFrameCount() const { return m_FrameCount; }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    const std::string& GetWindowTitle() const { return m_WindowTitle; }
    void SetWindowTitle(const std::string& title) { m_WindowTitle = title; }

private:
    float m_LastTime = 0.f;
    float m_FrameTime;
    int m_FrameCount = 0;

    int m_Width, m_Height; // TODO: Remove

    std::string m_WindowTitle;
};

#endif //OPENGLPLAYGROUND_APPLICATION_H
