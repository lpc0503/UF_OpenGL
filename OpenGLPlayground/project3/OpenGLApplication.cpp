#include "OpenGLApplication.h"
#include "Log.h"
#include "Utils.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


#define GLFW_ERROR(x) \
    do {                   \
        const char* msg; \
        glfwGetError(&msg); \
        ERROR_TAG(TAG_APP, "{}: {}", (x), msg); \
    } while(0)

OpenGLApplication::OpenGLApplication(int argc, char **argv)
    : Application(argc, argv)
{
}

OpenGLApplication::~OpenGLApplication()
{
}

bool OpenGLApplication::Init()
{
    if(!Application::Init())
        return false;

    if(!InitGlfwWindow())
        return false;

    if(!InitOpenGL())
        return false;

    if(!InitImGui())
        return false;

    Renderer::Init();

    OnInit();

    return true;
}

void OpenGLApplication::Shutdown()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Renderer::Shutdown();

    Application::Shutdown();

    glfwTerminate();
}

void OpenGLApplication::ProcessInput()
{
    glfwPollEvents();

    OnProcessInput();
}

void OpenGLApplication::Update(float dt)
{
    OnUpdate(dt);
}

void OpenGLApplication::ImGuiUpdate()
{
    OnImGuiUpdate();
}

void OpenGLApplication::Render()
{
    Renderer::SetClearColor({0.0f, 0.0f, 0.2f, 0.0f}); // TODO: expose
    Renderer::ClearViewport();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiUpdate();

    OnRender();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
#endif

    glfwSwapBuffers(m_Window);
}

bool OpenGLApplication::IsWindowRunning()
{
    auto title = GetWindowTitle() + fmt::format("[FPS: {:.2f}, {:.2f} ms/frame]", GetFps(), GetFrameTime());
    glfwSetWindowTitle(m_Window, title.c_str());
    return !glfwWindowShouldClose(m_Window);;
}

bool OpenGLApplication::InitGlfwWindow()
{
    if(glfwInit() == GLFW_FALSE)
    {
        GLFW_ERROR("Failed to initialize GLFW");
        return false;
    }

#if defined(__APPLE__)
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC
#elif defined(_WIN32)
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    // Init Window
    m_Window = glfwCreateWindow(GetWidth(), GetHeight(), GetWindowTitle().c_str(), nullptr, nullptr);
    if(!m_Window)
    {
        GLFW_ERROR("Failed to open GLFW window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ERROR_TAG_STR(TAG_OPENGL, "Failed to initialize GLAD");
        return false;
    }

    glfwSwapInterval(1);

    // TODO
    glfwSetInputMode(m_Window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(m_Window, GetWidth() / 2.f, GetHeight() / 2.f);
//    glfwSetMouseButtonCallback(window, MouseCallback);
//    glfwSetKeyCallback(window, KeyCallback);
//    glfwSetScrollCallback(window, MouseWheelCallback);
    GLFWCallbackWrapper::SetApplication(this);
    glfwSetFramebufferSizeCallback(m_Window, GLFWCallbackWrapper::OnFrameBufferResize);
    return true;
}

bool OpenGLApplication::InitOpenGL()
{
#if defined(WIN32)
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        INFO_TAG(TAG_OPENGL, "Enable DEBUG_OUTPUT");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr); // TODO: move glDebugOutput to here
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
#endif

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
//    glEnable(GL_CULL_FACE);
    return true;
}

bool OpenGLApplication::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init(m_glsl_version);

    return true;
}

float OpenGLApplication::GetCurrentTime()
{
    return (float)glfwGetTime();
}

//
Application* OpenGLApplication::GLFWCallbackWrapper::s_App = nullptr;

void OpenGLApplication::GLFWCallbackWrapper::SetApplication(Application *app)
{
    s_App = app;
}

OpenGLApplication* OpenGLApplication::GLFWCallbackWrapper::GetApp()
{
    return dynamic_cast<OpenGLApplication*>(s_App);
}

void OpenGLApplication::GLFWCallbackWrapper::OnFrameBufferResize(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
