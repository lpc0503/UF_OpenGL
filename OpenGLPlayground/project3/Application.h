#pragma once
#ifndef OPENGLPLAYGROUND_APPLICATION_H
#define OPENGLPLAYGROUND_APPLICATION_H

class Application
{
public:
    explicit Application(int argc, char* argv[]);
    virtual ~Application();

    virtual bool Init();
    virtual void Shutdown();
    virtual void ProcessInput() = 0;
    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

    virtual bool IsWindowRunning() = 0;

    void Run();

    void SetWindowSize(int w, int h) { m_Width = w; m_Height = h; }

    // API
    virtual bool OnInit() = 0;
    virtual void OnShutdown() = 0;
    virtual void OnProcessInput() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnRender() = 0;

protected:
    int m_Argc;
    char **m_Argv;

    virtual float GetCurrentTime() = 0;
    float GetElapsedTime();
    float m_LastTime = 0.f;

    int m_Width, m_Height;
};

#endif //OPENGLPLAYGROUND_APPLICATION_H
