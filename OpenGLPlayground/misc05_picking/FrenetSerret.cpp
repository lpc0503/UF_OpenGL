#include "FrenetSerret.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

FrenetSerret::FrenetSerret(std::vector<Point> curve, size_t size) {

    index = 10;
    curveSize = size;
    SetVertices(curve, size);
    Calc();
}

void FrenetSerret::Clear() {

    T_.clear();
    B_.clear();
    N_.clear();
    Tv.clear();
    Bv.clear();
    Nv.clear();
}

void FrenetSerret::ToVertex() {

    for(const auto &i:T_)
        Tv.emplace_back(Vertex(i, {1.f, .0f, 0.f, 1.f}));

    for(int i = 0; i < Tv.size(); i++)
        TI.push_back(i);

    for(const auto &i:B_)
        Bv.emplace_back(Vertex(i, {0.f, 0.f, 1.f, 1.f}));

    for(int i = 0; i < Tv.size(); i++)
        BI.push_back(i);

    for(const auto &i:N_)
        Nv.emplace_back(Vertex(i, {0.f, 1.f, 0.f, 1.f}));


    for(int i = 0; i < Tv.size(); i++)
        NI.push_back(i);
}

void FrenetSerret::SetVertices(const std::vector<Point>& curve, size_t size) {

    curve_ = curve;
    curveSize = size;
}

void FrenetSerret::Calc() {

    Clear();
    if(index >= curveSize)
        index = 0;

    auto current = curve_[index];
    auto next = index >= curveSize ? curve_[0] : curve_[index + 1];

    auto t = next - current;
    auto b = next + current;
    auto point = glm::vec3(t.x, t.y, t.z);
    auto point2 = glm::vec3(b.x, b.y, b.z);

    glm::vec3 T = glm::normalize(point);
    glm::vec3 B = glm::normalize(glm::cross(T, point2));
    glm::vec3 N = glm::normalize(-glm::cross(B, T));

    T_.push_back(current);
    T_.emplace_back((current.x + T.x/2), (current.y + T.y/2), (current.z + T.z/2));
    B_.push_back(current);
    B_.emplace_back(current.x + B.x/2, current.y + B.y/2, current.z + B.z/2);
    N_.push_back(current);
    N_.emplace_back(current.x + N.x/2, current.y + N.y/2, current.z + N.z/2);

    ToVertex();
}
