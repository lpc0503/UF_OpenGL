#pragma once
#ifndef OPENGLPLAYGROUND_ENTITY_H
#define OPENGLPLAYGROUND_ENTITY_H

#include "Core.h"
#include "Utils.h"
#include "Mesh.h"
#include "Renderer.h"

#include <vector>

#include <glm/glm.hpp>

// Scene Graph 實現: 參考 https://learnopengl.com/Guest-Articles/2021/Scene/Scene-Graph
struct Entity
{
    uint32_t id;
    Entity *parent = nullptr;
    std::vector<Entity*> children = {};
    std::string name;
    //
    Ref<Mesh> mesh;
    glm::vec4 color;

    struct Transform
    {
        glm::vec3 pos;
        glm::vec3 rotate;
        glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

        glm::mat4 modelMatrix;

        glm::mat4 GetLocalModelMatrix();
        void GetRTS(glm::vec3 &pos, glm::vec3 &rotate, glm::vec3 &scale);
    } transform;

    Entity(const std::string &name_)
        : name(name_)
    {
        id = s_NextID++;
    }

    static uint32_t s_NextID;
    static Ref<Entity> Create(const std::string &name_);

public:
    void Move(const glm::vec3 &off);

    void Render();
    void UpdateSelfAndChild();

    void AddChild(Entity *ent);
};

#endif //OPENGLPLAYGROUND_ENTITY_H
