#pragma once
#ifndef OPENGLPLAYGROUND_RENDERERAPI_H
#define OPENGLPLAYGROUND_RENDERERAPI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Log.h"
#include "Vertex.h"

class RendererAPI
{
public:
    void Init();
    void Shutdown();

    void BindShader() { glUseProgram(m_CurrentShader); }
    void UnbindShader() { glUseProgram(0); m_CurrentShader = 0; } // TODO: remove

    void SetMatrix(const std::string &name, const glm::mat4 &mat);

    // Line
    void BindLineShader() { assert(m_LineShader); m_CurrentShader = m_LineShader; BindShader(); } // TODO: remove
    void PushLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color);
    void SendLineData();
    void DrawLines();
    void Clear();
private:
    GLuint m_LineVAO, m_LineVBO, m_LineIBO;
    GLuint m_LineShader;
    GLuint m_CurrentShader;
    std::vector<Vertex> m_LineVertices;
};

#endif //OPENGLPLAYGROUND_RENDERERAPI_H
