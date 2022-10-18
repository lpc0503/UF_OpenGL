#pragma once
#ifndef OPENGLPLAYGROUND_RENDERER_H
#define OPENGLPLAYGROUND_RENDERER_H

#include <glm/glm.hpp>

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene();
    static void EndScene();

    static void DrawPoint()
    static void DrawLine(const glm::vec3& p0, glm::vec3& p1, const glm::vec4& color);
};

#endif //OPENGLPLAYGROUND_RENDERER_H
