//
// Created by ICEJJ on 2023/5/11.
//
#pragma once
#ifndef OPENGLPLAYGROUND_PNTRIANGLE_H
#define OPENGLPLAYGROUND_PNTRIANGLE_H

#include "Core.h"
#include "Vertex.h"
#include <vector>

class PNTriangle {

private:

public:

    PNTriangle() = default;

    std::vector<glm::vec4> GenControlPoint(std::vector<Vertex>& vertices, float &u, float &v);

};


#endif //OPENGLPLAYGROUND_PNTRIANGLE_H
