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
#include "stb_image.h"
#include "Water.h"

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
#include "Texture.h"

#include "OpenGLApplication.h"
#include "OpenGLRenderAPI.h"

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

uint32_t gPickedIndex = -1;

#define STANDARD 0
#define TESSELATION 1
#define GEOMETRY 2
static int g_ShaderMode;

Ref<Camera> g_Camera;

int TessInner = 12;
glm::vec3 TessOuter = glm::vec3(12.f, 12.f, 12.f);

glm::vec4 g_ClearColor = {0.0f, 0.0f, 0.2f, 0.0f};
float g_MouseWheelFactor = 0.2;
glm::vec3 g_SunLight = {5.f, 5.f, 0.f};

float CameraMoveSpeed = 5.f;
glm::vec3 CameraRotate = {18.320f, -44.f, 0.f};
glm::vec3 CameraPos = {0.f, 0.f, 10.f};
double PrevMouseX, PrevMouseY;
float pointSize = 3.f;

class MyApp : public OpenGLApplication
{
public:
    MyApp(int argc, char **argv)
        : OpenGLApplication(argc, argv)
    {
        SetWindowTitle("Water");
        SetWindowSize(window_width, window_height);
    }

    ~MyApp() override
    {
    }

    bool OnInit() override
    {
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
        // Cull triangles which normal is not towards the camera
//	glEnable(GL_CULL_FACE);

        g_Camera = std::make_shared<Camera>(glm::perspective(45.0f, window_width / (float)window_height, 0.1f, 100.0f));
        g_Camera->SetPosition(10.0f, 10.0f, 10.0f);
        g_Camera->LookAt(0.f, 0.f, 0.f); // TODO: impl left drag to move target

        g_ShaderMode = STANDARD;

        water.mesh = Plane::GeneratePlane( 10, 4 );

        water.ambientColor = { 0.115, 0.207, 0.216 };
        water.diffuseColor = { 0.142, 0.309, 0.304 };
        water.specularColor = { 0.044, 0.044, 0.044 };

        Wave w;
        water.waves.push_back( w );

        return true;
    }

    void OnShutdown() override
    {
    }

    void OnProcessInput() override
    {
        // TODO: not implement
    }

    Water water;

    void OnUpdate(float dt) override
    {
        UpdateCamera(dt);

        water.OnUpdate();
    }

    void OnImGuiUpdate() override
    {
        //    ImGui::ShowDemoWindow();
        ImGui::Begin("Settings");

        ImGui::Separator();

        ImGui::SliderFloat("Speed", &CameraMoveSpeed, 1.f, 10.f);
        ImGui::DragFloat3("Pos", &CameraPos);
        ImGui::DragFloat3("Rotation", &CameraRotate);

        ImGui::Text("Options");
        ImGui::DragFloat("Mouse Wheel", &g_MouseWheelFactor, 0.1f);

        ImGui::ColorEdit4("Background", glm::value_ptr(g_ClearColor));

        if(ImGui::RadioButton("Standard shader", &g_ShaderMode, STANDARD))
        {
            g_ShaderMode = STANDARD;
            Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(STANDARD));
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Tessellation shader", &g_ShaderMode, TESSELATION))
        {
            g_ShaderMode = TESSELATION;
            Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(TESSELATION));
        }
        ImGui::SameLine();
        if(ImGui::RadioButton("Geometry shader", &g_ShaderMode, GEOMETRY))
        {
            g_ShaderMode = GEOMETRY;
            Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(GEOMETRY));
        }

        ImGui::DragFloat3("Light Dir", &g_SunLight, 0.2f);

        ImGui::DragFloat("Point Size", &pointSize, 0.1f);
        

        ImGui::Separator();

        water.OnImGuiUpdate();

        ImGui::End();
    }

    void OnRender() override
    {
        Renderer::BeginScene(g_Camera);
        //Renderer::DrawGrid(5, 5);

        Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});

        Renderer::DrawPoint(g_SunLight, {1.f, 1.f, 1.f, 1.f}, 50);


        water.OnRender();

        Renderer::EndScene();
    }

    void OnMouseEvent(int button, int action, int mods) override
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            PickObject();
        }
    }

    void UpdateCamera(float dt)
    {
        { // Camera rotates
            { // Camera Pan
                double x = GetCursorPos().x, y = GetCursorPos().y, offX, offY;
                x = x - window_width / 2.f;
                y = window_height / 2.f - y;
                offX = x - PrevMouseX;
                offY = y - PrevMouseY;

                if (GetMouseButton(MouseButton::Right))
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

            if (GetKey(GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                CameraRotate.y += 5.f * CameraMoveSpeed * dt;
            }
            if (GetKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                CameraRotate.y -= 5.f * CameraMoveSpeed * dt;
            }
            if (GetKey(GLFW_KEY_UP) == GLFW_PRESS)
            {
                CameraRotate.x += 5.f * CameraMoveSpeed * dt;
            }
            if (GetKey(GLFW_KEY_DOWN) == GLFW_PRESS)
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

    bool hold = false;
    bool DrawMeshLine = true;
    bool TmpFlag = false;
    void OnKeyboardEvent(int key, int scancode, int action, int mods) override
    {
        gPickedIndex = -1;

        if(action == GLFW_PRESS)
        {
            if(!hold)
            {
                switch (key)
                {
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

    void OnMouseWheelEvent(double xoffset, double yoffset) override
    {
        CameraPos.z += (float)-yoffset * g_MouseWheelFactor;
        if(CameraPos.z < 0)
            CameraPos.z = 0.f;
    }

    void PickObject()
    {
        Renderer::BeginPickingScene(g_Camera);

        Renderer::EndPickingScene(gPickedIndex);
        INFO("Picking {}", gPickedIndex);
    }
};



int main(int argc, char* argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0);

    MyApp app(argc, argv);

#if 0
    INFO("Attach RenderDoc...");
    getchar();
#endif

    app.Run();
}
