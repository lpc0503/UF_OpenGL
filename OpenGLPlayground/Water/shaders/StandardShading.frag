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
//out vec4 FragColor;

// Values that stay constant for the whole mesh.
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

uniform sampler2D tex;

uniform bool uUseTex;

struct DirectionalLight
{
	vec3 dir;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight dirLight;

uniform bool uEnableLight;

//uniform vec4 Color;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal)
{
	vec3 lightDir = (MV * vec4(-light.dir, 0.0)).xyz;
	lightDir = normalize(lightDir);
	//	vec3 lightDir = normalize(-light.dir);
	//	lightDir = (MV * vec4(lightDir, 0.0)).xyz;
	//
	float diffuse = max(dot(lightDir, normal), 0.0);
	vec3 diffuseColor = light.diffuse * diffuse;
	//
	//	float specular = pow(max(dot(reflect(-lightDir, normal), viewDir), 0.0), 32.0);
	//	vec3 specularColor = light.specular * specular;
	//
	return light.ambient * 0.1 + diffuseColor * 0.1;
}

void main() {
//	if(uUseTex)
//	{
//		color = texture(tex, UV).rgb;
//	}
//	else
//	{
//		color = vs_vertexColor.rgb;
//		// color = vec3(UV, 0.0);
//	}

	vec3 lightColor = vec3(1,1,0.5);
	float lightPower = 600.f;

	vec3 materialDiffuseColor = vec3(vs_vertexColor);
	vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(0.5, 0.5, 0.5) * materialDiffuseColor;

	float distance = length(dirLight.dir - Position_worldspace);

	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);

	float cosTheta = clamp(dot(n, l), 0.f, 1.f);

	vec3 e = normalize(EyeDirection_cameraspace);
	vec3 r = reflect(-l, n);

	float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

	if(uEnableLight)
	{
//		FragColor = vec4(texture(tex, UV));
//		color = vs_vertexColor.xyz;
		color = materialAmbientColor
		+ materialDiffuseColor * lightColor * lightPower * cosTheta / (distance * distance)
		+ materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5.f) / (distance * distance);
	}
	else
	{
		color = vs_vertexColor.xyz;
	}
}