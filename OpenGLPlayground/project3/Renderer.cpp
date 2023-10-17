#include "Renderer.h"
#include "RenderAPI.h"
#include "OpenGLRenderAPI.h"
#include "Log.h"
#include "Entity.h"
#include "Application.h"
#include "OpenGLApplication.h"

#include <array>
#include <glad/glad.h>
#include <memory>
#include <vector>

Application* Renderer::s_App = nullptr;
OpenGLRenderAPI* Renderer::s_RenderAPI = nullptr;
bool g_IsRendering = false;

// TODO: pimpl
Ref<Camera> g_CurrentCamera;

struct RendererData
{
    float CurrentPointSize = 3.f;
};

Uniq<RendererData> Renderer::m_RenderData = nullptr;
Renderer::ShaderMode Renderer::m_ShaderMode;

void Renderer::Init(Application *app, IRenderAPI *api)
{
    INFO_TAG("Renderer", "Init");

    s_App = app;
    s_RenderAPI = dynamic_cast<OpenGLRenderAPI*>(api); // TODO: Remove dynamic cast in the future *Many WORK*
    s_RenderAPI->Init();

    m_ShaderMode = ShaderMode::TESSELATION;
    m_RenderData = MakeUniq<RendererData>();

    s_RenderAPI->SetTessInnerLevel(4);
    s_RenderAPI->SetTessOuterLevel({1.f, 1.f, 1.f});
//
//    TessInner = 12;
//    TessOuter = {12.f, 12.f, 12.f};

}

void Renderer::Shutdown()
{
    INFO_TAG("Renderer", "Shutdown");
    s_RenderAPI->Shutdown();
    delete s_RenderAPI;
}

void Renderer::BeginScene(Ref<Camera> camera)
{
    g_CurrentCamera = camera;
    g_IsRendering = true;

    s_RenderAPI->BindPointShader();
    s_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    s_RenderAPI->SetMatrix("V", camera->GetView());
    s_RenderAPI->SetMatrix("P", camera->GetProjection());
    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindLineShader();
    s_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    s_RenderAPI->SetMatrix("V", camera->GetView());
    s_RenderAPI->SetMatrix("P", camera->GetProjection());
    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindTriangleShader();
    s_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    s_RenderAPI->SetMatrix("V", camera->GetView());
    s_RenderAPI->SetMatrix("P", camera->GetProjection());
    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->SetBool("sampler2D", 0);
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindMeshShader();
    s_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    s_RenderAPI->SetMatrix("V", camera->GetView());
    s_RenderAPI->SetMatrix("P", camera->GetProjection());
    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindQuadMeshShader();
    s_RenderAPI->SetMatrix("V", camera->GetView());
    s_RenderAPI->SetMatrix("P", camera->GetProjection());
    s_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->UnbindShader();
}

void Renderer::EndScene()
{
    s_RenderAPI->BindPointShader();
    s_RenderAPI->SetFloat("vertexPointSize", m_RenderData->CurrentPointSize);
    s_RenderAPI->SendPointData();
    s_RenderAPI->DrawPoints();
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindLineShader();
    s_RenderAPI->SendLineData();
//    s_RenderAPI->SetBool("uEnableLight", false);
    s_RenderAPI->DrawLines();
    s_RenderAPI->UnbindShader();

    s_RenderAPI->BindTriangleShader();
    s_RenderAPI->SendTriangleData();
//    s_RenderAPI->SetBool("uEnableLight", true);
    s_RenderAPI->DrawTriangles();
    s_RenderAPI->UnbindShader();

    // TODO: figure out how to batch rendering mesh
//    s_RenderAPI->BindMeshShader();
//    s_RenderAPI->SendMeshData();
//    s_RenderAPI->DrawMeshes();
//    s_RenderAPI->UnbindShader();

    s_RenderAPI->DrawMeshes();

    s_RenderAPI->ClearRendererState();
    g_IsRendering = false;
}

void Renderer::BeginPickingScene(Ref<Camera> camera)
{
    g_IsRendering = true;
    g_CurrentCamera = camera;
}

void Renderer::EndPickingScene()
{
    s_RenderAPI->BindPickingShader();
    s_RenderAPI->DrawEntitiesForPicking(g_CurrentCamera);
    s_RenderAPI->UnbindShader();

    g_IsRendering = false;
}

void Renderer::DrawPoint(const glm::vec3 &p0, const glm::vec4 &color, const float pointSize)
{
    m_RenderData->CurrentPointSize = pointSize;
    s_RenderAPI->PushPoint(p0, color);
}

void Renderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    s_RenderAPI->PushLine(p0, p1, color);
}

void Renderer::DrawPointLight(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec4 &color, float intensity)
{
    // TODO: implement
}

void Renderer::DrawGrid(int n, int m)
{
    for(int x = -n; x <= n; x++)
    {
        if(x == 0)
            Renderer::DrawLine({x, 0.f, 0}, {x, 0.f, -m}, {1.f, 1.f, 1.f, 1.f});
        else
            Renderer::DrawLine({x, 0.f, m}, {x, 0.f, -m}, {1.f, 1.f, 1.f, 1.f});
    }

    for(int z = -m; z <= m; z++)
    {
        if(z == 0)
            Renderer::DrawLine({0, 0.f, z}, {-n, 0.f, z}, {1.f, 1.f, 1.f, 1.f});
        else
            Renderer::DrawLine({n, 0.f, z}, {-n, 0.f, z}, {1.f, 1.f, 1.f, 1.f});
    }

    float l = std::max(n, m);
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {l, 0.f, 0.f}, glm::vec4 {1.f, 0.f, 0.f, 1.f});
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {0.f, l, 0.f}, glm::vec4 {0.f, 1.f, 0.f, 1.f});
    Renderer::DrawLine(glm::vec3 {0, 0, 0}, glm::vec3 {0.f, 0.f, l}, glm::vec4 {0.f, 0.f, 1.f, 1.f});
}

void Renderer::DrawModel(Ref<Model> model, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale)
{
    for(auto mesh : model->GetMeshes())
    {

    }
}

void Renderer::DrawTriangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const std::vector<glm::vec2> &uv, const glm::vec4 &color)
{
    std::array<Vertex, 3> vertices;

    Vertex v;
    v.pos = {p0, 1.f};
    v.color = color;
    v.uv = uv[0];
    vertices[0] = v;

    v = {};
    v.pos = {p1, 1.f};
    v.color = color;
    v.uv = uv[1];
    vertices[1] = v;

    v = {};
    v.pos = {p2, 1.f};
    v.color = color;
    v.uv = uv[2];
    vertices[2] = v;

    s_RenderAPI->PushTriangle(vertices);
}

void Renderer::DrawMesh(Ref<Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint, const bool &quad)
{
    s_RenderAPI->PushMesh(mesh, pos, rotate, scale, tint, quad);
}

void Renderer::DrawDirectionalLight(const glm::vec3 &dir, const glm::vec4 &color)
{
    s_RenderAPI->PushDirectionalLight(dir, color);
}

bool Renderer::IsSceneRendering()
{
    return g_IsRendering;
}

void Renderer::TestPickingEntity(Ref<Entity> entity)
{
    s_RenderAPI->PushPickingEntity(entity);
}

void Renderer::SetRendererMode(RendererMode mode)
{
    const auto modeAPI = static_cast<OpenGLRenderAPI::RendererMode>(mode);
    s_RenderAPI->SetRendererMode(modeAPI);
}

Renderer::RendererMode Renderer::GetRendererMode()
{
    auto mode = s_RenderAPI->GetRendererMode();
    return static_cast<Renderer::RendererMode>(mode);
}

void Renderer::ClearViewport()
{
    s_RenderAPI->ClearViewport();
}

void Renderer::SetViewportSize(int x, int y, int w, int h)
{
    s_RenderAPI->SetViewportSize(x, y, w, h);
}

void Renderer::SetShaderMode(Renderer::ShaderMode mode)
{
    m_ShaderMode = mode;
    s_RenderAPI->SetShaderMode(static_cast<OpenGLRenderAPI::ShaderMode>(mode));
}

Renderer::ShaderMode Renderer::GetShaderMode()
{
    return static_cast<Renderer::ShaderMode>(s_RenderAPI->GetShaderMode());
}

void Renderer::SetClearColor(const glm::vec4 &color)
{
    s_RenderAPI->SetClearColor(color);
}

void Renderer::SetTessInnerLevel(const float tessInner)
{
    s_RenderAPI->SetTessInnerLevel(tessInner);
}

void Renderer::SetTessOuterLevel(const glm::vec3 &tessOuter)
{
    s_RenderAPI->SetTessOuterLevel(tessOuter);
}

void Renderer::WaitForGPUCompletion()
{
    s_RenderAPI->WaitForGPUCompletion();
}

void Renderer::FlushBuffers()
{
    s_RenderAPI->FlushBuffers();
}
