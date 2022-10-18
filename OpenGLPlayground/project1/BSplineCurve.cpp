#include "BSplineCurve.h"

BSplineCurve::BSplineCurve(Vertex P[], size_t size) {

    SetVertices(P, size);
}

void BSplineCurve::SetVertices(Vertex p[], size_t size){

    B.resize(size);
    for(int i = 0 ; i < size ; i++) {

        B[i] = Point(p[i].Position);
    }
    Origin = B;
}

void BSplineCurve::Recover() {

    int prevK = K;
    for(int i = 0; i < prevK; i++)
        SubDivide();
    K = prevK;
}

void BSplineCurve::SubDivide() {

    K++;

    A.clear();
    for(int i = 0 ; i < B.size() ; i++) {

        BSplineSubDividePoints(i);
    }
    B = A;

    I.resize(A.size());

    for(int i = 0 ; i < A.size(); i++) {

        I[i] = i;
    }
    ToVertex();
}

void BSplineCurve::BSplineSubDividePoints(int i) {

    Point a;
    Point b;
    Point Pi_m1 = (i-1 >= 0) ? B[i-1] : B[B.size()-1];
    Point Pi = B[i];
    Point Pi_p1 = i+1 < B.size() ? B[i+1] : B[0];
    a = (Pi_m1 * 4.f + Pi * 4.f) / 8.f;
    b = (Pi_m1 + Pi*6.f + Pi_p1) / 8.f;

    A.push_back(a);
    A.push_back(b);
}

void BSplineCurve::ToVertex() {

    V.clear();

    for(auto &i : A) {

        V.emplace_back(i);
    }
}

void BSplineCurve::Clear() {

    K = 0;
    A.clear();
    B = Origin;
    I.clear();
    V.clear();
}

