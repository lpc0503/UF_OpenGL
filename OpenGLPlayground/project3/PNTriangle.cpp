//
// Created by ICEJJ on 2023/5/11.
//

#include "PNTriangle.h"

std::vector<glm::vec4> PNTriangle::GenControlPoint(std::vector<Vertex> &vertices, float &u, float &v) {

    auto b300 = vertices[0].pos;
    auto b030 = vertices[1].pos;
    auto b003 = vertices[2].pos;
    INFO("{} {} {}", b003.x, b003.y, b003.z);

    auto P1 = b300;
    auto P2 = b030;
    auto P3 = b003;

    std::vector<std::vector<float>> wij(3, std::vector<float>(3, 0.f));

    for(int i = 0 ; i < vertices.size() ; i++) {

        for(int j = 0 ; j < vertices.size() ; j++) {

            if(i != j) {

                auto jpos = vertices[j].pos;
                auto ipos = vertices[i].pos;
                wij[i][j] = glm::dot(glm::vec3(jpos - ipos), glm::normalize(vertices[j].normal));
            }
        }
    }

    auto b210 = (2.f*P1 + P2 - glm::vec4(wij[0][1]*vertices[0].normal, 1.0f))/3.f;
    auto b120 = (2.f*P2 + P1 - glm::vec4(wij[1][0]*vertices[1].normal, 1.0f))/3.f;
    auto b021 = (2.f*P2 + P3 - glm::vec4(wij[1][2]*vertices[1].normal, 1.0f))/3.f;
    auto b012 = (2.f*P3 + P2 - glm::vec4(wij[2][1]*vertices[2].normal, 1.0f))/3.f;
    auto b102 = (2.f*P1 + P3 - glm::vec4(wij[0][2]*vertices[2].normal, 1.0f))/3.f;
    auto b201 = (2.f*P3 + P1 - glm::vec4(wij[2][0]*vertices[0].normal, 1.0f))/3.f;

    auto E = (b210 + b120 + b021 + b012 + b201 + b102) / 6.f;
    auto V = (b300 + b030 + b003) / 3.f;

    auto b111 = E + (E-V) / 2.f;

    float w = 1-u-v;

    float u3 = u * u * u;
    float v3 = v * v * v;
    float w3 = w * w * w;
    float u2 = u * u;
    float v2 = v * v;
    float w2 = w * w;

    glm::vec4 pos = b300 * w3 + b030 * u3 + b003 * v3
               + b210 * 3.f * w2 * u + b120 * 3.f * w * u2 + b201 * 3.f * w2 * v
               + b021 * 3.f * u2 * v + b102 * 3.f * w * v2 + b012 * 3.f * u * v2
               + b012 * 6.f * w * u * v;

    INFO("{} {} {}", pos.x, pos.y, pos.z);
    Vertex add;
    add.pos = pos;
    vertices.push_back(add);

    std::vector<glm::vec4> controlPoint(10);

    controlPoint[0] = b300;
    controlPoint[1] = b030;
    controlPoint[2] = b003;
    controlPoint[3] = b210;
    controlPoint[4] = b021;
    controlPoint[5] = b201;
    controlPoint[6] = b120;
    controlPoint[7] = b012;
    controlPoint[8] = b102;
    controlPoint[9] = b111;

    return controlPoint;
}
