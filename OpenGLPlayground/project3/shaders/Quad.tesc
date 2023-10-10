struct V2T
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct TC2E
{
    vec3 position;
    vec3 normal;
    vec4 color;
};

//< output patch size is the number of vertices in the output patch
layout (vertices = 4) out;
uniform float tessellationLevelInner;
uniform vec3 tessellationLevelOuter;

in  V2T vdata[];

//< output variables are passed directly to the TES, without any form of interpolation.
//< array size is implicit
out TC2E tcdata[];

patch out vec3 b0;
patch out vec3 b1;
patch out vec3 b2;
patch out vec3 b3;
patch out vec3 b01;
patch out vec3 b10;
patch out vec3 b12;
patch out vec3 b21;
patch out vec3 b23;
patch out vec3 b32;
patch out vec3 b30;
patch out vec3 b03;


void evaluateControlPoints(in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3, in vec3 n0, in vec3 n1, in vec3 n2, in vec3 n3)
{
    //< Calculate Vertex control points
    b0 = p0;
    b1 = p1;
    b2 = p2;
    b3 = p3;

    //< Weight
    float w01 = dot(b1 - b0, n0);
    float w10 = dot(b0 - b1, n1);
    float w12 = dot(b2 - b1, n1);
    float w21 = dot(b1 - b2, n2);
    float w23 = dot(b3 - b2, n2);
    float w32 = dot(b2 - b3, n3);
    float w30 = dot(b0 - b3, n3);
    float w03 = dot(b3 - b0, n0);

    //< Tangent control points
    b01 = (2.*p0 + p1 - w01*n0) / 3.;
    b10 = (2.*p1 + p0 - w10*n1) / 3.;
    b12 = (2.*p1 + p2 - w12*n1) / 3.;
    b21 = (2.*p2 + p1 - w21*n2) / 3.;
    b23 = (2.*p2 + p3 - w23*n2) / 3.;
    b32 = (2.*p3 + p2 - w32*n3) / 3.;
    b30 = (2.*p3 + p0 - w30*n3) / 3.;
    b03 = (2.*p0 + p3 - w03*n0) / 3.;

    //< Center Control point
//    vec3 ee = (b120 + b120 + b021 + b012 + b102 + b210) * 0.25f;
//    vec3 vv = (p1 + p2 + p3) * 0.166666667f;
//    b111 = ee - vv;
}

patch out vec3 n0;
patch out vec3 n1;
patch out vec3 n2;
patch out vec3 n3;
patch out vec3 n01;
patch out vec3 n12;
patch out vec3 n23;
patch out vec3 n30;

void evaluateNormalControlPoints(in vec3 p0, in vec3 p1, in vec3 p2, in vec3 p3, in vec3 n0_, in vec3 n1_, in vec3 n2_, in vec3 n3_)
{
    n0 = n0_;
    n1 = n1_;
    n2 = n2_;
    n3 = n3_;

    float v01 = (2.*(dot(p1 - p0, n0 + n1) / dot(p1 - p0, p1 - p0)));
    float v12 = (2.*(dot(p2 - p1, n1 + n2) / dot(p2 - p1, p2 - p1)));
    float v23 = (2.*(dot(p3 - p2, n2 + n3) / dot(p3 - p2, p3 - p2)));
    float v30 = (2.*(dot(p0 - p3, n3 + n0) / dot(p0 - p3, p0 - p3)));

    n01 = normalize(n0 + n1 - v01*(p1 - p0));
    n12 = normalize(n1 + n2 - v12*(p2 - p1));
    n23 = normalize(n2 + n3 - v23*(p3 - p2));
    n30 = normalize(n3 + n0 - v30*(p0 - p3));
}

void evaluateTesLevel()
{
/*
      - The interpretation of tessellation level depends on the abstract patch type, but the general idea is this:
      - In common cases, each tessellation level defines how many segments an edge is tessellated into( 4 means  one edge ==> 4 edges (2 vertices become 5)).
      - The outer tessellation levels define the tessellation for the outer edges of the patch.
      - The inner tessellation levels are for the number of tessellations within the patch.
      - Triangles only uses 1 inner level and 3 outer levels. The rest are ignored.
      - The patch can be discarded if any outer tessellation level <= 0, it gives chance to TCS to cull patches by passing 0 for a relevant outer tessellation level
      - The effective tessellation levels = function{ value here, spacing policy }
      */
    gl_TessLevelInner[0] = tessellationLevelInner;

    gl_TessLevelOuter[0] = tessellationLevelOuter.x;
    gl_TessLevelOuter[1] = tessellationLevelOuter.y;
    gl_TessLevelOuter[2] = tessellationLevelOuter.z;
    gl_TessLevelOuter[3] = tessellationLevelOuter.z;
    gl_TessLevelInner[1] = tessellationLevelInner;
}



//<
//< Output from Hull Shader(Name in D3D11):
//< 1. Compute tessellate factor( edge tessellation level, inner tessellation level)
//< 2. Transformed "Control Point", like what VS does.
//<
//< patch out float gl_TessLevelOuter[4];
//< patch out float gl_TessLevelInner[2];
//<
//< built-in input variables:
//< gl_PatchVerticesIn : the number of vertices in the input patch.
//< gl_PrimitiveID     : the index of the current patch within this rendering command.
//< gl_InvocationID    : the index of the TCS invocation within this patch. A TCS invocation writes to per-vertex output variables by using this to index them.
//<
//< built-in variables from output of vertex shader
/*
    in gl_PerVertex
    {
      vec4 gl_Position;
      float gl_PointSize;
      float gl_ClipDistance[];
    } gl_in[gl_MaxPatchVertices];
*/
//<
void main()
{
    #define ID gl_InvocationID //< once per vertex(CP)
    tcdata[ID].position = vdata[ID].position;
    tcdata[ID].normal   = vdata[ID].normal;
    tcdata[ID].color    = vdata[ID].color;

    if(ID == 0)
    {
        vec3 p1 = vdata[0].position;
        vec3 p2 = vdata[1].position;
        vec3 p3 = vdata[2].position;
        vec3 p4 = vdata[3].position;

        vec3 n1 = vdata[0].normal;
        vec3 n2 = vdata[1].normal;
        vec3 n3 = vdata[2].normal;
        vec3 n4 = vdata[3].normal;

        evaluateControlPoints(p1, p2, p3, p4, n1, n2, n3, n4);
        evaluateNormalControlPoints(p1, p2, p3,p4, n1, n2, n3, n4);
        evaluateTesLevel();
    }
    #undef ID
}

//< Where is "Patch Constant Function" ?
//< which is called per patch to calculate constant values valid for full patch.