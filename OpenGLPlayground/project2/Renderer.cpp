#include "Renderer.h"
#include "RendererAPI.h"
#include "Log.h"

#include <glad/glad.h>
#include <memory>
#include <vector>

RendererAPI *g_RenderAPI;

void Renderer::Init()
{
    INFO_TAG("Renderer", "Init");
    g_RenderAPI = new RendererAPI();
    g_RenderAPI->Init();
}

void Renderer::Shutdown()
{
    INFO_TAG("Renderer", "Shutdown");
    g_RenderAPI->Shutdown();
    delete g_RenderAPI;
}

void Renderer::BeginScene(Ref<Camera> camera)
{
    g_RenderAPI->BindLineShader();
    g_RenderAPI->SetMatrix("M", glm::mat4(1.f));
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", false);
    g_RenderAPI->UnbindShader();

    g_RenderAPI->BindMeshShader();
    g_RenderAPI->SetMatrix("V", camera->GetView());
    g_RenderAPI->SetMatrix("P", camera->GetProjection());
    g_RenderAPI->SetBool("uEnableLight", true);
    g_RenderAPI->UnbindShader();
}

void Renderer::EndScene()
{
    g_RenderAPI->BindLineShader();
    g_RenderAPI->SendLineData();
    g_RenderAPI->DrawLines();
    g_RenderAPI->UnbindShader();

    // TODO: figure out how to batch rendering mesh
//    g_RenderAPI->BindMeshShader();
//    g_RenderAPI->SendMeshData();
//    g_RenderAPI->DrawMeshs();
//    g_RenderAPI->UnbindShader();

    g_RenderAPI->DrawMeshs();

    g_RenderAPI->Clear();
}

void Renderer::DrawPoint()
{
    // TODO: implement
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
}

void Renderer::DrawModel(Ref<Model> model, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale)
{
    for(auto mesh : model->GetMeshes())
    {

    }
}

void Renderer::DrawMesh(Ref<Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale)
{
    g_RenderAPI->PushMesh(mesh, pos, rotate, scale);
}

void Renderer::DrawDirectionalLight(const glm::vec3 &dir, const glm::vec4 &color)
{
    g_RenderAPI->PushDirectionalLight(dir, color);
}
