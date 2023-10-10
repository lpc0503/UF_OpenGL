// 4 Vertices in a PN_quad patch
layout (vertices = 4) out;

// Uniform attributes
uniform float tessellationLevelInner;
uniform vec3 tessellationLevelOuter;

// attributes of the input control points
in vec3 Quad_Position_CS_in[];
in vec4 Quad_Color_CS_in[];
in vec3 Quad_Normal_CS_in[];
// in vec2 Quad_UV_CS_in[];

struct Output_QuadPatch
{
    // Four input control point
    vec3 CP_Pos_b0;
    vec3 CP_Pos_b1;
    vec3 CP_Pos_b2;
    vec3 CP_Pos_b3;
//    vec4 CP_Color_b0;
//    vec4 CP_Color_b1;
//    vec4 CP_Color_b2;
//    vec4 CP_Color_b3;

    // Boundary control points
    vec3 CP_Pos_b01;
    vec3 CP_Pos_b10;
    vec3 CP_Pos_b12;
    vec3 CP_Pos_b21;
    vec3 CP_Pos_b23;
    vec3 CP_Pos_b32;
    vec3 CP_Pos_b03;
    vec3 CP_Pos_b30;
//    vec4 CP_Color_b01;
//    vec4 CP_Color_b10;
//    vec4 CP_Color_b12;
//    vec4 CP_Color_b21;
//    vec4 CP_Color_b23;
//    vec4 CP_Color_b32;
//    vec4 CP_Color_b03;
//    vec4 CP_Color_b30;


    // Inner control points
    vec3 CP_Pos_b02;
    vec3 CP_Pos_b20;
    vec3 CP_Pos_b13;
    vec3 CP_Pos_b31;
//    vec3 CP_Color_b02;
//    vec3 CP_Color_b20;
//    vec3 CP_Color_b13;
//    vec3 CP_Color_b31;

    // Normals
    vec3 Normal_N0;
    vec3 Normal_N1;
    vec3 Normal_N2;
    vec3 Normal_N3;

    vec3 Normal_N01;
    vec3 Normal_N12;
    vec3 Normal_N23;
    vec3 Normal_N03;

    vec3 Normal_N0123;
};

// attributes of the output control points
patch out Output_QuadPatch oPatch;

vec3 CalcBoundaryPosition(vec3 P0, vec3 P1, vec3 N0)
{
    return (2.0 * P0 + P1 - (dot(P0 - P1, N0) * N0))/3.0;
}

vec3 CalcBoundaryNormal(vec3 P0, vec3 P1, vec3 N0, vec3 N1)
{
    float V01 = 2.0 * dot(P1 - P0, N0 + N1) / dot(P1 - P0, P1 - P0);
    return normalize(N0 + N1 - V01 * (P1 - P0));
}

void CalcInterpolation()
{
    // Init
    oPatch.CP_Pos_b0 = Quad_Position_CS_in[0];
    oPatch.Normal_N0 = Quad_Normal_CS_in[0];

    oPatch.CP_Pos_b1 = Quad_Position_CS_in[1];
    oPatch.Normal_N1 = Quad_Normal_CS_in[1];

    oPatch.CP_Pos_b2 = Quad_Position_CS_in[2];
    oPatch.Normal_N2 = Quad_Normal_CS_in[2];

    oPatch.CP_Pos_b3 = Quad_Position_CS_in[3];
    oPatch.Normal_N3 = Quad_Normal_CS_in[3];

//    oPatch.CP_Color_b0 = Quad_Color_CS_in[0];
//    oPatch.CP_Color_b1 = Quad_Color_CS_in[1];
//    oPatch.CP_Color_b2 = Quad_Color_CS_in[2];
//    oPatch.CP_Color_b3 = Quad_Color_CS_in[3];


    // Edge Control Points
    oPatch.CP_Pos_b01 = CalcBoundaryPosition(Quad_Position_CS_in[0], Quad_Position_CS_in[1], Quad_Normal_CS_in[0]);
    oPatch.CP_Pos_b10 = CalcBoundaryPosition(Quad_Position_CS_in[1], Quad_Position_CS_in[0], Quad_Normal_CS_in[1]);

    oPatch.CP_Pos_b12 = CalcBoundaryPosition(Quad_Position_CS_in[1], Quad_Position_CS_in[2], Quad_Normal_CS_in[1]);
    oPatch.CP_Pos_b21 = CalcBoundaryPosition(Quad_Position_CS_in[2], Quad_Position_CS_in[1], Quad_Normal_CS_in[2]);

    oPatch.CP_Pos_b23 = CalcBoundaryPosition(Quad_Position_CS_in[2], Quad_Position_CS_in[3], Quad_Normal_CS_in[2]);
    oPatch.CP_Pos_b32 = CalcBoundaryPosition(Quad_Position_CS_in[3], Quad_Position_CS_in[2], Quad_Normal_CS_in[3]);

    oPatch.CP_Pos_b03 = CalcBoundaryPosition(Quad_Position_CS_in[0], Quad_Position_CS_in[3], Quad_Normal_CS_in[0]);
    oPatch.CP_Pos_b30 = CalcBoundaryPosition(Quad_Position_CS_in[3], Quad_Position_CS_in[0], Quad_Normal_CS_in[3]);



    // Edge Normals
    oPatch.Normal_N01 = CalcBoundaryNormal(Quad_Position_CS_in[0], Quad_Position_CS_in[1], Quad_Normal_CS_in[0], Quad_Normal_CS_in[1]);
    oPatch.Normal_N12 = CalcBoundaryNormal(Quad_Position_CS_in[1], Quad_Position_CS_in[2], Quad_Normal_CS_in[1], Quad_Normal_CS_in[2]);
    oPatch.Normal_N23 = CalcBoundaryNormal(Quad_Position_CS_in[2], Quad_Position_CS_in[3], Quad_Normal_CS_in[2], Quad_Normal_CS_in[3]);
    oPatch.Normal_N03 = CalcBoundaryNormal(Quad_Position_CS_in[0], Quad_Position_CS_in[3], Quad_Normal_CS_in[0], Quad_Normal_CS_in[3]);

    vec3 q =  oPatch.CP_Pos_b01 + oPatch.CP_Pos_b10
    + oPatch.CP_Pos_b12 + oPatch.CP_Pos_b21
    + oPatch.CP_Pos_b23 + oPatch.CP_Pos_b32
    + oPatch.CP_Pos_b03 + oPatch.CP_Pos_b30;

    // Inter Control Points
    vec3 e0 = (2.0 * (oPatch.CP_Pos_b01 + oPatch.CP_Pos_b03 + q) - (oPatch.CP_Pos_b21 + oPatch.CP_Pos_b23)) / 18.0;
    vec3 v0 = (4.0 * oPatch.CP_Pos_b0 + 2.0 * (oPatch.CP_Pos_b3 + oPatch.CP_Pos_b1) + oPatch.CP_Pos_b2) / 9.0;
    oPatch.CP_Pos_b02 = 3.0 * e0 / 2.0 - v0 / 2.0;

    vec3 e1 = (2.0 * (oPatch.CP_Pos_b12 + oPatch.CP_Pos_b10 + q) - (oPatch.CP_Pos_b32 + oPatch.CP_Pos_b30)) / 18.0;
    vec3 v1 = (4.0 * oPatch.CP_Pos_b1 + 2.0 * (oPatch.CP_Pos_b0 + oPatch.CP_Pos_b2) + oPatch.CP_Pos_b3) / 9.0;
    oPatch.CP_Pos_b13 = 3.0 * e1 / 2.0 - v1 / 2.0;

    vec3 e2 = (2.0 * (oPatch.CP_Pos_b23 + oPatch.CP_Pos_b21 + q) - (oPatch.CP_Pos_b03 + oPatch.CP_Pos_b01)) / 18.0;
    vec3 v2 = (4.0 * oPatch.CP_Pos_b2 + 2.0 * (oPatch.CP_Pos_b1 + oPatch.CP_Pos_b3) + oPatch.CP_Pos_b0) / 9.0;
    oPatch.CP_Pos_b20 = 3.0 * e2 / 2.0 - v2 / 2.0;

    vec3 e3 = (2.0 * (oPatch.CP_Pos_b30 + oPatch.CP_Pos_b32 + q) - (oPatch.CP_Pos_b10 + oPatch.CP_Pos_b12)) / 18.0;
    vec3 v3 = (4.0 * oPatch.CP_Pos_b3 + 2.0 * (oPatch.CP_Pos_b2 + oPatch.CP_Pos_b0) + oPatch.CP_Pos_b1) / 9.0;
    oPatch.CP_Pos_b31 = 3.0 * e3 / 2.0 - v3 / 2.0;

    oPatch.Normal_N0123 = (oPatch.Normal_N0 + oPatch.Normal_N1 + oPatch.Normal_N2 + oPatch.Normal_N3
    + 2.0 * (oPatch.Normal_N01 + oPatch.Normal_N12 + oPatch.Normal_N23 +oPatch.Normal_N03))/ 12.0;
}

void main()
{

    CalcInterpolation();

    // Set tessellation level
    gl_TessLevelOuter[0] = tessellationLevelOuter.x;
    gl_TessLevelOuter[1] = tessellationLevelOuter.y;
    gl_TessLevelOuter[2] = tessellationLevelOuter.z;
    gl_TessLevelOuter[3] = tessellationLevelOuter.x;

    gl_TessLevelInner[0] = tessellationLevelInner;

}
