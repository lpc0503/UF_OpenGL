#include "Renderer.h"
#include "RendererData.h"
#include "Log.h"

#include <glad/glad.h>
#include <memory>
#include <vector>

RendererData *g_RenderData;

void Renderer::Init()
{
    INFO_TAG("Renderer", "Init");
    g_RenderData = new RendererData();
    g_RenderData->Init();
}

void Renderer::Shutdown()
{
    INFO_TAG("Renderer", "Shutdown");
    g_RenderData->Shutdown();
    delete g_RenderData;
}

void Renderer::BeginScene(Ref<Camera> camera)
{
    g_RenderData->BindLineShader();
    g_RenderData->SetMatrix("M", glm::mat4(1.f));
    g_RenderData->SetMatrix("V", camera->GetView());
    g_RenderData->SetMatrix("P", camera->GetProjection());
    g_RenderData->UnbindShader();
}

void Renderer::EndScene()
{
    g_RenderData->BindLineShader();
    g_RenderData->SendLineData();
    g_RenderData->DrawLines();
    g_RenderData->UnbindShader();
}

void Renderer::DrawPoint()
{

}

void Renderer::DrawLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color)
{
    g_RenderData->PushLine(p0, p1, color);
}

void Renderer::DrawPointLight(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec4 &color, float intensity)
{

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
