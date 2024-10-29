#pragma once

#include "Core.h"
//#include "Mesh.h"


#include <vector>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class Mesh;

struct Plane
{
    Plane( int planeLength, int quadRes )
        : planeLength( planeLength )
        , quadRes( quadRes )
    {
    }
    Plane( const Plane& other ) = default;

    int planeLength;
    int quadRes;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<uint32_t> indices;

    Ref<Mesh> ToMesh() const;

    static Ref<Plane> GeneratePlane( int planeLength, int quadRes );
};

struct Wave
{
    enum WaveType
    {
        WT_Sine,
        WT_SteepSine,
        WT_Gerstner
    };
    WaveType type = WT_Sine;

    float frequency;
    float amplitude = 1.0f;  // editable
    float phase;
    glm::vec2 direction;
    float steepness = 1.0f; // editable

    float speed = 1.0f;         // editable
    float wavelength = 1.0f;    // editable
    float direction_deg = 0.f; // editable

    void Init()
    {
        frequency = 2.0f / wavelength;
        phase = speed * 2.0f / wavelength;
        direction = glm::vec2{ cos( glm::radians( direction_deg ) ), sin( glm::radians( direction_deg ) ) };
        direction = glm::normalize( direction );
    }

    Wave();
    Wave( float wavelength, float amplitude, float speed, float direction_deg, float steepness );

    void OnImGuiUpdate();
    void UpdateWater( glm::vec3& vertice, glm::vec3& normal );

    float Sine( glm::vec3 v );
    glm::vec2 SineNormal( glm::vec3 v );

    float SteepSine( glm::vec3 v );
    glm::vec2 SteepSineNormal( glm::vec3 v );

    glm::vec3 Gerstner( glm::vec3 v );
    glm::vec3 GerstnerNormal( glm::vec3 v );
};

struct Water
{
    Ref<Plane> mesh;
    Ref<Plane> displacedMesh;

    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    glm::vec3 pos    = { 0.f, 0.f, 0.f };
    glm::vec3 rotate = { 0.f, 0.f, 0.f };
    glm::vec3 scale  = { 1.f, 1.f, 1.f };

    std::vector<Wave> waves;

    void OnUpdate();
    void OnRender();
    void OnImGuiUpdate();
};
