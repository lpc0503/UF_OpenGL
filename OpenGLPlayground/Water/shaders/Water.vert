#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec4 vertexPosition_modelspace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV;

// Output data; will be interpolated for each fragment.
out vec4 vs_vertexColor;
out vec3 vertexPosition_worldspace;
out vec3 normal_worldspace;

// Values that stay constant for the whole mesh.
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform float vertexPointSize;

void main()
{
	gl_PointSize = vertexPointSize;

	vec4 worldPosition = M * vertexPosition_modelspace;
	vertexPosition_worldspace = worldPosition.xyz;

	gl_Position =  P * V * worldPosition;

	normal_worldspace = normalize(mat3(transpose(inverse(M))) * vertexNormal);
}

