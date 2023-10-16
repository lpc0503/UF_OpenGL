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

    while(IsWindowRunning())
    {
        float dt = GetElapsedTime();
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
