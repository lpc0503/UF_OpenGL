//
// Created by ICEJJ on 2023/5/11.
//

#include "PNTriangle.h"

void PNTriangle::GenControlPoint(std::vector<Vertex> &vertices, float &u, float &v) {

    auto b300 = vertices[0].pos;
    auto b030 = vertices[1].pos;
    auto b003 = vertices[2].pos;
    INFO("{} {} {}", b003.x, b003.y, b003.z);

    std::vector<std::vector<float>> wij(3, std::vector<float>(3, 0.f));

    for(int i = 0 ; i < vertices.size() ; i++) {

        for(int j = 0 ; j < vertices.size() ; j++) {

            if(i != j) {

                auto jpos = vertices[j].pos;
                auto ipos = vertices[i].pos;
                wij[i][j] = glm::dot(glm::vec3(jpos - ipos), vertices[j].normal);
            }
        }
    }

    auto b210 = (2.f*b300 + b030 - glm::vec4(wij[0][1]*vertices[0].normal, 0.0f))/3.f;
    auto b021 = (2.f*b030 + b003 - glm::vec4(wij[1][2]*vertices[1].normal, 0.0f))/3.f;
    auto b201 = (2.f*b003 + b300 - glm::vec4(wij[2][0]*vertices[2].normal, 0.0f))/3.f;
    auto b120 = (2.f*b030 + b300 - glm::vec4(wij[1][0]*vertices[1].normal, 0.0f))/3.f;
    auto b012 = (2.f*b003 + b030 - glm::vec4(wij[2][1]*vertices[2].normal, 0.0f))/3.f;
    auto b102 = (2.f*b300 + b003 - glm::vec4(wij[0][2]*vertices[0].normal, 0.0f))/3.f;

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

//    vertices.clear();
//    Vertex tmp1, tmp2, tmp3;
//    tmp1.pos = b300;
//    tmp2.pos = b210;
//    tmp3.pos = b201;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b201;
//    tmp2.pos = b210;
//    tmp3.pos = b111;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b111;
//    tmp2.pos = b210;
//    tmp3.pos = b120;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b120;
//    tmp2.pos = b030;
//    tmp3.pos = b021;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b021;
//    tmp2.pos = b111;
//    tmp3.pos = b120;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b201;
//    tmp2.pos = b111;
//    tmp3.pos = b102;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b102;
//    tmp2.pos = b111;
//    tmp3.pos = b012;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b012;
//    tmp2.pos = b003;
//    tmp3.pos = b102;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
//    tmp1.pos = b111;
//    tmp2.pos = b021;
//    tmp3.pos = b012;
//    vertices.push_back(tmp1);
//    vertices.push_back(tmp2);
//    vertices.push_back(tmp3);
}
