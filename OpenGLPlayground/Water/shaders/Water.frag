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

void main()
{
    vec3 normal = normalize(normal_worldspace);

    // vec3 lightDir = normalize(uLightPos - vertexPosition_worldspace);
    vec3 lightDir = normalize(uLightPos);
    vec3 viewDir = normalize(uCameraPos - vertexPosition_worldspace);

    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = vec3(0.1);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uLightColor * diff;

    float spec = pow(max(dot(normal, halfwayDir), 0.0), 50.0);
    vec3 specular = uLightColor * spec;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
}