#version 330 core

struct V2T
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec2 vertexUV;

out V2T vdata;

void main()
{
    vdata.position = vec3((vertexPosition_modelspace).xyz);
    vdata.normal = vertexNormal_modelspace;
//    vdata.color = vec4(vertexNormal_modelspace, 1.0);
    if(vertexNormal_modelspace == vec3(0.0, 0.0, 0.0))
        vdata.color = vec4((0.0, 0.0, 0.0, 1.0));
    else
        vdata.color = vertexColor;
}