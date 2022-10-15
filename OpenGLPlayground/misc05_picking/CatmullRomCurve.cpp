#include "CatmullRomCurve.h"

CatmullRomCurve::CatmullRomCurve(Vertex *p, size_t size) {

    SetVertices(p, size);
    Calc();
    ToVertex();
}

void CatmullRomCurve::Clear() {

    A.clear();
    B = Origin;
    I.clear();
    V.clear();
    C.clear();
}

void CatmullRomCurve::ToVertex() {

    V.clear();
    for(const auto &i : A)
        V.emplace_back(Vertex(i, {1.0f, 0.0f, 0.0f, 1.f}));

    for(int i = 0; i < V.size(); i++)
        I.push_back(i);

    V2.clear();
    for(const auto &i : C)
        V2.emplace_back(Vertex(i, {0.0f, 0.0f, 1.0f, 1.f}));
}

void CatmullRomCurve::SetVertices(Vertex *p, size_t size) {

    B.resize(size);
    for(int i = 0 ; i < size ; i++){

        B[i] = Point(p[i].Position);
    }
    Origin = B;
}

void CatmullRomCurve::Calc() {

    // Generate bezier points
    A.resize(B.size() * 3);
    for(int i = 0 ; i < B.size() ; i++){

        for(int j = 0 ; j < 3 ; j++){

            if(j == 0){

                A[i*3 + j] = B[i];
            }
            else if(j == 1){

                A[i*3 + j] = B[i] + (B[(i+1)% B.size()]-B[i-1<0?B.size()-1:i-1]) / 6.f;
            }
            else if(j == 2){

                A[i*3 + j] = B[(i+1)% B.size()] + (B[i]-B[(i+2) % B.size()]) / 6.f;
            }
        }
    }

    // Generate curve points (deCasteljau's Algorithm)
    // ref: https://blog.csdn.net/Fioman/article/details/2578895
    int n = 3;
    for(int k = 0 ; k < A.size() ; k += n) {

        for (float u = 0.f ; u <= 1.f ; u += step / n) {

            std::vector<Point> T = A;
            for (int i = 0 ; i < n ; i++) {

                for (int j = k ; j < (n - i) + k ; j++) {

                    T[j] = T[j] * (1 - u) + T[(j + 1) % T.size()] * u;
                }
            }
            C.push_back(T[k]);
        }
    }
}
