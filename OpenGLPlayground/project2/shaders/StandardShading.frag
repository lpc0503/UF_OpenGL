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

struct DirectionalLight
{
	vec3 dir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight dirLight;

uniform bool uEnableLight;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
	vec3 lightDir = normalize(-light.dir);
	//
	float diffuse = max(dot(lightDir, normal), 0.0);
	vec3 diffuseColor = light.diffuse * diffuse;
	//
//	float specular = pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), 32.0);
//	vec3 specularColor = light.specular * specular;
	//
	return light.ambient + diffuseColor/* + specularColor*/;
}

void main()
{
	if(uEnableLight)
	{
		color = vs_vertexColor.rgb * CalcDirectionalLight(dirLight, Normal_cameraspace);
	}
	else
	{
		color = vs_vertexColor.rgb;
	}
}