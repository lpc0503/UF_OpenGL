#include "Application.h"
#include "Log.h"

Application::Application(int argc, char **argv)
{
    m_Argc = argc;
    m_Argv = argv;
}

Application::~Application()
{
}

void Application::Run()
{
    if(!Init())
    {
        ERROR_TAG(TAG_APP, "Failed to Initialize...");
        Shutdown();
        return;
    }

    float accTime = 0.f;
    while(IsWindowRunning())
    {
        m_FrameCount++;
        float dt = GetElapsedTime();
        accTime += dt;
        if(accTime >= 1.f)
        {
            m_FrameTime = 1000.f / static_cast<float>(m_FrameCount);
            m_FrameCount = 0;
            accTime -= 1.f;
        }

        ProcessInput();
        Update(dt);
        Render();
    }

    Shutdown();
}

float Application::GetElapsedTime()
{
    float now = GetCurrentTime();
    float elapsed = now - m_LastTime;
    m_LastTime = now;
    return elapsed;
}

bool Application::Init()
{
    return true;
}

void Application::Shutdown()
{
}
