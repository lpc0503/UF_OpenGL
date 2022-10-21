#pragma once
#ifndef OPENGLPLAYGROUND_RENDERER_H
#define OPENGLPLAYGROUND_RENDERER_H

#include "Camera.h"
#include "Core.h"

#include <glm/glm.hpp>

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Ref<Camera> camera);
    static void EndScene();

    static void DrawPoint();
    static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color);

    // Gizmo
    static void DrawGrid(int n, int m); // draw grid on x-z plane

    // Light
    static void DrawPointLight(const glm::vec3 &pos, const glm::vec3 &dir, const glm::vec4& color, float intensity);
    static void DrawDirectionalLight(const glm::vec3 &dir, const glm::vec4& color);
};

#endif //OPENGLPLAYGROUND_RENDERER_H
