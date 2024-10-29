// Include standard headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <array>
#include <random>
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

float randomRange(float a, float b)
{
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::uniform_real_distribution<float> dis( a, b );
    return dis( gen );
}

struct WaterRandomSetting
{
    WaterRandomSetting( int& planeLength, int& waveCount )
        : planeLength( planeLength ), waveCount( waveCount )
    {
    }

    Wave::WaveType type = Wave::WT_Sine;

    float medianWavelength = 1.0f;
    float wavelengthRange = 1.0f;

    bool genMedianDirection = true;
    float medianDirection = 0.0f; // genMedianDirection == true
    float directionMin = 0.0f;    // genMedianDirection == false
    float directionMax = 360.f;   // genMedianDirection == false

    float directionalRange = 30.0f;
    float medianAmplitude = 1.0f;
    float medianSpeed = 1.0f;
    float speedRange = 0.1f;
    float steepness = 0.0f;

    int& planeLength;
    int& waveCount;
};

std::vector<Wave> GenerateWaves(const WaterRandomSetting& setting )
{
    const auto& medianWavelength = setting.medianWavelength;
    const auto& wavelengthRange = setting.wavelengthRange;
    const auto& medianDirection = setting.medianDirection;
    const auto& directionalRange = setting.directionalRange;
    const auto& medianAmplitude = setting.medianAmplitude;
    const auto& medianSpeed = setting.medianSpeed;
    const auto& speedRange = setting.speedRange;
    const auto& steepness = setting.steepness;
    const auto& planeLength = setting.planeLength;
    const auto& waveCount = setting.waveCount;
    const auto& genMedianDirection = setting.genMedianDirection;

    const float wavelengthMin = medianWavelength / (1.0f + wavelengthRange);
    const float wavelengthMax = medianWavelength * (1.0f + wavelengthRange);
    const float directionMin = genMedianDirection ? (medianDirection - directionalRange) : setting.directionMin;
    const float directionMax = genMedianDirection ? (medianDirection + directionalRange) : setting.directionMax;
    const float speedMin = max( 0.01f, medianSpeed - speedRange );
    const float speedMax = medianSpeed + speedRange;
    const float ampOverLen = medianAmplitude / medianWavelength;

    const float halfPlaneWidth = planeLength * 0.5f;
    const auto minPoint = glm::vec3( -halfPlaneWidth, 0.0f, -halfPlaneWidth );
    const auto maxPoint = glm::vec3( halfPlaneWidth, 0.0f, halfPlaneWidth );

    std::vector<Wave> ans;
    for ( int wi = 0; wi < waveCount; ++wi )
    {
        float wavelength = randomRange( wavelengthMin, wavelengthMax );
        float direction = randomRange( directionMin, directionMax );
        float amplitude = wavelength * ampOverLen;
        float speed = randomRange( speedMin, speedMax );
        //auto origin = glm::vec2( randomRange( minPoint.x * 2, maxPoint.x * 2 ), randomRange( minPoint.x * 2, maxPoint.x * 2 ) );

        Wave w( wavelength, amplitude, speed, direction, steepness );
        w.type = setting.type;
        ans.emplace_back( w ); // TODO: waveType(circle, line), waveFunction
    }
    return ans;
}

class MyApp : public OpenGLApplication
{
public:
    MyApp(int argc, char **argv)
        : OpenGLApplication(argc, argv)
        , m_RandomWaveSetting( m_PlaneLength, m_WaveCount ) // bind var
    {
        SetWindowTitle("Water");
        SetWindowSize(window_width, window_height);
    }

    ~MyApp() override
    {
    }

    Water m_Water;
    
    enum WaterMode
    {
        WM_Manual,
        WM_Generate
    } m_WaterMode = WM_Manual;

    int m_WaveCount = 4;
    int m_PlaneLength = 10;
    int m_QuadRes = 4;

    WaterRandomSetting m_RandomWaveSetting;

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

        m_Water.mesh = Plane::GeneratePlane( m_PlaneLength, m_QuadRes );

        m_Water.ambientColor = { 0.193, 0.349, 0.321 };
        m_Water.diffuseColor = { 0.142, 0.309, 0.304 };
        m_Water.specularColor = { 0.044, 0.044, 0.044 };

        switch( m_WaterMode )
        {
        case WM_Manual:
        {
            m_Water.waves.resize( m_WaveCount );

            m_Water.waves[0].direction_deg = 88.f;
            m_Water.waves[0].amplitude = 0.1f;

            m_Water.waves[1].direction_deg = 303.f;
            m_Water.waves[1].amplitude = 0.1f;

            m_Water.waves[2].direction_deg = 257.f;
            m_Water.waves[2].amplitude = 0.1f;

            m_Water.waves[3].direction_deg = 38.f;
            m_Water.waves[3].amplitude = 0.1f;

            for ( auto& w : m_Water.waves )
            {
                w.Init();
            }
            break;
        }

        case WM_Generate:
        {
            m_Water.waves = GenerateWaves( m_RandomWaveSetting );
            break;
        }

        default:
            assert( 0 );
            break;
        }

        return true;
    }

    void OnShutdown() override
    {
    }

    void OnProcessInput() override
    {
        // TODO: not implement
    }

    void OnUpdate(float dt) override
    {
        UpdateCamera(dt);

        m_Water.OnUpdate();
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

        ImGui::DragFloat3("Light Dir", &g_SunLight, 0.2f);

        ImGui::DragFloat("Point Size", &pointSize, 0.1f);

        ImGui::Separator();

        ImGui::Text( "Water Settings" );

        if ( ImGui::RadioButton( "Manual", (int*)&m_WaterMode, WM_Manual ) )
        {
            m_Water.waves.clear();
        }
        ImGui::SameLine();
        if ( ImGui::RadioButton( "Generate", (int*)&m_WaterMode, WM_Generate ) )
        {
            m_Water.waves.clear();

            m_RandomWaveSetting.medianAmplitude = 0.1f;
        }

        if( m_WaterMode == WM_Manual )
        {
            static Wave::WaveType s_manualWaveType = Wave::WT_Sine;
            auto SetWaveType = [&]( Wave::WaveType type )
            {
                for ( auto& w : m_Water.waves )
                {
                    w.type = type;
                }
            };
            if ( ImGui::RadioButton( "Sine", (int *)&s_manualWaveType, Wave::WT_Sine) )
            {
                SetWaveType( s_manualWaveType );
            }
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Steep Sine", (int*)&s_manualWaveType, Wave::WT_SteepSine ) )
            {
                SetWaveType( s_manualWaveType );
            }
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Gerstner", (int*)&s_manualWaveType, Wave::WT_Gerstner ) )
            {
                SetWaveType( s_manualWaveType );
            }
            if ( ImGui::SliderInt( "Wave Count", &m_WaveCount, 1, 10 ) )
            {
                m_Water.waves.resize( m_WaveCount );
                SetWaveType( s_manualWaveType );
            }
        }
        else if ( m_WaterMode == WM_Generate )
        {
            if ( ImGui::RadioButton( "Sine", (int*)&m_RandomWaveSetting.type, Wave::WT_Sine ) )
            {

            }
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Steep Sine", (int*)&m_RandomWaveSetting.type, Wave::WT_SteepSine ) )
            {
                
            }
            ImGui::SameLine();
            if ( ImGui::RadioButton( "Gerstner", (int*)&m_RandomWaveSetting.type, Wave::WT_Gerstner ) )
            {

            }
            ImGui::SliderInt( "Wave Count", &m_WaveCount, 1, 10 );
            ImGui::DragFloat( "Median Wavelength", &m_RandomWaveSetting.medianWavelength, 0.01f, 0.0f, 3.0f );
            ImGui::DragFloat( "Wavelength Range", &m_RandomWaveSetting.wavelengthRange, 0.01f, 0.0f, 2.0f );
            ImGui::Checkbox( "Use Median Direction", &m_RandomWaveSetting.genMedianDirection );
            if ( m_RandomWaveSetting.genMedianDirection )
            {
                ImGui::DragFloat( "Median Direction", &m_RandomWaveSetting.medianDirection, 0.01f, 0.0f, 360.0f );
            }
            else
            {
                float val[2] = { m_RandomWaveSetting.directionMin, m_RandomWaveSetting.directionMax };
                if ( ImGui::DragFloat2( "Min/Max Direction", val, 0.01f, 0.0f, 360.0f ) )
                {
                    if ( val[0] < val[1] )
                    {
                        m_RandomWaveSetting.directionMin = val[0];
                        m_RandomWaveSetting.directionMax = val[1];
                    }
                    else
                    {
                        m_RandomWaveSetting.directionMin = m_RandomWaveSetting.directionMax = val[0] = val[1];
                    }
                }
            }
            ImGui::DragFloat( "Directional Range", &m_RandomWaveSetting.directionalRange, 0.01f, 0.0f, 360.0f );
            ImGui::DragFloat( "Median Amplitude", &m_RandomWaveSetting.medianAmplitude, 0.01f, 0.0f, 3.0f );
            ImGui::DragFloat( "Median Speed", &m_RandomWaveSetting.medianSpeed, 0.01f, 0.0f, 2.0f );
            ImGui::DragFloat( "Speed Range", &m_RandomWaveSetting.speedRange, 0.01f, 0.0f, 1.0f );
            ImGui::DragFloat( "Steepness", &m_RandomWaveSetting.steepness, 0.01f, 10.f ); // TODO: Gerstner limit

            if ( ImGui::Button( "Generate###GenBtn" ) )
            {
                m_Water.waves.clear();
                m_Water.waves = GenerateWaves( m_RandomWaveSetting );
            }
        }
        else
        {
            assert( 0 );
        }

        m_Water.OnImGuiUpdate();

        ImGui::End();
    }

    void OnRender() override
    {
        Renderer::BeginScene(g_Camera);
        //Renderer::DrawGrid(5, 5);

        Renderer::DrawDirectionalLight(g_SunLight, {1.f, 1.f, 1.f, 1.f});

        Renderer::DrawPoint(g_SunLight, {1.f, 1.f, 1.f, 1.f}, 50);


        m_Water.OnRender();

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
