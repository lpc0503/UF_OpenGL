// Include standard headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>
#include <unordered_map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
using namespace glm;

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "objloader.hpp"
#include "vboindexer.hpp"
//#include <controls.hpp>

#include "Renderer.h"
#include "Camera.h"
#include "Log.h"
#include "Model.h"
#include "Utils.h"
#include "Entity.h"
#include "PNTriangle.h"

static int SHADERMODE;

const int window_width = 1024, window_height = 768;

int InitWindow();
void InitOpenGL();
void PickObject();
void OnRenderScene();
void Cleanup();
void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseCallback(GLFWwindow*, int, int, int);

// ===============================================================
// GLOBAL VARIABLES
GLFWwindow* window;

GLuint gPickedIndex = -1;

#define STANDARD 0
#define TESSELATION 1
#define GEOMETRY 2

Ref<Model> BunnyModel;
Ref<Model> RobotArmModel;
//Ref<Model> TestModel;
Ref<Model> HeadModel;

Ref<Camera> g_Camera;

glm::vec4 g_ClearColor = {0.0f, 0.0f, 0.2f, 0.0f};
float g_MouseWheelFactor = 0.2;
glm::vec3 g_SunLight = {28.2f, -8.f, -3.6f};
// ===============================================================

int InitWindow() {
    if (!glfwInit()) {
        const char* des;
        glfwGetError(&des);
        ERROR_TAG(TAG_OPENGL, "Failed to initialize GLFW: {}", des);
        return -1;
    }

#if defined(__APPLE__)
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // FOR MAC
#elif defined(_WIN32)
    const char* glsl_version = "#version 460";
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

    window = glfwCreateWindow(window_width, window_height, "Po_Chuan,Liang(7336-5707)", nullptr, nullptr);
    if (window == nullptr) {
        const char* des;
        glfwGetError(&des);
        ERROR_TAG(TAG_OPENGL, "Failed to open GLFW window. {}", des);
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        ERROR_TAG_STR(TAG_OPENGL, "Failed to initialize GLAD");
        return -1;
    }

    GLenum error = glGetError();
    if(error != GL_NO_ERROR)
    {
        ERROR_TAG(TAG_OPENGL, "error code: {}", error);
        return -1;
    }

    glfwSwapInterval(1);

#if defined(WIN32)
    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        INFO_TAG(TAG_OPENGL, "Enable DEBUG_OUTPUT");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
#endif

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

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
    glfwSetCursorPos(window, static_cast<float>(window_width) / 2, static_cast<float>(window_height) / 2);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetScrollCallback(window, MouseWheelCallback);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    Renderer::Init();

    return 0;
}

void InitOpenGL() {
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
}

void PickObject() {

    Renderer::BeginPickingScene(g_Camera);

    Renderer::EndPickingScene();

	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];

    int winX, winY;
    int fx, fy;
    glfwGetWindowSize(window, &winX, &winY);
    glfwGetFramebufferSize(window, &fx, &fy);

    glReadPixels(xpos * (fx/winX), (window_height - ypos) * (fy/winY), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);
    INFO("pick {}", gPickedIndex);

    Renderer::ClearViewport(); // TODO: Draw this to a framebuffer don't draw it on screen = =
}

void OnInitScene()
{
    g_Camera = std::make_shared<Camera>(glm::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f));
    g_Camera->SetPosition(10.0f, 10.0f, 10.0f);
    g_Camera->LookAt(0.f, 0.f, 0.f); // TODO: impl left drag to move target

    SHADERMODE = STANDARD;
//    HeadModel = Model::LoadModel("../asset/test.mtl.obj", glm::vec4(1.0, 0.5, 0.5, 1.0));
//    HeadModel = Model::LoadModel("../asset/bunny.obj", glm::vec4(1.0, 0.5, 0.5, 1.0));
//
//    struct normalcnt {
//
//        glm::vec3 normal = glm::vec3(0.f, 0.f, 0.f);
//        double cnt = 0.f;
//    };
//
//    std::unordered_map<uint32_t, normalcnt> sum;
//    auto &hvertices = HeadModel->GetMeshes().front()->m_Vertices;
//    auto &hindex = HeadModel->GetMeshes().front()->m_Indices;
}

float CameraMoveSpeed = 5.f;
glm::vec3 CameraRotate = {18.320f, -44.f, 0.f};
glm::vec3 CameraPos = {0.f, 0.f, 10.f};
double PrevMouseX, PrevMouseY;
glm::vec3 BunnyPos = glm::vec3{0.f};
glm::vec3 BunnyScale = glm::vec3{1.f};
float pointSize = 3.f;
int pointRange = 0;
int triangleRange = 0;


void OnUpdateScene(float dt)
{
    glfwPollEvents();

    { // Camera rotate
        { // Camera Pan
            double x, y, offX, offY;
            glfwGetCursorPos(window, &x, &y);
            x = x - window_width / 2.f;
            y = window_height / 2.f - y;
            offX = x - PrevMouseX;
            offY = y - PrevMouseY;

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
            {
                CameraRotate.x += static_cast<float>(-offY) * 5.f * dt;
                CameraRotate.y += static_cast<float>(offX) * 5.f * dt;

                if(CameraRotate.x >= 90.f)
                    CameraRotate.x = 89.9f;
                if(CameraRotate.x <= -90.f)
                    CameraRotate.x = -89.9f;
            }

            PrevMouseX = x;
            PrevMouseY = y;
        }

//        auto EIndex = Entity::GetEntityByID(gPickedIndex);

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            CameraRotate.y += 5.f * CameraMoveSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            CameraRotate.y -= 5.f * CameraMoveSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            CameraRotate.x += 5.f * CameraMoveSpeed * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            CameraRotate.x -= 5.f * CameraMoveSpeed * dt;
        }

        glm::mat4 mat{1.f};
        // The order of rotation have to be x -> y or we have to deal with the gimbal lock
        mat = glm::rotate(mat, glm::radians(CameraRotate.x), glm::vec3{1.f, 0.f, 0.f});
        mat = glm::rotate(mat, glm::radians(CameraRotate.y), glm::vec3{0.f, 1.f, 0.f});

        auto tmp = glm::vec4{CameraPos, 1.f} * mat;
        g_Camera->SetPosition(glm::vec3(tmp.x, tmp.y, tmp.z)); // TODO: 需要理解????
    }
}

int qwe = 0;
void OnImGuiUpdate()
{
//    ImGui::ShowDemoWindow();
    ImGui::Begin("Settings");

    ImGui::DragFloat("Mouse Wheel", &g_MouseWheelFactor, 0.1f);

    ImGui::ColorEdit4("Background", glm::value_ptr(g_ClearColor));


    // ShaderMode
    if(ImGui::RadioButton("Standard shader", &SHADERMODE, STANDARD)) {

        Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(STANDARD));
    } ImGui::SameLine();

    if(ImGui::RadioButton("Tessellation shader", &SHADERMODE, TESSELATION)) {

        Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(TESSELATION));
    }

    if(ImGui::RadioButton("Geometry shader", &SHADERMODE, GEOMETRY)) {

        Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(GEOMETRY));
    }



    ImGui::SliderFloat("Speed", &CameraMoveSpeed, 1.f, 10.f);
    ImGui::DragFloat3("Pos", &CameraPos);
    ImGui::DragFloat3("Rotation", &CameraRotate);
    ImGui::DragFloat3("Pos", &BunnyPos);
    ImGui::DragFloat3("Scale", &BunnyScale);

    ImGui::DragFloat3("Light Dir", &g_SunLight, 0.2f);

    ImGui::DragFloat("Point Size", &pointSize, 0.1f);
//    ImGui::DragInt("Triangle Range", &triangleRange, 1, 0, HeadModel->GetMeshes().front()->m_Vertices.size());
    ImGui::DragInt("Point Range", &pointRange,1, 0, 2);

    ImGui::End();

}

bool DrawMeshLine = true;
bool TmpFlag = false;
void OnRenderScene()
{

    Renderer::BeginScene(g_Camera);
    Renderer::DrawGrid(5, 5);

    glm::vec4 color = {1.0f, 1.f, 1.f, 1.f};

    Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});

    glm::vec3 start = {-1.f, 1.f, -1.f};
    glm::vec3 end = {1.f, 1.f, 1.f};

    int num = 10;
    float stepX = fabs(end.x-start.x)/num;
    float stepZ = fabs(end.z-start.z)/num;
    for(float x = start.x; x <= end.x; x += stepX)
    {
        for(float z = start.z; z <= end.z; z += stepZ)
        {
            Renderer::DrawPoint({x, 1.f, z}, {1.f, 0.f, 0.f, 1.f}, 10.f);
        }
    }

//    Renderer::DrawMesh(HeadModel->GetMeshes().front(), BunnyPos, {0.f, 0.f, 0.f}, BunnyScale);
    auto sunDir = glm::normalize(g_Camera->GetDir());
    Renderer::DrawLine(g_SunLight, g_SunLight + sunDir * 0.5f, {1.f, 1.f, 0.f, 1.f});
    Renderer::EndScene();

    // Test
//    PickObject();
}

void Cleanup() {

	glfwTerminate();
}

void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    CameraPos.z += (float)-yoffset * g_MouseWheelFactor;
    if(CameraPos.z < 0)
        CameraPos.z = 0.f;
}

int b = 0;

bool hold = false;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    gPickedIndex = -1;

	if(action == GLFW_PRESS) {

        if(!hold) {

            switch (key){

                case GLFW_KEY_F:
                    DrawMeshLine = !DrawMeshLine;
                    Renderer::SetRendererMode(static_cast<Renderer::RendererMode>(DrawMeshLine));
                    break;
                case GLFW_KEY_B:
                    TmpFlag = !TmpFlag;
                    break;
                case GLFW_KEY_T:
                    Renderer::SetShaderMode(Renderer::GetShaderMode() == static_cast<Renderer::ShaderMode>(STANDARD) ?
                    static_cast<Renderer::ShaderMode>(TESSELATION) : static_cast<Renderer::ShaderMode>(STANDARD));
                    break;
                case GLFW_KEY_1:
                    break;
                case GLFW_KEY_2:
                    break;
                case GLFW_KEY_P:
                    break;
                case GLFW_KEY_S:
                    break;
                case GLFW_KEY_A:
                    break;
                case GLFW_KEY_D:
                    break;
                case GLFW_KEY_W:
                    break;
                case GLFW_KEY_SPACE:
                    break;
                default:
                    break;
            }
        }
	}
    if(action == GLFW_RELEASE) {

        hold = false;
    }
}

// Alternative way of triggering functions on mouse click events
void MouseCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        PickObject();
	}
}

int main() {
    // disable buffer
    setvbuf(stdout, NULL, _IONBF, 0);

#if 0
    INFO("Attach RenderDoc...");
    getchar();
#endif

	// Initialize window
	int errorCode = InitWindow();
	if (errorCode != 0)
		return errorCode;

	// Initialize OpenGL pipeline
    InitOpenGL();
    OnInitScene();

	// For speed computation
	double lastTime = glfwGetTime(), preTime = lastTime;
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
//			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

        Renderer::SetClearColor(g_ClearColor);
        Renderer::ClearViewport();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        OnImGuiUpdate();
        OnUpdateScene(currentTime-preTime);
        preTime = currentTime;
        OnRenderScene();

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
        // Swap buffers
        glfwSwapBuffers(window);
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    Renderer::Shutdown();

    Cleanup();
}