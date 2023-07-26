#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vs_vertexColor_v[];
in vec3 Position_worldspace_v[];
in vec3 Normal_cameraspace_v[];
in vec3 EyeDirection_cameraspace_v[];
in vec3 LightDirection_cameraspace_v[];
in vec2 UV_v[];

out vec4 vs_vertexColor;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec2 UV;

void main()
{

    for(int i = 0 ; i < 3; i++) {

        vs_vertexColor = vs_vertexColor_v[i];
        Position_worldspace = Position_worldspace_v[i];
        Normal_cameraspace = Normal_cameraspace_v[i];
        EyeDirection_cameraspace = EyeDirection_cameraspace_v[i];
        LightDirection_cameraspace = LightDirection_cameraspace_v[i];
        UV = UV_v[i];
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    EndPrimitive();
//
//    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
//    EmitVertex();
//
//    EndPrimitive();
}