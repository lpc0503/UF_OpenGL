#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

// TL
// ATTN: Refer to https://learnopengl.com/Lighting/Colors and https://learnopengl.com/Lighting/Basic-Lighting
// to familiarize yourself with implementing basic lighting model in OpenGL shaders

void main() {
	color = vs_vertexColor.rgb;
}