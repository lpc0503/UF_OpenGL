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
#include "PNTriangle.h"
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

Ref<Model> g_Model;
Ref<Model> sail;
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
glm::vec3 g_ModelPos = glm::vec3{0.f, 0.f, 0.f};
glm::vec3 g_ModelScale = glm::vec3{0.08f, 0.05f, 0.05f};
float pointSize = 3.f;
glm::vec3 SailPos = glm::vec3{-0.2f, 2.0f, 0.f};
glm::vec3 SailScale = glm::vec3{1.f};

glm::vec3 tOffset = glm::vec3{2.4f, 0.f, 1.5f};

struct BezierSurfaceMethod {
    enum {
        DeCasteljau = 0,
        Formula     = 1
    };
};
int bezierSurfaceMethod = BezierSurfaceMethod::DeCasteljau;
std::vector<glm::vec3> points;

struct WaveMethod {
    enum {
        Sine = 0,
        SineWithDir = 1,
        MultipleSineWithDir = 2,
        Count
    };
};
int g_WaveMethod = WaveMethod::MultipleSineWithDir;

bool g_StartSimulateWave = true;

struct WaveProperity
{
    WaveProperity()
    {
    }
    ~WaveProperity()
    {
    }
    WaveProperity(float degree, float A, float omega, float phase)
        : waveDirDegree(degree),
          A(A),
          omega(omega),
          phase(phase)
    {
    }

    bool enable = true;
    glm::vec2 waveDir = {1.f, 0.f};
    float waveDirDegree = 0.f;

    // y = A * sin(omega * t + phase)
    float A = 2.f;
    float omega = 180.f; // degree
    float phase = 90.f;  // degree
};
std::vector<WaveProperity> waves = {
    {0.f, 0.25f, 180.f, 90.f},
    {135.f, 0.75f, 225.f, 90.f},
};

bool g_DrawPNTriangle = true;
// ===============================================================

int factorial(int n)
{
    int r = 1;
    for(int i = n; i>0; i--)
    {
        r *= i;
    }
    return r;
}

float bernstein(int i, int n, float t)
{
    float r = (float) factorial(n) / (float) (factorial(i) * factorial(n - i));
    r *= pow(t, i);
    r *= pow(1-t, n-i);
    return r;
}

glm::vec3 deCasteljau(std::vector<glm::vec3> points, int degree, float t)
{
    std::vector<float> p((degree + 1) * 3);

    const int w = 3;
    for(int j = 0; j <= degree; j++)
    {
        p[j*w + 0] = points[j].x;
        p[j*w + 1] = points[j].y;
        p[j*w + 2] = points[j].z;
    }
    for(int k = 1; k <= degree; k++)
    {
        for(int j = 0; j <= degree - k; j++)
        {
            int jw = j * w;
            int jp1w = (j + 1) * w;
            p[jw + 0] = (1.f - t) * p[jw + 0] + t * p[jp1w + 0];
            p[jw + 1] = (1.f - t) * p[jw + 1] + t * p[jp1w + 1];
            p[jw + 2] = (1.f - t) * p[jw + 2] + t * p[jp1w + 2];
        }
    }

    glm::vec3 result(p[0], p[1], p[2]);
    return result;
}

int precision = 10;
std::vector<std::vector<glm::vec3>> calSurface(int uPoints, int vPoints, std::vector<std::vector<glm::vec3>> controlPoints)
{
    using ::precision;
    std::vector<std::vector<glm::vec3>> curvePoints(precision+1, std::vector<glm::vec3>(precision+1));

    if(bezierSurfaceMethod == BezierSurfaceMethod::DeCasteljau)
    {
        // de Casteljau's Algorithm
        for(int ui = 0; ui <= precision; ui++)
        {
            float u = float(ui)/float(precision);
            for(int vi = 0; vi <= precision; vi++)
            {
                float v = float(vi)/float(precision);
                std::vector<glm::vec3> qPoints(uPoints+1);
                for(int i=0; i <= uPoints; i++)
                {
                    qPoints[i] = deCasteljau(controlPoints[i], vPoints, v);
                }
                curvePoints[ui][vi] = deCasteljau(qPoints, uPoints, u);
            }
        }
    }
    else if(bezierSurfaceMethod == BezierSurfaceMethod::Formula)
    {
        for(int ui = 0; ui <= precision; ui++)
        {
            float u = float(ui)/float(precision);
            for(int vi = 0; vi <= precision; vi++)
            {
                float v = float(vi)/float(precision);

                glm::vec3 point = glm::vec3{0.f};
                for(int m = 0; m <= uPoints; m++)
                {
                    for(int n = 0; n <= vPoints; n++)
                    {
                        float bm = bernstein(m, uPoints, u);
                        float bn = bernstein(n, vPoints, v);
                        float b = bm * bn;
                        point.x += b * controlPoints[m][n].x;
                        point.y += b * controlPoints[m][n].y;
                        point.z += b * controlPoints[m][n].z;
                    }
                }

                curvePoints[ui][vi] = point;
            }
        }
    }

    return curvePoints;
}

class MyApp : public OpenGLApplication
{
public:
    MyApp(int argc, char **argv)
        : OpenGLApplication(argc, argv)
    {
        SetWindowTitle("Po_Chuan,Liang(7336-5707)");
        SetWindowSize(window_width, window_height);
    }

    // TODO: Remove
    MyApp()
        : OpenGLApplication(0, nullptr)
    {
        SetWindowTitle("Po_Chuan,Liang(7336-5707)");
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

        g_ShaderMode = TESSELATION;
        points = {
                {-8.0, 1.0, -8.0},
                {-4.0, 1.0, -8.0},
                {0.0, 1.0, -8.0},
                {4.0, 1.0, -8.0},
                {8.0, 1.0, -8.0},
                {-8.0, -1.0, -4.0},
                {-4.0, -1.0, -4.0},
                {0.0, -1.0, -4.0},
                {4.0, -1.0, -4.0},
                {8.0, -1.0, -4.0},
                {-8.0, 4.0, 0.0},
                {-4.0, 4.0, 0.0},
                {0.0, 4.0, 0.0},
                {4.0, 4.0, 0.0},
                {8.0, 4.0, 0.0},
                {-8.0, -1.0, 4.0},
                {-4.0, -1.0, 4.0},
                {0.0, -1.0, 4.0},
                {4.0, -1.0, 4.0},
                {8.0, -1.0, 4.0},
                {-8.0, 1.0, 8.0},
                {-4.0, 1.0, 8.0},
                {0.0, 1.0, 8.0},
                {4.0, 1.0, 8.0},
                {8.0, 1.0, 8.0},
        };

        // u*v = 4*4

        g_Model = Model::LoadModel("assets/model/boat.obj", glm::vec3(0, 0.95, 1.f));
        sail = Model::LoadQuadModel("assets/model/quad.obj", glm::vec4(1.f, 0.f, 1.f, 1.f));
        return true;
    }

    void OnShutdown() override
    {
    }

    void OnProcessInput() override
    {
        // TODO: not implement
    }

    float u1[5] = {0.f, 0.f, 0.f, 0.f, 0.f};
    void OnUpdate(float dt) override
    {
        UpdateCamera(dt);

        //    g_SunLight = g_Camera->GetDir();

        Renderer::SetTessInnerLevel(TessInner);
        Renderer::SetTessOuterLevel(TessOuter);

//    if(g_ShaderMode == STANDARD) {
//
//        Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(STANDARD));
//    }
//    else if(g_ShaderMode == TESSELATION) {
//
//        Renderer::SetShaderMode(static_cast<Renderer::ShaderMode>(TESSELATION));
//    }


        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static float sinceStart = 0.f;
        static const float DEG2RAD = 3.14159265f / 180.f;

        if(g_WaveMethod == WaveMethod::Sine)
        {
            const auto& omega = waves[0].omega;
            const auto& phase = waves[0].phase;
            const auto& A = waves[0].A;

            for (int i = 0; i < 5; i++)
            {
                u1[i] = A * sin(omega * DEG2RAD * sinceStart + phase * i * DEG2RAD);
            }

            for(int i = 0 ; i < 5 ; i++)
            {
                for(int j = 0 ; j < 5 ; j++)
                {
                    int index = 5*i+j;
                    points[index].y = u1[i];
                }
            }
        }
        else if(g_WaveMethod == WaveMethod::SineWithDir)
        {
            const auto& waveDir = waves[0].waveDir;
            const auto& omega = waves[0].omega;
            const auto& phase = waves[0].phase;
            const auto& A = waves[0].A;

            for(int i = 0; i < 5; i++)
            {
                for(int j = 0; j < 5; j++)
                {
                    int index = 5*i + j;
                    points[index].y =  A * sin( glm::dot(waveDir, glm::vec2{points[index].x, points[index].z}) * (omega * DEG2RAD)  + (phase * DEG2RAD)* sinceStart );
                }
            }
        }
        else if(g_WaveMethod == WaveMethod::MultipleSineWithDir)
        {
            std::array<std::array<float, 5>, 5> w{};
            for(const auto& wave : waves)
            {
                const auto& waveDir = wave.waveDir;
                const auto& omega = wave.omega;
                const auto& phase = wave.phase;
                const auto& A = wave.A;
                const auto enable = wave.enable;

                if(!enable)
                    continue;

                for(int i = 0; i < 5; i++)
                {
                    for(int j = 0; j < 5; j++)
                    {
                        int index = 5*i + j;
                        w[i][j] += A * sin( glm::dot(waveDir, glm::vec2{points[index].x, points[index].z}) * (omega * DEG2RAD)  + (phase * DEG2RAD)* sinceStart );
                    }
                }
            }
            for(int i = 0; i < 5; i++)
                for(int j = 0; j < 5; j++)
                    points[5*i + j].y = w[i][j];
        }

        if(g_StartSimulateWave)
        {
            sinceStart += dt;
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

        ImGui::DragFloat3("SailPos", &SailPos, 0.05);
        ImGui::DragFloat3("SailScale", &SailScale, 0.05, 0.f);

        ImGui::DragFloat3("Pos", &g_ModelPos, 0.05);
        ImGui::DragFloat3("Scale", &g_ModelScale, 0.05);

        ImGui::DragFloat3("Light Dir", &g_SunLight, 0.2f);

        ImGui::DragInt("TessInner", &TessInner, 1);
        ImGui::DragFloat3("TessOuter", &TessOuter, 1);

        ImGui::DragFloat("Point Size", &pointSize, 0.1f);
//    ImGui::DragInt("Triangle Range", &triangleRange, 1, 0, g_Model->GetMeshes().front()->m_Vertices.size());
//    ImGui::DragInt("Point Range", &pointRange,1, 0, 2);

        ImGui::Separator();

        ImGui::Text("PN");
        ImGui::Checkbox("PN Triangle", &g_DrawPNTriangle);

        ImGui::Text("Curve options");
        ImGui::DragFloat3("Move to origin", &tOffset);
        ImGui::RadioButton("de Casteljau's", &bezierSurfaceMethod, BezierSurfaceMethod::DeCasteljau);
        ImGui::RadioButton("formula of Bezier curve", &bezierSurfaceMethod, BezierSurfaceMethod::Formula);
        ImGui::DragInt("Precision", &::precision, 1.f, 1, 100);
        ImGui::DragFloat("u1", &u1[0], 0.01f, -50, 50);
        ImGui::DragFloat("u2", &u1[1], 0.01f, -50, 50);
        ImGui::DragFloat("u3", &u1[2], 0.01f, -50, 50);
        ImGui::DragFloat("u4", &u1[3], 0.01f, -50, 50);
        ImGui::DragFloat("u5", &u1[4], 0.01f, -50, 50);
        ImGui::Separator();

        ImGui::Text("Wave"); ImGui::SameLine();
        ImGui::Checkbox("Start simulate", &g_StartSimulateWave);

        ImGui::Text("Wave Method");
        ImGui::RadioButton("Simple Sine", &g_WaveMethod, WaveMethod::Sine);
        ImGui::RadioButton("Sine With Direction", &g_WaveMethod, WaveMethod::SineWithDir);
        ImGui::RadioButton("Multiple Sine With Direction", &g_WaveMethod, WaveMethod::MultipleSineWithDir);
        if(g_WaveMethod == WaveMethod::Sine)
        {
            auto& omega = waves[0].omega;
            auto& phase = waves[0].phase;
            auto& A = waves[0].A;

            ImGui::DragFloat("A", &A);
            ImGui::DragFloat("omega", &omega);
            ImGui::DragFloat("phase", &phase);
        }
        else if(g_WaveMethod == WaveMethod::SineWithDir)
        {
            auto& waveDir = waves[0].waveDir;
            auto& waveDirDegree = waves[0].waveDirDegree;
            auto& omega = waves[0].omega;
            auto& phase = waves[0].phase;
            auto& A = waves[0].A;

            ImGui::DragFloat("A", &A);
            ImGui::DragFloat("omega", &omega);
            ImGui::DragFloat("phase", &phase);
            ImGui::DragFloat("dir (degree)", &waveDirDegree, 0.1f, 0.f, 360.f);
            waveDir.y = sin(glm::radians(waveDirDegree));
            waveDir.x = cos(glm::radians(waveDirDegree));
            ImGui::Text("dir = %.2f %.2f", waveDir.x, waveDir.y);
        }
        else if(g_WaveMethod == WaveMethod::MultipleSineWithDir)
        {
            if(ImGui::Button("Add wave"))
            {
                waves.push_back({});
            }

            static char buf[10];
            if(ImGui::InputText("View Only", buf, 10, ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_AutoSelectAll))
            {
                int viewIdx = std::strtol(buf, nullptr, 10);
                if(viewIdx >= 0 && viewIdx < waves.size())
                {
                    for(int i = 0; i < waves.size(); i++)
                    {
                        if(i == viewIdx)
                            waves[i].enable = true;
                        else
                            waves[i].enable = false;
                    }
                }
                else
                {
                    buf[0] = '\0';
                }
            }

            int i = 0;
            for(auto& wave : waves)
            {
                auto& enable = wave.enable;
                auto& waveDir = wave.waveDir;
                auto& waveDirDegree = wave.waveDirDegree;
                auto& omega = wave.omega;
                auto& phase = wave.phase;
                auto& A = wave.A;

                ImGui::PushID(i);
                ImGui::Checkbox("Enable", &enable); ImGui::SameLine();
                if(ImGui::Button("Delete"))
                {
                    waves.erase(waves.begin()+i);
                    ImGui::PopID();
                    break;
                }
                ImGui::Text("Wave %d", i++);
                ImGui::DragFloat("A", &A);
                ImGui::DragFloat("omega", &omega);
                ImGui::DragFloat("phase", &phase);
                ImGui::DragFloat("dir (degree)", &waveDirDegree, 0.1f, 0.f, 360.f);
                waveDir.y = sin(glm::radians(waveDirDegree));
                waveDir.x = cos(glm::radians(waveDirDegree));
                ImGui::Text("dir = %.2f %.2f", waveDir.x, waveDir.y);
                ImGui::PopID();
            }
        }

        ImGui::End();
    }

    void OnRender() override
    {
        Renderer::BeginScene(g_Camera);
        Renderer::DrawGrid(5, 5);

        glm::vec4 color = {1.0f, 1.f, 1.f, 1.f};

        Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});

        glm::vec3 start = {-1.f, 1.f, -1.f};
        glm::vec3 end = {1.f, 1.f, 1.f};

        for(const auto& wave : waves)
        {
            const auto& waveDir = wave.waveDir;
            Renderer::DrawLine({0.f, 5.f, 0.f}, {waveDir.x, 5.f, waveDir.y}, {1.f, 0.f, 0.f, 1.f});
        }

        Renderer::DrawPoint(g_SunLight, {0.f, 0.f, 0.f, 1.f}, 50);
        Renderer::DrawPoint(sail->GetMeshes().back()->m_Vertices[sail->GetMeshes().back()->m_Indices[20]].pos, {1.f, 1.f, 0.f, 1.f}, pointSize);
        Renderer::DrawPoint(sail->GetMeshes().back()->m_Vertices[sail->GetMeshes().back()->m_Indices[21]].pos, {1.f, 1.f, 0.f, 1.f}, pointSize);
        Renderer::DrawPoint(sail->GetMeshes().back()->m_Vertices[sail->GetMeshes().back()->m_Indices[22]].pos, {1.f, 1.f, 0.f, 1.f}, pointSize);
        Renderer::DrawPoint(sail->GetMeshes().back()->m_Vertices[sail->GetMeshes().back()->m_Indices[23]].pos, {1.f, 1.f, 0.f, 1.f}, pointSize);
//    Renderer::DrawLine({0.f, 5.f, 0.f}, {waveDir.x, 5.f, waveDir.y}, {1.f, 0.f, 0.f, 1.f});

        int u = 4, v = 4;
        std::vector<std::vector<glm::vec3>> controlPoints(u+1, std::vector<glm::vec3>(v+1));
        for(int m = 0; m <= u; m++)
        {
            for(int n = 0; n <= v; n++)
            {
                auto& p = points[m*(v+1)+n];

//            p.x -= tOffset.x;
//            p.z -= tOffset.z;

                controlPoints[m][n] = p;
                Renderer::DrawPoint(controlPoints[m][n], {1.f, 0.f, 0.f, 1.f}, pointSize);
//            printf("[%d][%d] = [%d]\n", m, n, m*(v+1)+n);
            }
        }

        auto curve = calSurface(u, v, controlPoints);
        using ::precision;
        for(int i = 0; i <= precision; i++)
        {
            for(int j = 0; j <= precision; j++)
            {
                Renderer::DrawPoint(curve[i][j], {0.f, 1.f, 0.f, 1.f}, pointSize);
            }
        }

        { // Draw surface
            const auto white = glm::vec4{1.f, 1.f, 1.f, 1.f};
            const auto black = glm::vec4{0.f, 0.f, 0.f, 1.f};
            bool useWhite = true;
            for(int ui = 0; ui <= precision - 1; ui++)
            {
                std::vector<glm::vec3> strip;
                std::vector<glm::vec2> uv;
                glm::vec2 preuv = glm::vec2(0.f, 0.f);
                for(int vi = 0; vi <= precision; vi++)
                {
                    glm::vec3 p1 = curve[ui][vi];
                    glm::vec3 p2 = curve[ui+1][vi];

                    glm::vec2 uv1 = glm::vec2((float)ui/(float)precision, (float)vi/(float)precision);
                    glm::vec2 uv2 = glm::vec2((float)(ui+1)/(float)precision, (float)vi/(float)precision);

                    strip.push_back(p1);
                    strip.push_back(p2);
                    uv.push_back(uv1);
                    uv.push_back(uv2);
                }
                // convert GL_TRIANGLE_STRIP to GL_TRIANGLES: https://blog.csdn.net/sinat_29255093/article/details/103276431
                std::vector<glm::vec3> triangleVertices;
//            INFO("{}", strip.size());
                for(int i = 2; i < strip.size(); i++)
                {
                    std::vector<glm::vec2> uv_(3);
                    uv_[0] = uv[i-1];
                    uv_[1] = uv[i-2];
                    uv_[2] = uv[i];

                    if(i % 2 != 0)
                    {
                        Renderer::DrawTriangle(strip[i-1], strip[i-2], strip[i], uv_,useWhite ? white : black);
                    }
                    else
                    {
                        Renderer::DrawTriangle(strip[i-2], strip[i-1], strip[i], uv_, useWhite ? white : black);
                    }

//                if(i != 2 && i % 2 == 0)
//                {
//                    useWhite = !useWhite;
//                }
                }
            }
        }

        if(g_DrawPNTriangle)
        {
            Renderer::DrawMesh(g_Model->GetMeshes().front(), g_ModelPos, {0.f, 0.f, 0.f}, g_ModelScale);
        }

        Renderer::DrawMesh(sail->GetMeshes().back(), SailPos, {0.f, 0.f, 0.f}, SailScale, {1.f, 1.f, 1.f, 1.f}, true);

//    Renderer::DrawMesh(g_Model->GetMeshes().front(), BunnyPos, {0.f, 0.f, 0.f}, BunnyScale);
//    auto sunDir = glm::normalize(g_Camera->GetDir());
//    Renderer::DrawLine(g_SunLight, g_SunLight + sunDir * 0.5f, {1.f, 1.f, 0.f, 1.f});

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

MyApp app;

int main(int argc, char* argv[]) {
    // disable buffer
    setvbuf(stdout, NULL, _IONBF, 0);

#if 0
    INFO("Attach RenderDoc...");
    getchar();
#endif

    app.Run();
}
