struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct T2F
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TesOut
{
    vec3 position_world;
    vec3 normal_world;
    vec3 diffuse_color;
};

//< specify domain, spacing policy for Tessellator
/*
    ## input layout qualifiers
    - specify options that control the particular form of tessellation
    - equal_spacing            : clamp to [1, max], rounded up to the nearest integer
    - fractional_even_spacing  : clamp to [2, max], rounded up to the nearest even integer
    - fractional_odd_spacing   : clamp to [1, max - 1], rounded up to the nearest odd integer

    - If all of the effective outer levels (as computed above) levels are exactly 1.0, and the effective inner level is also 1.0,
      then nothing is tessellated, and the TES will get 3 vertices and one triangle.
*/
layout(quads, equal_spacing, ccw) in;

//<per-vertex inputs from the TCS are arrays indexed by index implicitly
in TC2E tcdata[];

//< per-patch outputs from the TCS as inputs in the TES using the patch keyword
//patch in vec4 data;

out TesOut tes_out;

out T2F tedata;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
vec3 lightPosition_worldspace;

patch in vec3 b0;
patch in vec3 b1;
patch in vec3 b2;
patch in vec3 b3;
patch in vec3 b01;
patch in vec3 b10;
patch in vec3 b12;
patch in vec3 b21;
patch in vec3 b23;
patch in vec3 b32;
patch in vec3 b30;
patch in vec3 b03;
patch in vec3 n0;
patch in vec3 n1;
patch in vec3 n2;
patch in vec3 n3;
patch in vec3 n01;
patch in vec3 n12;
patch in vec3 n23;
patch in vec3 n30;

patch in vec4 c0;
patch in vec4 c1;
patch in vec4 c2;
patch in vec4 c3;


vec3 evaluatePosistion(in vec3 tessCoord)
{
    float u =tessCoord.x;
    float v =tessCoord.y;

    vec3 q = b01 + b10 + b12 + b21 + b23 + b32 + b30 + b03;

    vec3 e0 = (2.*(b01 + b03 + q) - (b21 + b23)) / 18.;
    vec3 v0 = (4.*b0 + 2.*(b3 + b1) + b2) / 9.;
    vec3 b02 = e0 + (e0 - v0) / 2.;

    vec3 e1 = (2.*(b12 + b10 + q) - (b32 + b30)) / 18.;
    vec3 v1 = (4.*b1 + 2.*(b0 + b2) + b3) / 9.;
    vec3 b13 = e1 + (e1 - v1) / 2.;

    vec3 e2 = (2.*(b23 + b21 + q) - (b03 + b01)) / 18.;
    vec3 v2 = (4.*b2 + 2.*(b1 + b3) + b0) / 9.;
    vec3 b20 = e2 + (e2 - v2) / 2.;

    vec3 e3 = (2.*(b30 + b32 + q) - (b10 + b12)) / 18.;
    vec3 v3 = (4.*b3 + 2.*(b2 + b0) + b1) / 9.;
    vec3 b31 = e3 + (e3 - v3) / 2.;

    float bu0 = (1.-u) * (1.-u) * (1.-u);
    float bu1 = 3. * u * (1.-u) * (1.-u);
    float bu2 = 3. * u * u * (1.-u);
    float bu3 = u * u * u;

    float bv0 = (1.-v) * (1.-v) * (1.-v);
    float bv1 = 3. * v * (1.-v) * (1.-v);
    float bv2 = 3. * v * v * (1.-v);
    float bv3 = v * v * v;

    vec3 pos = bu0*(bv0*b0 + bv1*b01 + bv2*b10 + bv3*b1)
    + bu1*(bv0*b03 + bv1*b02 + bv2*b13 + bv3*b12)
    + bu2*(bv0*b30 + bv1*b31 + bv2*b20 + bv3*b21)
    + bu3*(bv0*b3 + bv1*b32 + bv2*b23 + bv3*b2);

    return pos;
}

vec3 evaluateNormal(in vec3 tessCoord)
{
    float u =tessCoord.x;
    float v =tessCoord.y;

    vec3 n0123 = ((2.*(n01 + n12 + n23 + n30)) + (n0 + n1 + n2 + n3)) / 12.;

    float nu0 = (1.-u) * (1.-u);
    float nu1 = 2. * u * (1.-u);
    float nu2 = u * u;

    float nv0 = (1.-v) * (1.-v);
    float nv1 = 2. * v * (1.-v);
    float nv2 = v * v;

    vec3 normal = nu0*(nv0*n0 + nv1*n01 + nv2*n1)
    + nu1*(nv0*n30 + nv1*n0123 + nv2*n12)
    + nu2*(nv0*n3 + nv1*n23 + nv2*n2);

    return normal;
}

vec4 evaluateColor(in vec3 tessCoord) {

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    return vec4(c0 * w + c1 * u + c2 * v + c3);
}

//<
//< evaluate surface for given UV coordinate
//< interpolate attributes of vertex
//< maybe displacement here
//<
/*
    - reference : https://www.khronos.org/opengl/wiki/Tessellation_Evaluation_Shader
    - The inputs for the TES are : per-vertex & per-patch
    - built-in variables
      - in vec3 gl_TessCoord      : Barycentric coordinates of generated primitive
      - in int gl_PatchVerticesIn : the vertex count for the patch being processed
      - in int gl_PrimitiveID     : the index of the current patch in the series of patches being processed for this draw call

      - buit-in input
          in gl_PerVertex
            {
              vec4 gl_Position;
              float gl_PointSize;
              float gl_ClipDistance[];
            } gl_in[gl_MaxPatchVertices];

      - buit-in output
          out gl_PerVertex
          {
          vec4 gl_Position;   //< the clip-space output position of the current vertex.
          float gl_PointSize; //< the pixel width/height of the point being rasterized. valid for point primitives
          float gl_ClipDistance[]; //< allows shader to set the distance from the vertex to each User-Defined Clip Plane
          };
*/
void main()
{
    lightPosition_worldspace = vec3(5.f, 0.f, 0.f);

    //    pos = b300 * w3 + b030 * u3 + b003 * v3;
    vec3 pos = evaluatePosistion(gl_TessCoord);
    //    tedata.position = pos;

    vec3 normal = evaluateNormal(gl_TessCoord);
    tedata.position = vec3((M * vec4(pos, 1.0)).xyz);
    tedata.normal = normal;

    tedata.color = evaluateColor(gl_TessCoord);

    gl_Position = P * V * M * vec4(pos, 1.0);

    vec3 vertexPosition_cameraspace = (V * M * vec4(pos, 0.5)).xyz;
    eyeDirection_cameraspace = vec3(0.0f, 0.0f, 0.0f) - vertexPosition_cameraspace;

    vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1.0)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;

    normal_cameraspace = vec3((V * M * vec4(tedata.normal, 0)).xyz);
}