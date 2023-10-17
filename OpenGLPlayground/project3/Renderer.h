#pragma once
#ifndef OPENGLPLAYGROUND_RENDERER_H
#define OPENGLPLAYGROUND_RENDERER_H

#include "Camera.h"
#include "Core.h"
#include "Model.h"

#include <glm/glm.hpp>

class Entity;
struct RendererData;

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Ref<Camera> camera);
    static void EndScene();

    static bool IsSceneRendering(); // not thread-safe

    static void ClearViewport();
    static void SetViewportSize(int x, int y, int w, int h);
    static void SetClearColor(const glm::vec4 &color);

    static void DrawPoint(const glm::vec3 &p0, const glm::vec4 &color, const float pointSize);
    static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

    static void DrawMesh(Ref<Mesh> mesh, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale, const glm::vec4 &tint = {1.f, 1.f, 1.f, 1.f}, const bool &quad=false);
    static void DrawModel(Ref<Model> model, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale);

    static void DrawTriangle(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec3 &p2, const std::vector<glm::vec2> &uv, const glm::vec4 &color = {1.f, 1.f, 1.f, 1.f});

    // Gizmo
    static void DrawGrid(int n, int m); // draw grid on x-z plane

    // Light
    static void DrawPointLight(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec4& color, float intensity);
    static void DrawDirectionalLight(const glm::vec3 &dir, const glm::vec4& color);

    static void TestPickingEntity(Ref<Entity> entity);

    static void BeginPickingScene(Ref<Camera> camera);
    static void EndPickingScene();

    // Mode
    enum class RendererMode : uint8_t {

        Fill,
        Line,
        Points
    };
    static void SetRendererMode(RendererMode mode);
    static RendererMode GetRendererMode();

    enum class ShaderMode : uint8_t {

        STANDARD,
        TESSELATION,
        GEOMETRY
    };

    static void SetShaderMode(ShaderMode mode);
    static ShaderMode GetShaderMode();

    static void SetTessInnerLevel(const float tessInner);
    static void SetTessOuterLevel(const glm::vec3& tessOuter);

private:

    static ShaderMode m_ShaderMode;
    static Uniq<RendererData> m_RenderData;
};

#endif //OPENGLPLAYGROUND_RENDERER_H
