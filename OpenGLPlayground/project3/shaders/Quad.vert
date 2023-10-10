// Input Attributes to Vertex shader
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in vec2 vertexUV;

//layout(location = 0) in vec4 Quad_Position_VS_in;
//layout(location = 1) in vec4 Quad_Color_VS_in;
//layout(location = 2) in vec3 Quad_Normal_VS_in;
//layout(location = 3) in vec2 Quad_UV_VS_in;

// Output Attributes into Tessellation Control Shader
out vec3 Quad_Position_CS_in;
out vec4 Quad_Color_CS_in;
out vec3 Quad_Normal_CS_in;
// out vec2 Quad_UV_CS_in;

void main()
{
    Quad_Position_CS_in = vertexPosition_modelspace.xyz;
    Quad_Color_CS_in = vertexColor;
    Quad_Normal_CS_in = normalize(vertexNormal_modelspace);
//    Quad_UV_CS_in = vertexUV;
}