#include "RendererAPI.h"

#include "shader.hpp"

#include <glm/gtc/type_ptr.hpp>

void RendererAPI::Init()
{
    INFO_TAG("Renderer/Data", "Init");
    glGenVertexArrays(1, &m_LineVAO);
    glBindVertexArray(m_LineVAO);
    glGenBuffers(1, &m_LineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 4));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 8));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    m_LineShader = LoadShaders("shaders/StandardShading.vert", "shaders/StandardShading.frag");
}

void RendererAPI::Shutdown()
{
    INFO_TAG("Renderer/Data", "Shutdown");
    glDeleteBuffers(1, &m_LineVBO); // TODO: delete IBO?
    glDeleteVertexArrays(1, &m_LineVAO);
    glDeleteProgram(m_LineShader);
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

void RendererAPI::SetMatrix(const std::string &name, const glm::mat4 &mat)
{
    auto id = glGetUniformLocation(m_CurrentShader, name.c_str());
    assert(id >= 0);
    glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat));
}

void RendererAPI::SendLineData()
{
    glBindVertexArray(m_LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_LineVertices.size(), glm::value_ptr(m_LineVertices[0].pos), GL_STATIC_DRAW);
}

void RendererAPI::DrawLines()
{
    glDrawArrays(GL_LINES, 0, m_LineVertices.size());
}

void RendererAPI::Clear()
{
    m_LineVertices.clear();
}
