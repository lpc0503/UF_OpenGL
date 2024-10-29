#pragma once

#include "Core.h"
#include "Mesh.h"

struct Water
{
    Ref<Mesh> mesh;

    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;

    glm::vec3 pos    = { 0.f, 0.f, 0.f };
    glm::vec3 rotate = { 0.f, 0.f, 0.f };
    glm::vec3 scale  = { 1.f, 1.f, 1.f };



};
