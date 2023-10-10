struct QuadCP_FragData
{
    vec3 pos;
    vec3 normal;
    vec4 color;
};

struct DirectionalLight
{
    vec3 dir;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in QuadCP_FragData fragdata;
in vec3 position_worldspace;
in vec3 normal_cameraspace;
in vec3 eyeDirection_cameraspace;
in vec3 lightDirection_cameraspace;

out vec3 color;

// Uniform attributes
uniform vec3 lightPosition_worldspace;
uniform DirectionalLight dirLight;

void main() {
    vec3 lightColor = vec3(1,1,0.5);
    float lightPower = 600.f;

    vec3 materialDiffuseColor = fragdata.color.rgb;
    vec3 materialAmbientColor = vec3(0.3, 0.3, 0.3) * materialDiffuseColor;
    vec3 materialSpecularColor = vec3(0.5, 0.5, 0.5)* materialDiffuseColor;

    float distance = length(dirLight.dir - fragdata.pos);

    vec3 n = normalize(fragdata.normal);
    vec3 l = normalize(lightDirection_cameraspace - fragdata.pos);

    float cosTheta = clamp(dot(n, l), 0.f, 1.f);

    vec3 e = normalize(eyeDirection_cameraspace - fragdata.pos);
    vec3 r = reflect(-l, n);

    float cosAlpha = clamp(dot(e,r), 0.f, 1.f);

    color = fragdata.color.rgb;
}