#version 330 core

// Interpolated values from the vertex shaders
in vec4 vs_vertexColor;
in vec3 vertexPosition_worldspace;
in vec3 normal_worldspace;

// Ouput data
out vec4 FragColor;

uniform vec3 uLightPos;         // World-space position of the light
uniform vec3 uLightColor;       // Light color
uniform vec3 uCameraPos;        // World-space position of the camera

uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;

#define PI 3.14159265358979323846

void main()
{
    vec3 normal = normalize(normal_worldspace);

    // vec3 lightDir = normalize(uLightPos - vertexPosition_worldspace);
    vec3 lightDir = uLightPos;
    vec3 viewDir = normalize(uCameraPos - vertexPosition_worldspace);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float ndotl = max(dot(lightDir, normal), 0.0);

    vec3 ambient = uAmbientColor;

    vec3 diffuse_reflectance = uDiffuseColor / PI; // 為啥要除 PI?
    vec3 diffuse = uLightColor * ndotl * diffuse_reflectance;

    vec3 specularReflectance = uSpecularColor;
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 50.0);
    vec3 specular = uLightColor * ndotl * specularReflectance * spec;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}