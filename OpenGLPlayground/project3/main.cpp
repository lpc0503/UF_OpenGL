// Include standard headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <array>
#include <stack>   
#include <sstream>

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

const int window_width = 1024, window_height = 768;

struct Vertex2 {
	float Position[4];
	float Color[4];
	float Normal[3];
    float UV[2];
	void SetPosition(float *coords) {
		Position[0] = coords[0];
		Position[1] = coords[1];
		Position[2] = coords[2];
		Position[3] = 1.0;
	}
	void SetColor(float *color) {
		Color[0] = color[0];
		Color[1] = color[1];
		Color[2] = color[2];
		Color[3] = color[3];
	}
	void SetNormal(float *coords) {
		Normal[0] = coords[0];
		Normal[1] = coords[1];
		Normal[2] = coords[2];
	}
    void SetUV(float* uv) {
        UV[0] = uv[0];
        UV[1] = uv[1];
    }
};

// function prototypes
int InitWindow();
void InitOpenGL();
void CreateVAOs(Vertex2[], GLushort[], int);
void LoadObject(char*, glm::vec4, Vertex2* &, GLushort* &, int);
void CreateObjects();
void PickObject();
void OnRenderScene();
void Cleanup();
void MouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);
void KeyCallback(GLFWwindow*, int, int, int, int);
void MouseCallback(GLFWwindow*, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex = -1;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

const GLuint NumObjects = 1;	// ATTN: THIS NEEDS TO CHANGE AS YOU ADD NEW OBJECTS
GLuint VertexArrayId[NumObjects];
GLuint VertexBufferId[NumObjects];
GLuint IndexBufferId[NumObjects];

// TL
size_t VertexBufferSize[NumObjects];
size_t IndexBufferSize[NumObjects];
size_t NumIdcs[NumObjects];
size_t NumVerts[NumObjects];

GLint MatrixID;
GLint ModelMatrixID;
GLint ViewMatrixID;
GLint ProjMatrixID;
GLint PickingMatrixID;
GLint pickingColorID;
GLint LightID;

// hotfix
#define RED {1.f, 0.f, 0.f, 1.f}
#define GREEN {0.f, 1.f, 0.f, 1.f}
#define BLUE {0.f, 0.f, 1.f, 1.f}
#define PURPLE {1.f, 0.f, 1.f, 1.f}
#define CYAN {0.f, 1.f, 1.f, 1.f}
#define YELLOW {1.f, 1.f, 0.f, 1.f}
#define WHITE {1.f, 1.f, 1.f, 1.f}
glm::vec4 MeshColor[9] = {RED, GREEN, BLUE, PURPLE, CYAN, YELLOW, RED, YELLOW, WHITE};
#define BASE 0
#define TOP 1
#define ARM1 2
#define JOINT 3
#define ARM2 4
#define PEN 5
#define BOTTOM 6
#define SELECT 8
#define BULLET 7
int Index[6] = {3, 1, 4, 0, 5, 2};
// 0 joint, 1 top, 2 pen, 3 base, 4 arm1, 5 arm2

Ref<Model> BunnyModel;
Ref<Model> RobotArmModel;
//Ref<Model> TestModel;

Ref<Model> HeadModel;


glm::vec3 Rotate[6];

std::vector<Ref<Model>> TModel(8);
glm::vec3 MeshPos[7];

Ref<Entity> base, top, arm1, joint, arm2, pen, bottom;


std::vector<Ref<Entity>> bullets;
//std::vector<Ref<Entity>> g_RobotArm;

Ref<Camera> g_Camera;

glm::vec4 g_ClearColor = {0.0f, 0.0f, 0.2f, 0.0f};
float g_MouseWheelFactor = 0.2;
glm::vec3 g_SunLight = {1.f, 1.f, -1.2f};

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
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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

// Ensure your .obj files are in the correct format and properly loaded by looking at the following function
void loadObject(char* file, glm::vec4 color, Vertex2*& out_Vertices, GLushort*& out_Indices, int ObjectId) {
    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ(file, vertices, uvs, normals);
//
    std::vector<GLushort> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec2> indexed_uvs;
    std::vector<glm::vec3> indexed_normals;
    indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

    const size_t vertCount = indexed_vertices.size();
    const size_t idxCount = indices.size();

    // populate output arrays
    out_Vertices = new Vertex2[vertCount];
    for (int i = 0; i < vertCount; i++) {
        out_Vertices[i].SetPosition(&indexed_vertices[i].x);
        out_Vertices[i].SetNormal(&indexed_normals[i].x);
        out_Vertices[i].SetColor(&color[0]);
        out_Vertices[i].SetUV(&indexed_uvs[i].x);
    }
    out_Indices = new GLushort[idxCount];
    for (int i = 0; i < idxCount; i++) {
        out_Indices[i] = indices[i];
    }

    // set global variables!!
    NumIdcs[ObjectId] = idxCount;
    VertexBufferSize[ObjectId] = sizeof(out_Vertices[0]) * vertCount;
    IndexBufferSize[ObjectId] = sizeof(GLushort) * idxCount;
}

void OnInitScene()
{
    g_Camera = std::make_shared<Camera>(glm::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f));
    g_Camera->SetPosition(10.0f, 10.0f, 10.0f);
    g_Camera->LookAt(0.f, 0.f, 0.f);

//    BunnyModel = Model::LoadModel("./asset/bunny.obj");
//    loadObject("Head.obj", glm::vec4(0.5, 0.5, 0.5, 1.0), Head_Verts, Head_Idcs, 3);

    HeadModel = Model::LoadModel("../asset/head.obj", glm::vec4(0.5, 0.5, 0.5, 1.0));
//    TestModel = Model::LoadModel("../asset/Robot.obj");

//    RobotArmModel = Model::LoadModel("../asset/robot-arm/robot-arm.obj");
//    INFO("size = {}", TestModel->GetMeshes().size());
}

float CameraMoveSpeed = 5.f;
glm::vec3 CameraRotate = {18.320f, -44.f, 0.f};
glm::vec3 CameraPos = {0.f, 0.f, 10.f};
double PrevMouseX, PrevMouseY;
glm::vec3 BunnyPos = glm::vec3{0.f};
glm::vec3 BunnyScale = glm::vec3{1.f};

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

void OnImGuiUpdate()
{
//    ImGui::ShowDemoWindow();

    ImGui::Begin("Settings");

    ImGui::DragFloat("Mouse Wheel", &g_MouseWheelFactor, 0.1f);

    auto dir = g_Camera->GetDir();
    ImGui::Text("Camera Direction = %.2f %.2f %.2f", dir.x, dir.y, dir.z);
    ImGui::Text("Sun Direction = %.2f %.2f %.2f", g_SunLight.x, g_SunLight.y, g_SunLight.z);
    if(ImGui::Button("Set Sun"))
    {
        g_SunLight = dir;
    }

    ImGui::ColorEdit4("Background", glm::value_ptr(g_ClearColor));

    ImGui::SliderFloat("Speed", &CameraMoveSpeed, 1.f, 10.f);
    ImGui::DragFloat3("Pos", &CameraPos);
    ImGui::DragFloat3("Rotation", &CameraRotate);
    ImGui::DragFloat3("Bunny Pos", &BunnyPos);
    ImGui::DragFloat3("Bunny Scale", &BunnyScale);

    ImGui::DragFloat3("Light Dir", &g_SunLight, 0.2f);
    // ==== 測試用 之後拿掉我 ====
    if(ImGui::CollapsingHeader("Test"))
    {
    }
    ImGui::End();
}

void OnRenderScene()
{

    Renderer::BeginScene(g_Camera);
    Renderer::DrawGrid(5, 5);
//    Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});
    Renderer::DrawMesh(HeadModel->GetMeshes().front(), BunnyPos, {0.f, 0.f, 0.f}, BunnyScale);
//    auto sunDir = glm::normalize(g_Camera->GetDir());
//    Renderer::DrawLine({1.f, 1.f, 1.f}, glm::vec3{1.f, 1.f, 1.f} + sunDir * 0.5f, {1.f, 1.f, 0.f, 1.f});
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
void CreateBullet()
{
    Ref<Entity> bullet = Entity::Create("bullet" + std::to_string(b++));
    bullet->transform.pos = {0.f, 0.f, -1.5f};
    bullet->color = MeshColor[BULLET];
    bullet->mesh = TModel[BULLET] -> GetMeshes().back();
    bullet->UpdateSelfAndChild();
    bullet->transform.modelMatrix = pen->transform.modelMatrix * bullet->transform.modelMatrix;
    bullet->transform.GetRTS(bullet->transform.pos, bullet->transform.rotate, bullet->transform.scale);
    bullets.push_back(bullet);
}

bool hold = false;
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    gPickedIndex = -1;

	if(action == GLFW_PRESS) {

        if(!hold) {

            switch (key){

                case GLFW_KEY_B:
                    break;
                case GLFW_KEY_T:
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

//    printf(">>");
//    getchar();

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

        glClearColor(g_ClearColor.x, g_ClearColor.y, g_ClearColor.z, g_ClearColor.w);

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