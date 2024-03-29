#pragma once
#ifndef OPENGLPLAYGROUND_OPENGLRENDERAPI_H
#define OPENGLPLAYGROUND_OPENGLRENDERAPI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Log.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Core.h"
#include "RenderAPI.h"

class Camera;
class Entity;
class OpenGLApplication;

class OpenGLRenderAPI : public IRenderAPI
{
    OpenGLApplication *m_App = nullptr;

public:
    OpenGLRenderAPI();
    virtual ~OpenGLRenderAPI() override;

    void Init();
    void Shutdown();

    void BindShader();
    void UnbindShader() { glUseProgram(0); m_CurrentShader = -1; } // TODO: remove
    void ClearRendererState();

    // Shader Uniforms
    void SetMatrix(const std::string &name, const glm::mat4 &mat);
    void SetFloat(const std::string &name, const float f);
    void SetFloat3(const std::string &name, const glm::vec3 &v);
    void SetFloat4(const std::string &name, const glm::vec4 &v);
    void SetBool(const std::string &name, bool b);

private:
    GLuint m_CurrentShader;
    GLint GetUniformID(const std::string &name);

    // ======================================================================================
    // Line
public:
    void InitLineRenderer();
    void BindLineShader() { assert(m_LineShader); m_CurrentShader = m_LineShader; BindShader(); } // TODO: remove
    void PushLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color);
    void SendLineData();
    void DrawLines();
private:
    GLuint m_LineVAO, m_LineVBO, m_LineIBO;
    GLuint m_LineShader;
    std::vector<Vertex> m_LineVertices;

    // ======================================================================================
    // Point
public:
    void InitPointRenderer();
    void BindPointShader() { assert(m_LineShader); m_CurrentShader = m_PointShader; BindShader(); }
    void PushPoint(const glm::vec3 &p0, const glm::vec4 &color);
    void SendPointData();
    void DrawPoints();
private:
    GLuint m_PointVAO, m_PointVBO;
    GLuint m_PointShader;
    std::vector<Vertex> m_PointVertices;

    // ======================================================================================
    // Triangle
public:
    void InitTriangleRenderer();
    void BindTriangleShader() { assert(m_TriangleShader); m_CurrentShader = m_TriangleShader; BindShader(); }
    void PushTriangle(const std::array<Vertex, 3> &t);
    void SendTriangleData();
    void DrawTriangles();

private:
    GLuint m_TriangleVAO, m_TriangleVBO;
    GLuint m_TriangleShader;
    std::vector<Vertex> m_TriangleVertices;

    // ======================================================================================
    // Mesh
public:
    void InitMeshRenderer();
    void BindMeshShader() { assert(m_MeshShader); m_CurrentShader = m_MeshShader; BindShader(); }
    void BindQuadMeshShader() { assert(m_QuadMeshShader); m_CurrentShader = m_QuadMeshShader; BindShader(); }
    void PushMesh(Ref<Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint = {1.f, 1.f, 1.f, 1.f}, const bool &quad=false);
    void SendMeshData();
    void DrawMeshes();
    bool Quad = false;

private:
    GLuint m_MeshShader;
    GLuint m_QuadMeshShader;
    GLuint m_MeshVAO, m_MeshVBO, m_MeshIBO;
    struct MeshData {
        Ref<Mesh> mesh;
        glm::vec3 pos;
        glm::vec3 rotate;
        glm::vec3 scale;
        glm::vec4 tint;
        bool quad;
    };
    std::vector<MeshData> m_Meshes;

    // ======================================================================================
    // Light
public:
//    void BindLightShader() { assert(m_LightShader); m_CurrentShader = m_LightShader; BindShader(); }
    void PushPointLight(const glm::vec3 &pos, const glm::vec4 &color, float intensity);
    void PushDirectionalLight(const glm::vec3 &dir, const glm::vec4 &color);
    void SendLightData();

    struct DirectionalLightData {
        glm::vec3 dir;
        glm::vec4 color;
    };
    DirectionalLightData m_DirectionalLight;
    static constexpr size_t MaxNumLight = 10;

    // ======================================================================================
    // Picking
public:
    void BindPickingShader() { assert(m_PickingShader); m_CurrentShader = m_PickingShader; BindShader(); }
private:
    GLuint m_PickingShader;

    // ======================================================================================
    // Entity
public:
    void PushPickingEntity(Ref<Entity> entity) { m_Entities.push_back(entity); }
    void DrawEntitiesForPicking(Ref<Camera> camera);
private:
    std::vector<Ref<Entity>> m_Entities;

    // ======================================================================================
    // Mode
public:
    enum class RendererMode : uint8_t {
        Fill,
        Line,
        Point,
        Invalid = 0xFF
    };
    void SetRendererMode(RendererMode mode);
    RendererMode GetRendererMode();

    // ======================================================================================
    // Shader Mode
    enum class ShaderMode : uint8_t {
        STANDARD,
        TESSELATION,
        GEOMETRY
    };
    void SetShaderMode(ShaderMode mode);
    ShaderMode GetShaderMode() const;
private:
    ShaderMode m_ShaderMode;

    // ======================================================================================
public:
    void ClearViewport();
    void SetViewportSize(int x, int y, int w, int h);
    void SetClearColor(const glm::vec4 &color);

    void WaitForGPUCompletion();
    void FlushBuffers();

public:
    void SetTessInnerLevel(const float tessInner) { m_TessInnerLevel = tessInner; }
    float GetTessInnerLevel() const { return m_TessInnerLevel; }
    void SetTessOuterLevel(const glm::vec3& tessOuter) { m_TessOuterLevel = tessOuter; }
    glm::vec3 GetTessOuterLevel() const { return m_TessOuterLevel; }
private:
    int m_TessInnerLevel;
    glm::vec3 m_TessOuterLevel;

    // ======================================================================================
    // Texture
private:
    GLuint m_Texture;
};

#endif //OPENGLPLAYGROUND_OPENGLRENDERAPI_H
