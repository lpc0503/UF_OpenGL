#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec2 UV;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

uniform sampler2D tex;

uniform bool uUseTex;

void main() {
//	if(uUseTex)
//	{
//		color = texture(tex, UV).rgb;
//	}
//	else
	{
		color = vs_vertexColor.rgb;
		// color = vec3(UV, 0.0);
	}
}