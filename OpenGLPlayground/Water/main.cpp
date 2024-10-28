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

struct Plane
{
    Plane( int planeLength, int quadRes )
        : planeLength( planeLength )
        , quadRes( quadRes )
    {
    }

    int planeLength;
    int quadRes;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<uint32_t> indices;

    Ref<Mesh> ToMesh() const
    {
        int sideVertCount = planeLength * quadRes;
        int N = sideVertCount + 1;

        // convert to mesh
        auto plane = MakeRef<Mesh>();
        plane->m_Indices = indices;
        for ( int x = 0; x <= sideVertCount; ++x )
        {
            for ( int z = 0; z <= sideVertCount; ++z )
            {
                plane->m_Vertices.push_back(
                    Vertex( vertices[x * N + z],
                            glm::vec4{ 1.f, 1.f, 1.f, 1.f },
                            normals[x * N + z],
                            uvs[x * N + z] )
                );
            }
        }

        return plane;
    }
};
Ref<Plane> GeneratePlane( int planeLength, int quadRes )
{
    float halfLength = planeLength * 0.5f;
    int sideVertCount = planeLength * quadRes;

    auto plane = MakeRef<Plane>( planeLength, quadRes );
    //
    auto& vertices = plane->vertices;
    auto& uvs = plane->uvs;
    auto& normals = plane->normals;
    auto& tangents = plane->tangents;
    auto& indices = plane->indices;

    // Generate vertices, UVs, and tangents
    for ( int x = 0; x <= sideVertCount; ++x )
    {
        for ( int z = 0; z <= sideVertCount; ++z )
        {
            float nx = ((float)x / sideVertCount * planeLength) - halfLength;
            float ny = 0.0f;
            float nz = ((float)z / sideVertCount * planeLength) - halfLength;

            // Vertex position
            vertices.push_back( glm::vec3( nx, ny, nz ) );

            // UV coordinates
            uvs.push_back( glm::vec2( static_cast<float>(x) / sideVertCount, static_cast<float>(z) / sideVertCount ) );

            // Tangent (same for all vertices)
            tangents.push_back( glm::vec4( 1.0f, 0.0f, 0.0f, -1.0f ) );

            // Initialize normal (will be recalculated)
            normals.push_back( glm::vec3( 0.0f, 1.0f, 0.0f ) );
        }
    }

    // Generate indices for the triangles
    int N = sideVertCount + 1;
    for ( int x = 0; x < sideVertCount; ++x )
    {
        for ( int z = 0; z < sideVertCount; ++z )
        {
            int topLeft = x * N + z;
            int topRight = topLeft + 1;

            int bottomLeft = (x + 1) * N + z;
            int bottomRight = bottomLeft + 1;

            // Triangle 1
            indices.push_back( topLeft );
            indices.push_back( bottomLeft );
            indices.push_back( topRight );

            // Triangle 2
            indices.push_back( topRight );
            indices.push_back( bottomLeft );
            indices.push_back( bottomRight );
        }
    }

    return plane;
}

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

        planeMesh = GeneratePlane( 10, 1 );

        return true;
    }

    void OnShutdown() override
    {
    }

    void OnProcessInput() override
    {
        // TODO: not implement
    }

    struct Wave
    {
        float frequency;
        float amplitude;
        float phase;
        glm::vec2 direction;

        Wave( float wavelength, float amplitude, float speed, float direction_deg )
            : frequency( 2.0f / wavelength )
            , amplitude( amplitude )
            , phase( speed * 2.0f / wavelength )
            , direction( cos( glm::radians( direction_deg ) ), sin( glm::radians( direction_deg ) ) )
        {
        }
    };

    float speed = 1.0f;
    float amplitude = 1.0f;
    float wavelength = 1.0f;
    float direction = 0.f;

    Ref<Plane> planeMesh;

    void OnUpdate(float dt) override
    {
        UpdateCamera(dt);

        Wave w( wavelength, amplitude, speed, direction );

        auto& vertices = planeMesh->vertices;
        auto& normals = planeMesh->normals;
        for ( int i = 0; i < vertices.size(); ++i )
        {
            auto v = vertices[i];
            auto n = normals[i];
            float t = GetTime();

            // Update xyz
            v.x *= w.direction.x;
            v.z *= w.direction.y;
            float h = sin( w.frequency * (v.x + v.z) + t * w.phase ) * w.amplitude;
            vertices[i].y = h;

            // Update Normal
            float dx = w.frequency * w.amplitude * w.direction.x * cos( (v.x + v.z) * w.frequency + t * w.phase );
            float dy = w.frequency * w.amplitude * w.direction.y * cos( (v.x + v.z) * w.frequency + t * w.phase );
            n = glm::vec3( -dx, 1, -dy );
            n = glm::normalize( n );
            normals[i] = n;
        }
       
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

        // ShaderMode
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

        ImGui::DragInt("TessInner", &TessInner, 1);
        ImGui::DragFloat3("TessOuter", &TessOuter, 1);

        ImGui::DragFloat("Point Size", &pointSize, 0.1f);

        ImGui::Separator();

        ImGui::Text("Wave"); ImGui::SameLine();

        ImGui::DragFloat( "Speed", &speed, 0.1f );
        ImGui::DragFloat( "Amplitude", &amplitude, 0.1f );
        ImGui::DragFloat( "WaveLength", &wavelength, 0.1f );
        ImGui::DragFloat( "Direction", &direction, 1.0f, 0.f, 360.f );

        ImGui::Text("Wave Method");
        

        ImGui::End();
    }

    void OnRender() override
    {
        Renderer::BeginScene(g_Camera);
        Renderer::DrawGrid(5, 5);

        Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});

        Renderer::DrawPoint(g_SunLight, {1.f, 1.f, 1.f, 1.f}, 50);

        Renderer::DrawMesh( planeMesh->ToMesh(), {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});

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
