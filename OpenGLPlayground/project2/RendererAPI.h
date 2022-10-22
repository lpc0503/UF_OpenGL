#pragma once
#ifndef OPENGLPLAYGROUND_RENDERERAPI_H
#define OPENGLPLAYGROUND_RENDERERAPI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Log.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Core.h"

class RendererAPI
{
public:
    void Init();
    void Shutdown();

    void BindShader() const { glUseProgram(m_CurrentShader); }
    void UnbindShader() { glUseProgram(0); m_CurrentShader = 0; } // TODO: remove
    void Clear();

    void SetMatrix(const std::string &name, const glm::mat4 &mat);
    void SetFloat3(const std::string &name, const glm::vec3 &v);
    void SetFloat4(const std::string &name, const glm::vec4 &v);

private:
    GLuint m_CurrentShader;

    GLint GetUniformID(const std::string &name);

    // Line
public:
    void BindLineShader() { assert(m_LineShader); m_CurrentShader = m_LineShader; BindShader(); } // TODO: remove
    void PushLine(const glm::vec3 &p0, const glm::vec3 &p1, const glm::vec4 &color);
    void SendLineData();
    void DrawLines();
private:
    GLuint m_LineVAO, m_LineVBO, m_LineIBO;
    GLuint m_LineShader;
    std::vector<Vertex> m_LineVertices;
    //

    // Mesh
public:
    void BindMeshShader() { assert(m_MeshShader); m_CurrentShader = m_MeshShader; BindShader(); }
    void PushMesh(Ref<Mesh> model, const glm::vec3 &pos, const glm::vec3 &rotate, const glm::vec3 &scale);
    void SendMeshData();
    void DrawMeshs();

private:
    GLuint m_MeshShader;
    GLuint m_MeshVAO, m_MeshVBO, m_MeshIBO;
    std::vector<Ref<Mesh>> m_MeshList;
};

#endif //OPENGLPLAYGROUND_RENDERERAPI_H
