#include "RendererAPI.h"
#include "Entity.h"
#include "Camera.h"

//#include "Utils.h"
#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

bool CheckOpenGLError()
{
    auto err = glGetError();
    if(err != GL_NO_ERROR)
    {
        ERROR_TAG(TAG_OPENGL, "Error: {}", err);
        return false;
    }
    return true;
}

void RendererAPI::Init()
{
    INFO_TAG("Renderer/Data", "Init");

    InitPointRenderer();
    InitLineRenderer();
    InitMeshRenderer();

    m_ShaderMode = ShaderMode::STANDARD;
    m_PickingShader = LoadShaders("shaders/Picking.vert", "shaders/Picking.frag");
}

void RendererAPI::Shutdown()
{
    INFO_TAG("Renderer/Data", "Shutdown");
    glDeleteBuffers(1, &m_LineVBO); // TODO: delete IBO?
    glDeleteVertexArrays(1, &m_LineVAO);
    glDeleteProgram(m_LineShader);

    glDeleteBuffers(1, &m_MeshVBO);
    glDeleteBuffers(1, &m_MeshIBO);
    glDeleteVertexArrays(1, &m_MeshVAO);
    glDeleteProgram(m_MeshShader);
}

void RendererAPI::SetMatrix(const std::string &name, const glm::mat4 &mat)
{
    auto id = GetUniformID(name);
    glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat));
}

void RendererAPI::SetFloat(const std::string &name, const float f)
{
    auto id = GetUniformID(name);
    glUniform1f(id, f);
}

void RendererAPI::SetFloat3(const std::string &name, const glm::vec3 &v)
{
    auto id = GetUniformID(name);
    glUniform3f(id, v.x, v.y, v.z);
}

void RendererAPI::SetFloat4(const std::string &name, const glm::vec4 &v)
{
    auto id = GetUniformID(name);
    glUniform4f(id, v.x, v.y, v.z, v.w);
}

void RendererAPI::SetBool(const std::string &name, bool b)
{
    auto id = GetUniformID(name);
    glUniform1i(id, b);
}

GLint RendererAPI::GetUniformID(const std::string &name)
{
    auto id = glGetUniformLocation(m_CurrentShader, name.c_str());
    if(!CheckOpenGLError())
    {
        ASSERT(id != -1, "Invalid shader uniform name");
    }
    return id;
}

void RendererAPI::ClearRendererState()
{
    UnbindShader();
    m_LineVertices.clear();
    m_PointVertices.clear();
    m_Meshes.clear();
}

//////////////////////////////////////////////////////////////////////
// Line
//////////////////////////////////////////////////////////////////////

void RendererAPI::SendLineData()
{
    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_LineVertices.size(), glm::value_ptr(m_LineVertices[0].pos), GL_STATIC_DRAW);
}

void RendererAPI::PushLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    Vertex v0, v1;
    v0.pos = glm::vec4(p0, 1.f);
    v0.color = color;
    v0.normal = glm::vec4{0.f};
    m_LineVertices.emplace_back(v0);

    v1.pos = glm::vec4(p1, 1.f);
    v1.color = color;
    v1.normal = glm::vec4{0.f};
    m_LineVertices.emplace_back(v1);
}

void RendererAPI::DrawLines()
{
    glDrawArrays(GL_LINES, 0, m_LineVertices.size());
}

//////////////////////////////////////////////////////////////////////
// Point
//////////////////////////////////////////////////////////////////////

void RendererAPI::PushPoint(const glm::vec3 &p0, const glm::vec4 &color) {

    Vertex v0;
    v0.pos = glm::vec4(p0, 1.f);
    v0.color = color;
    v0.normal = glm::vec4{0.f};
    m_PointVertices.emplace_back(v0);
}

void RendererAPI::SendPointData() {

    glBindVertexArray(m_PointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_PointVertices.size(), glm::value_ptr(m_PointVertices[0].pos), GL_STATIC_DRAW);
}

void RendererAPI::DrawPoints() {

    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, m_PointVertices.size());
    glDisable(GL_PROGRAM_POINT_SIZE);
}

//////////////////////////////////////////////////////////////////////
// Mesh
//////////////////////////////////////////////////////////////////////

void RendererAPI::PushMesh(Ref <Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint)
{
    m_Meshes.emplace_back(MeshData{mesh, pos, rotate, scale, tint});
}

void RendererAPI::SendMeshData()
{
    /*glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_LineVertices.size(), glm::value_ptr(m_LineVertices[0].pos), GL_STATIC_DRAW);*/
}

void RendererAPI::DrawMeshes()
{
    auto SendModelMatrix = [&](const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint)
    {
        glm::mat4 m{1.f};
        m = glm::translate(m, pos);
        m = glm::rotate(m, glm::radians(rotate.x), {1.f, 0.f, 0.f});
        m = glm::rotate(m, glm::radians(rotate.y), {0.f, 1.f, 0.f});
        m = glm::rotate(m, glm::radians(rotate.z), {0.f, 0.f, 1.f});
        m = glm::scale(m, scale);
        SetMatrix("M", m);
        SetFloat4("Color", tint);
    };
    auto DrawMesh = [&](Ref<Mesh> mesh)
    {
        glBindVertexArray(m_MeshVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_MeshVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->m_Vertices.size(), glm::value_ptr(mesh->m_Vertices[0].pos), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_MeshIBO);
//        for(auto x : mesh->m_Vertices) {
//
//            INFO("{} {} {}", x.normal.x, x.normal.y, x.normal.z);
//        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh->m_Indices.size(), &mesh->m_Indices[0], GL_STATIC_DRAW);
        if(GetShaderMode() == ShaderMode::STANDARD) {
            glDrawElements(GL_TRIANGLES, mesh->m_Indices.size(), GL_UNSIGNED_INT, 0);
        }
        else if(GetShaderMode() == ShaderMode::GEOMETRY) {
            glDrawElements(GL_TRIANGLES, mesh->m_Indices.size(), GL_UNSIGNED_INT, 0);
        }
        else if(GetShaderMode() == ShaderMode::TESSELATION) {
            glPatchParameteri(GL_PATCH_VERTICES, 3);
            glDrawElements(GL_PATCHES, mesh->m_Indices.size(), GL_UNSIGNED_INT, 0);
        }
    };

    BindMeshShader();
    SendLightData();
    for(MeshData &md : m_Meshes)
    {
        SendModelMatrix(md.pos, md.rotate, md.scale, md.tint);
        DrawMesh(md.mesh);
    }
    UnbindShader();
}

//////////////////////////////////////////////////////////////////////
// Light
//////////////////////////////////////////////////////////////////////

void RendererAPI::PushDirectionalLight(const glm::vec3 &dir, const glm::vec4 &color)
{
    m_DirectionalLight.dir = dir;
    m_DirectionalLight.color = color;
}

void RendererAPI::SendLightData()
{
    SetFloat3("dirLight.dir", m_DirectionalLight.dir);
    SetFloat3("dirLight.ambient", m_DirectionalLight.color);
    SetFloat3("dirLight.diffuse", m_DirectionalLight.color);
}

//////////////////////////////////////////////////////////////////////
// Entity
//////////////////////////////////////////////////////////////////////

void RendererAPI::DrawEntitiesForPicking(Ref<Camera> camera)
{
    auto SendModelMatrix = [&](const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale)
    {
        glm::mat4 m{1.f};
        m = glm::translate(m, pos);
        m = glm::rotate(m, glm::radians(rotate.x), {1.f, 0.f, 0.f});
        m = glm::rotate(m, glm::radians(rotate.y), {0.f, 1.f, 0.f});
        m = glm::rotate(m, glm::radians(rotate.z), {0.f, 0.f, 1.f});
        m = glm::scale(m, scale);
        m = camera->GetProjection() * camera->GetView() * m; // P * V * M
        SetMatrix("MVP", m);
    };
    auto DrawMesh = [&](Ref<Mesh> mesh)
    {
        glBindVertexArray(m_MeshVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_MeshVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->m_Vertices.size(), glm::value_ptr(mesh->m_Vertices[0].pos), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_MeshIBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh->m_Indices.size(), &mesh->m_Indices[0], GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, mesh->m_Indices.size(), GL_UNSIGNED_INT, 0);
    };

    for(auto entity : m_Entities)
    {
        glm::vec3 pos, rotate, scale;
        entity->transform.GetRTS(pos, rotate, scale);
        SendModelMatrix(pos, rotate, scale);
        SetFloat("PickingID", entity->id);
        DrawMesh(entity->mesh);
    }
}

void RendererAPI::SetRendererMode(RendererMode mode) {

    switch(mode)
    {
        case RendererMode::Line:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            break;
        case RendererMode::Fill:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case RendererMode::Point:
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glPointSize(5.f);
            break;
        default:
            ASSERT(0, "Failed");
    }
}

RendererAPI::RendererMode RendererAPI::GetRendererMode()
{
    GLint polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

    switch (polygonMode)
    {
        case GL_LINE:
            return RendererAPI::RendererMode::Line;
        case GL_FILL:
            return RendererAPI::RendererMode::Fill;
        case GL_POINTS:
            return RendererAPI::RendererMode::Point;
        default:
            ASSERT(0, "Failed");
    }
    return RendererAPI::RendererMode::Invalid;
}

void RendererAPI::ClearViewport()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::InitPointRenderer()
{
    glGenVertexArrays(1, &m_PointVAO);
    glBindVertexArray(m_PointVAO);
    glGenBuffers(1, &m_PointVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PointVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 11));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    m_PointShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
}

void RendererAPI::InitLineRenderer()
{
    glGenVertexArrays(1, &m_LineVAO);
    glBindVertexArray(m_LineVAO);
    glGenBuffers(1, &m_LineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 11));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    m_LineShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
}

void RendererAPI::InitMeshRenderer()
{
    glGenVertexArrays(1, &m_MeshVAO);
    glBindVertexArray(m_MeshVAO);
    glGenBuffers(1, &m_MeshVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_MeshVBO);
    glGenBuffers(1, &m_MeshIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_MeshIBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 11));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    m_MeshShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
}

void RendererAPI::SetShaderMode(RendererAPI::ShaderMode mode)
{
    m_ShaderMode = mode;

    if(m_ShaderMode == ShaderMode::STANDARD) {
        m_PointShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
        m_LineShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
        m_MeshShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
    }

    if(m_ShaderMode == ShaderMode::TESSELATION) {
//        m_PointShader = LoadTessShaders("shaders/Tess.vert", "shaders/Tess.tesc", "shaders/Tess.tese","shaders/Tess.frag");
//        m_LineShader = LoadTessShaders("shaders/Tess.vert", "shaders/Tess.tesc", "shaders/Tess.tese","shaders/Tess.frag");
        m_MeshShader = LoadTessShaders("shaders/Tess.vert", "shaders/Tess.tesc", "shaders/Tess.tese","shaders/Tess.frag");
    }

    if(m_ShaderMode == ShaderMode::GEOMETRY) {
        m_MeshShader = LoadGeoShaders("shaders/Geo.vert", "shaders/Geo.geom", "shaders/Geo.frag");
    }
}

RendererAPI::ShaderMode RendererAPI::GetShaderMode()
{
    return m_ShaderMode;
}

void RendererAPI::SetClearColor(const glm::vec4 &color)
{
    glClearColor(color.x, color.y, color.z, color.w);
}
