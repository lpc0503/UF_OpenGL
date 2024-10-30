#pragma once

#include "Camera.h"
#include "OpenGLApplication.h"
#include "Water.h"

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

class MyApp : public OpenGLApplication
{
public:
    MyApp( int argc, char** argv );
    virtual ~MyApp() override;

    bool OnInit() override;
    void OnShutdown() override;

    void OnProcessInput() override;
    void OnUpdate( float dt ) override;
    void OnImGuiUpdate() override;
    void OnRender() override;

    void OnMouseEvent( int button, int action, int mods ) override;
    void OnKeyboardEvent( int key, int scancode, int action, int mods ) override;
    void OnMouseWheelEvent( double xoffset, double yoffset ) override;

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

    Ref<Camera> g_Camera;
    void UpdateCamera( float dt );

    uint32_t m_PickedIndex = -1;
    void PickObject();
};

