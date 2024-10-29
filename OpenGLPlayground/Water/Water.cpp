#include "Water.h"

#include "Core.h"
#include "Mesh.h"
#include "Renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

float GetTime()
{
    return (float)glfwGetTime();
}

bool s_DebugWaterNormal = false;

void Water::OnUpdate()
{
    auto vertices = mesh->vertices;
    auto normals = mesh->normals;

    displacedMesh = MakeRef<Plane>( *mesh ); // copy
    auto& displacedVertices = displacedMesh->vertices;
    auto& displacedNormals = displacedMesh->normals;

    for ( int i = 0; i < vertices.size(); ++i )
    {
        auto v = vertices[ i ]; // copy

        // 更新頂點
        glm::vec3 newPos{ 0.f };
        for ( auto& w : waves )
        {
            switch ( w.type )
            {
            case Wave::WT_Sine:
                newPos.y += w.Sine( v );
                break;

            case Wave::WT_SteepSine:
                newPos.y += w.SteepSine( v );
                break;

            case Wave::WT_Gerstner:
            {
                glm::vec3 g = w.Gerstner( v );

                newPos.x += g.x;
                newPos.z += g.z;
                newPos.y += g.y;
                break;
            }

            default:
                assert( 0 );
                break;
            }
        }

        displacedVertices[i] = glm::vec3( v.x + newPos.x, newPos.y, v.z + newPos.z );

        // 更新 normal
        glm::vec3 newNormal{ 0.f };
        for ( auto& w : waves )
        {
            switch ( w.type )
            {
            case Wave::WT_Sine:
            {
                auto normal_xz = w.SineNormal( v );
                newNormal.x += normal_xz.x; // x
                newNormal.y += normal_xz.y; // z
                break;
            }

            case Wave::WT_SteepSine:
            {
                auto normal_xz = w.SteepSineNormal( v );
                newNormal.x += normal_xz.x; // x
                newNormal.y += normal_xz.y; // z
                break;
            }

            case Wave::WT_Gerstner:
            {
                auto normal = w.GerstnerNormal( displacedVertices[i] );
                newNormal.x += normal.x;
                newNormal.y += normal.y;
                newNormal.z += normal.z;
                break;
            }

            default:
                assert( 0 );
                break;
            }
        }

        bool hasGerstner = std::any_of( waves.begin(), waves.end(), []( const Wave& w ) { return w.type == Wave::WT_Gerstner; } );
        if ( hasGerstner )
        {
            displacedNormals[i] = glm::vec3( -newNormal.x, 1.0f - newNormal.y, -newNormal.z );
        }
        else
        {
            displacedNormals[i] = glm::vec3( -newNormal.x, 1.0f, -newNormal.y );
        }

        displacedNormals[i] = glm::normalize( displacedNormals[i] );
    }
    
}

void Water::OnRender()
{
    Renderer::DrawWater( this );

    if ( s_DebugWaterNormal )
    {
        for( int i = 0; i < displacedMesh->vertices.size(); i++ )
        {
            auto color = glm::vec4{ 1.f, 0.f, 0.f , 1.f };
            Renderer::DrawPoint( displacedMesh->vertices[i], color, 5.f );
            Renderer::DrawLine( displacedMesh->vertices[i], displacedMesh->vertices[i] + displacedMesh->normals[i], color );
        }
    }
}

void Water::OnImGuiUpdate()
{
    ImGui::Text( "Water Material" );
    ImGui::ColorEdit3( "Ambient", glm::value_ptr( ambientColor ), ImGuiColorEditFlags_Float );
    ImGui::ColorEdit3( "Diffuse", glm::value_ptr( diffuseColor ), ImGuiColorEditFlags_Float );
    ImGui::ColorEdit3( "Specular", glm::value_ptr( specularColor ), ImGuiColorEditFlags_Float );
    ImGui::Checkbox( "Debug Normal", &s_DebugWaterNormal );

    for ( int i = 0; i < waves.size(); i++ )
    {
        ImGui::PushID( i );
        {
            ImGui::Text( "Wave %d", i );
            waves[i].OnImGuiUpdate();
        }
        ImGui::PopID();
    }

    ImGui::Text( "Wave Method" );
}

Wave::Wave()
{
    Init();
}

Wave::Wave( float wavelength, float amplitude, float speed, float direction_deg, float steepness )
    : amplitude( amplitude )
    , steepness( steepness )
    , speed( speed )
    , wavelength( wavelength )
    , direction_deg( direction_deg )
{
    Init();
}

void Wave::OnImGuiUpdate()
{
    if ( ImGui::DragFloat( "Speed", &speed, 0.1f ) )
    {
        phase = speed * 2.0f / wavelength;
    }
    ImGui::DragFloat( "Amplitude", &amplitude, 0.1f );
    if ( ImGui::DragFloat( "WaveLength", &wavelength, 0.1f ) )
    {
        frequency = 2.0f / wavelength;
        phase = speed * 2.0f / wavelength;
    }
    if ( ImGui::DragFloat( "Direction", &direction_deg, 1.0f, 0.f, 360.f ) )
    {
        direction = glm::normalize( glm::vec2{ cos( glm::radians( direction_deg ) ), sin( glm::radians( direction_deg ) ) } );
    }

    if ( type != WT_Sine )
    {
        if ( type == WT_SteepSine )
        {
            ImGui::DragFloat( "Steepness", &steepness, 0.1f, 1.0f, 10.0f );
        }
        else if ( type == WT_Gerstner )
        {
            ImGui::DragFloat( "Steepness", &steepness, 0.1f, 0.0f, 1.0f );
        }
        else
        {
            assert( 0 );
        }
    }
}

void Wave::UpdateWater( glm::vec3& vertice, glm::vec3& normal )
{
    // TODO: delete
}

float Wave::Sine( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    return sin( frequency * DirectionDotxz + t * phase ) * amplitude;
}

glm::vec2 Wave::SineNormal( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    float dx = frequency * amplitude * direction.x * cos( DirectionDotxz * frequency + t * phase );
    float dz = frequency * amplitude * direction.y * cos( DirectionDotxz * frequency + t * phase );

    return glm::vec2( dx, dz );
}

float Wave::SteepSine( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    float base = (sin( DirectionDotxz * frequency + t * phase ) + 1) / 2.0f; // move sine to 0.0 ~ 1.0
    return 2 * amplitude * pow( base, steepness );
}

glm::vec2 Wave::SteepSineNormal( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    float h = 2 * amplitude * pow( (sin( DirectionDotxz * frequency + t * phase ) + 1) / 2.0f, steepness - 1 );
    float dx = steepness * direction.x * frequency * amplitude * h * cos( DirectionDotxz * frequency + t * phase );
    float dy = steepness * direction.y * frequency * amplitude * h * cos( DirectionDotxz * frequency + t * phase );

    return glm::vec2( dx, dy );
}

glm::vec3 Wave::Gerstner( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    glm::vec3 g{0.f};
    g.x = steepness * amplitude * direction.x * cos( frequency * DirectionDotxz + t /** phase*/ );
    g.z = steepness * amplitude * direction.y * cos( frequency * DirectionDotxz + t /** phase*/ );
    g.y = amplitude * sin( frequency * DirectionDotxz + GetTime() );

    return g;
}

glm::vec3 Wave::GerstnerNormal( glm::vec3 v )
{
    float t = GetTime();

    glm::vec2 xz = { v.x, v.z };
    auto DirectionDotxz = glm::dot( direction, xz );

    glm::vec3 n{0.f};

    float wa = frequency * amplitude;
    float s = sin( frequency * DirectionDotxz + t /** phase*/ );
    float c = cos( frequency * DirectionDotxz + t /** phase*/ );

    n.x = direction.x * wa * c;
    n.z = direction.y * wa * c;
    n.y = steepness * wa * s;

    return n;
}

Ref<Mesh> Plane::ToMesh() const
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

Ref<Plane> Plane::GeneratePlane( int planeLength, int quadRes )
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
