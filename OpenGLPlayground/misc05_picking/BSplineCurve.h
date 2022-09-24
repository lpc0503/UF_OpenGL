#pragma once
#ifndef OPENGLPLAYGROUND_BSPLINECURVE_H
#define OPENGLPLAYGROUND_BSPLINECURVE_H

#include <vector>
#include <glad/glad.h>
#include "Geometry.h"

class BSplineCurve {

public:

    BSplineCurve() = default;
    BSplineCurve(Vertex P[], size_t size);

public:

    void Recover();
    void SubDivide();
    void Clear();
    void BSplineSubDividePoints(int i);
    void ToVertex();

public:

    void SetVertices(Vertex p[], size_t size);



    std::vector<Point> A, B; // K+1, K
    std::vector<Vertex> V;
    std::vector<GLushort> I;
    std::vector<Point> Origin;
    int K = 0;
};


#endif //OPENGLPLAYGROUND_BSPLINECURVE_H
