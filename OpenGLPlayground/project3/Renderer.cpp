#include "Renderer.h"
#include "RendererAPI.h"
#include "Log.h"
#include "Entity.h"
#include <array>

#include <glad/glad.h>
#include <memory>
#include <vector>

RendererAPI *g_RenderAPI;
bool g_IsRendering = false;

// TODO: pimpl
Ref<Camera> g_CurrentCamera;

struct RendererData
{
    float CurrentPointSize = 3.f;
};

Uniq<RendererData> Renderer::m_RenderData = nullptr;
Renderer::ShaderMode Renderer::m_ShaderMode;

void Renderer::Init()
{
    INFO_TAG("Renderer", "Init");
    g_RenderAPI = new RendererAPI();
    g_RenderAPI->Init();

    m_ShaderMode = ShaderMode::TESSELATION;
    m_RenderData = MakeUniq<RendererData>();

    g_RenderAPI->SetTessInnerLevel(4);
    g_RenderAPI->SetTessOuterLevel({1.f, 1.f, 1.f});
//
//    TessInner = 12;
//    TessOuter = {12.f, 12.f, 12.f};

}

void Renderer::Shutdown()
{
    INFO_TAG("Renderer", "Shutdown");
    g_RenderAPI->Shutdown();
    delete g_RenderAPI;
}

void Renderer::BeginScene(Ref<Camera> camera)
{
    g_CurrentCamera = camera;
    g_IsRendering = true;

    g_RenderAPI->BindPointShader();
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindLineShader();
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindTriangleShader();
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->SetBool("sampler2D", 0);
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindMeshShader();
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindQuadMeshShader();
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->UnbindShader();
}

void Renderer::EndScene()
{
    g_RenderAPI->BindPointShader();
    g_RenderAPI->SetFloat("vertexPointSize", m_RenderData->CurrentPointSize);
    g_RenderAPI->SendPointData();
    g_RenderAPI->DrawPoints();
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindLineShader();
    g_RenderAPI->SendLineData();
//    g_RenderAPI->SetBool("uEnableLight", false);
    g_RenderAPI->DrawLines();
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindTriangleShader();
    g_RenderAPI->SendTriangleData();
//    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->DrawTriangles();
    g_RenderAPI->UnbindShader();

    // TODO: figure out how to batch rendering mesh
//    g_RenderAPI->BindMeshShader();
//    g_RenderAPI->SendMeshData();
//    g_RenderAPI->DrawMeshes();
//    g_RenderAPI->UnbindShader();

    g_RenderAPI->DrawMeshes();

    g_RenderAPI->ClearRendererState();
    g_IsRendering = false;
}

void Renderer::BeginPickingScene(Ref<Camera> camera)
{
    g_IsRendering = true;
    g_CurrentCamera = camera;
}

void Renderer::EndPickingScene()
{
    g_RenderAPI->BindPickingShader();
    g_RenderAPI->DrawEntitiesForPicking(g_CurrentCamera);
    g_RenderAPI->UnbindShader();

    g_IsRendering = false;
}

void Renderer::DrawPoint(const glm::vec3 &p0, const glm::vec4 &color, const float pointSize)
{
    m_RenderData->CurrentPointSize = pointSize;
    g_RenderAPI->PushPoint(p0, color);
}

void Renderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    g_RenderAPI->PushLine(p0, p1, color);
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

    g_RenderAPI->PushTriangle(vertices);
}

void Renderer::DrawMesh(Ref<Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint, const bool &quad)
{
    g_RenderAPI->PushMesh(mesh, pos, rotate, scale, tint, quad);
}

void Renderer::DrawDirectionalLight(const glm::vec3 &dir, const glm::vec4 &color)
{
    g_RenderAPI->PushDirectionalLight(dir, color);
}

bool Renderer::IsSceneRendering()
{
    return g_IsRendering;
}

void Renderer::TestPickingEntity(Ref<Entity> entity)
{
    g_RenderAPI->PushPickingEntity(entity);
}

void Renderer::SetRendererMode(RendererMode mode)
{
    const auto modeAPI = static_cast<RendererAPI::RendererMode>(mode);
    g_RenderAPI->SetRendererMode(modeAPI);
}

Renderer::RendererMode Renderer::GetRendererMode()
{
    auto mode = g_RenderAPI->GetRendererMode();
    return static_cast<Renderer::RendererMode>(mode);
}

void Renderer::ClearViewport()
{
    g_RenderAPI->ClearViewport();
}

void Renderer::SetViewportSize(int x, int y, int w, int h)
{
    g_RenderAPI->SetViewportSize(x, y, w, h);
}

void Renderer::SetShaderMode(Renderer::ShaderMode mode)
{
    m_ShaderMode = mode;
    g_RenderAPI->SetShaderMode(static_cast<RendererAPI::ShaderMode>(mode));
}

Renderer::ShaderMode Renderer::GetShaderMode()
{
    return static_cast<Renderer::ShaderMode>(g_RenderAPI->GetShaderMode());
}

void Renderer::SetClearColor(const glm::vec4 &color)
{
    g_RenderAPI->SetClearColor(color);
}

void Renderer::SetTessInnerLevel(const float tessInner)
{
    g_RenderAPI->SetTessInnerLevel(tessInner);
}

void Renderer::SetTessOuterLevel(const glm::vec3 &tessOuter)
{
    g_RenderAPI->SetTessOuterLevel(tessOuter);
}
