#pragma once
#ifndef OPENGLPLAYGROUND_MODEL_H
#define OPENGLPLAYGROUND_MODEL_H

#include "Core.h"
#include "Mesh.h"
#include <glad/glad.h>

#include <filesystem>

class Model
{
public:
    Model() = default;
    Model(const std::string &path);

    std::string GetPath() { return m_Path; }
    std::string GetDirectory() { return std::filesystem::path(m_Path).parent_path().string(); }
    const std::vector<Ref<Mesh>>& GetMeshes() const { return m_Meshes; }
    size_t GetMeshCount() const { return m_Meshes.size(); }

    static Ref<Model> LoadModel(const std::string &path);
    static Ref<Model> LoadModel(const std::string &path, glm::vec4 color);
    static Ref<Model> LoadModel(const std::string &path, glm::vec3 color);

    std::vector<Ref<Mesh>> m_Meshes;
private:

    std::string m_Path;
};

#endif //OPENGLPLAYGROUND_MODEL_H
