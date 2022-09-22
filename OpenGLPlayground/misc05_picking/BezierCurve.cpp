#include "BezierCurve.h"

#include <iostream>

const Point BezierCurve::ZeroPoint = Point(0.f, 0.f, 0.f);

BezierCurve::BezierCurve(Vertex *p, size_t size)
{
    SetVertices(p, size);
    Calc();
    ToVertex();
}

void BezierCurve::Clear()
{
    A.clear();
    B = Origin;
    I.clear();
    V.clear();
}

void BezierCurve::SetVertices(Vertex *p, size_t size)
{
    B.resize(size);
    for(int i = 0 ; i < size ; i++)
    {
        B[i] = Point(p[i].Position);
    }
    Origin = B;
}

void BezierCurve::ToVertex()
{
    V.clear();
    for(const auto &i : A)
        V.emplace_back(i);
    for(int i = 0; i < V.size(); i++)
        I.push_back(i);
}

void BezierCurve::Calc()
{
    int len = B.size() * 3;
    A.resize(len);
    A[0] = B[0];

    int a = 0, b = 1;
    int i = 1, cnt = 0;

    while(i < len)
    {
        if(i % 3 == 0)
        {
            a = b;
            b++;
            i++;
            continue;
        }

        // odd
        if(i % 2 != 0)
        {
            A[i] = (B[a] * 2.f + B[b]) / 3.f;
        }
        // even
        else
        {
            A[i] = (B[a] + B[b] * 2.f) / 3.f;
        }

        i++;
    }

    // TODO: BUGGGGGG
    // calc for 3
    for(int j = 3; j < len; j += 3)
    {
        A[j] = (A[j-1] + A[j+1]) / 2.f;
    }
}

//3
//A[4] = (B[1] * 2.f + B[2]) / 3.f;
//A[5] = (B[1] + B[2] * 2.f) / 3.f;
//6
//A[7] = (B[2] * 2.f + B[3]) / 3.f;
//A[8] = (B[2] + B[3] * 2.f) / 3.f;
