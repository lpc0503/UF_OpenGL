#pragma once
#ifndef OPENGLPLAYGROUND_FRENETSERRET_H
#define OPENGLPLAYGROUND_FRENETSERRET_H

#include <vector>
#include <glad/glad.h>
#include "Geometry.h"

class FrenetSerret {

public:

    FrenetSerret() = default;
    FrenetSerret(std::vector<Point> curve, size_t size);

public:
    void Clear();
    void ToVertex();

public:

    void SetVertices(const std::vector<Point>& curve, size_t size);
    void Calc();

    std::vector<Vertex> Tv;
    std::vector<Vertex> Bv;
    std::vector<Vertex> Nv;
    std::vector<Vertex> Vv;

    std::vector<Point> T_;
    std::vector<Point> B_;
    std::vector<Point> N_;

    std::vector<GLushort> TI;
    std::vector<GLushort> BI;
    std::vector<GLushort> NI;
    std::vector<GLushort> VI;

    std::vector<Point> curve_;

    int index;
    size_t curveSize;
};


#endif //OPENGLPLAYGROUND_FRENETSERRET_H
