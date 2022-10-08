#include "BezierCurve.h"

#include <iostream>

const Point BezierCurve::ZeroPoint = Point(0.f, 0.f, 0.f);

BezierCurve::BezierCurve(Vertex *p, size_t size){

    SetVertices(p, size);
    Calc();
    ToVertex();
}

void BezierCurve::Clear(){

    A.clear();
    B = Origin;
    I.clear();
    V.clear();
}

void BezierCurve::SetVertices(Vertex *p, size_t size){

    B.resize(size);
    for(int i = 0 ; i < size ; i++){

        B[i] = Point(p[i].Position);
    }
    Origin = B;
}

void BezierCurve::ToVertex(){

    V.clear();
    for(const auto &i : A)
        V.emplace_back(Vertex(i, {1.0f, 1.0f, 0.f, 1.f}));
    for(int i = 0; i < V.size(); i++)
        I.push_back(i);
}

void BezierCurve::Calc(){

    int len = B.size() * 3;
    A.resize(len);
    A[0] = B[0];

    for(int i = 0 ; i < B.size() ; i++) {

        // Ci_j where j = 0, 1, 2, 3
        // Ci_0 = Ci-1_3
        // Ci_0 = (Ci-1_3 + Ci_1) / 2
        // We will calculate Ci_0 later
        for(int j = 1 ; j < 4 ; j++) {

            if(j == 3) continue; // calculate Ci_0 later

            // Only calculate Ci_1 and Ci_2
            if(j % 2 != 0) {

                A[i*3+j] = (B[i] * 2.f + B[i+1 < B.size() ? i+1 : 0]) / 3.f;
            }
            // even
            else {

                A[i*3+j] = (B[i] + B[i+1 < B.size() ? i+1 : 0] * 2.f) / 3.f;
            }
        }
    }

    // calc for 3
    for(int j = 3; j < len; j += 3) {

        A[j] = (A[j-1] + A[j+1]) / 2.f;
    }
}
