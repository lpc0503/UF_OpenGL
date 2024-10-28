struct T2F
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

//in vec3 position_worldspace;
in vec3 normal_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;
in T2F tedata;
out vec3 color;

struct DirectionalLight
{
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionalLight dirLight;

uniform vec3 lightPosition_worldspace;

void main()
{
    vec3 lightColor = vec3(1, 0.8, 0.5);
    float lightPower = 30.f;

    vec3 materialDiffuseColor = tedata.color.rgb;
    vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(0.5, 0.5, 0.5) * materialDiffuseColor;

    float distance = length(dirLight.dir - tedata.position);

    vec3 n = normalize(tedata.normal);
    vec3 l = normalize(lightDirection_cameraspace - tedata.position);

    float cosTheta = clamp(dot(n, l), 0.f, 1.f);

    vec3 e = normalize(eyeDirection_cameraspace - tedata.position);
    vec3 r = reflect(-l, n);

    float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

//    color = tedata.color.rgb;
    color = materialAmbientColor
    + materialDiffuseColor * lightColor * lightPower * cosTheta / (distance * distance)
    + materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 5.f) / (distance * distance);
}