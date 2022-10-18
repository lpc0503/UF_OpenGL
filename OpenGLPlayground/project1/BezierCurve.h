#pragma once
#ifndef OPENGLPLAYGROUND_BEZIERCURVE_H
#define OPENGLPLAYGROUND_BEZIERCURVE_H

#include <vector>
#include <glad/glad.h>
#include "Geometry.h"

class BezierCurve {

public:
    BezierCurve() = default;
    BezierCurve(Vertex p[], size_t size);

public:
    void Clear();
    void ToVertex();

public: // Get & Set
    void SetVertices(Vertex p[], size_t size);

//private:
    void Calc();

    std::vector<Point> A, B; // K+1, K
    std::vector<Vertex> V;
    std::vector<GLushort> I;
    std::vector<Point> Origin;
    static const Point ZeroPoint;
};

#endif //OPENGLPLAYGROUND_BEZIERCURVE_H
