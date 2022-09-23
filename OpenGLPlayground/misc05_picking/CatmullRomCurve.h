#pragma once
#ifndef OPENGLPLAYGROUND_CATMULLROMCURVE_H
#define OPENGLPLAYGROUND_CATMULLROMCURVE_H

#include <vector>
#include <glad/glad.h>
#include "Geometry.h"

class CatmullRomCurve
{
public:
    CatmullRomCurve() = default;
    CatmullRomCurve(Vertex p[], size_t size);

public:
    void Clear();
    void ToVertex();

public: // Get & Set
    void SetVertices(Vertex p[], size_t size);

//private:
    void Calc();

    float step = 0.1f;
    std::vector<Point> A, B, C;
    std::vector<Vertex> V, V2;
    std::vector<GLushort> I;
    std::vector<Point> Origin;
    static const Point ZeroPoint;
};

#endif //OPENGLPLAYGROUND_CATMULLROMCURVE_H
