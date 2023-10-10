layout(quads, equal_spacing, ccw) in;

// Uniforms
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

struct QuadCP_FragData
{
    vec3 pos;
    vec3 normal;
    vec4 color;
};

struct Output_QuadPatch
{
    // Four input control point
    vec3 CP_Pos_b0;
    vec3 CP_Pos_b1;
    vec3 CP_Pos_b2;
    vec3 CP_Pos_b3;
    vec4 CP_Color_b0;
    vec4 CP_Color_b1;
    vec4 CP_Color_b2;
    vec4 CP_Color_b3;

    // Boundary control points
    vec3 CP_Pos_b01;
    vec3 CP_Pos_b10;
    vec3 CP_Pos_b12;
    vec3 CP_Pos_b21;
    vec3 CP_Pos_b23;
    vec3 CP_Pos_b32;
    vec3 CP_Pos_b03;
    vec3 CP_Pos_b30;
    vec4 CP_Color_b01;
    vec4 CP_Color_b10;
    vec4 CP_Color_b12;
    vec4 CP_Color_b21;
    vec4 CP_Color_b23;
    vec4 CP_Color_b32;
    vec4 CP_Color_b03;
    vec4 CP_Color_b30;


    // Inner control points
    vec3 CP_Pos_b02;
    vec3 CP_Pos_b20;
    vec3 CP_Pos_b13;
    vec3 CP_Pos_b31;
    vec3 CP_Color_b02;
    vec3 CP_Color_b20;
    vec3 CP_Color_b13;
    vec3 CP_Color_b31;

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

// attributes of input patch control points
patch in Output_QuadPatch oPatch;

// attributes of output patch control points
out QuadCP_FragData fragdata;
out vec3 position_worldspace;
out vec3 normal_cameraspace;
out vec3 eyeDirection_cameraspace;
out vec3 lightDirection_cameraspace;

vec3 Worldspace_LightPos;


void main()
{
    // Set fixed world space light source position
    vec3 lightPosition_worldspace = vec3(5.f, 0.f, 0.f);

    // Use gl_TessCoord to evaluate position and normal
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    float B_0u = pow(1.0 - u, 3.f);					// C03 * pow(1-u, 3) * pow(u, 0)
    float B_1u = 3.0 * pow(1.0 - u, 2.f) * u;			// C13 * pow(1-u, 2) * pow(u, 1)
    float B_2u = 3.0 * (1.0 - u) * pow(u, 2.f);		// C23 * pow(1-u, 1) * pow(u, 2)
    float B_3u = pow(u, 3.f);							// C33 * pow(1-u, 0) * pow(u, 3)

    float B_0v = pow(1.0 - v, 3.f);					// C03 * pow(1-v, 3) * pow(v, 0)
    float B_1v = 3.0 * pow(1.0 - v, 2.f) * v;			// C13 * pow(1-v, 2) * pow(v, 1)
    float B_2v = 3.0 * (1.0 - v) * pow(v, 2.f);		// C23 * pow(1-v, 1) * pow(v, 2)
    float B_3v = pow(v, 3.f);							// C33 * pow(1-v, 0) * pow(v, 3)

    float N_u0 = pow(1.0 - u, 2.f);
    float N_u1 = 2.0 * u * (1.0 - u);
    float N_u2 = pow(u, 2.f);

    float N_v0 = pow(1.0 - v, 2.f);
    float N_v1 = 2.0 * v * (1.0 - v);
    float N_v2 = pow(v, 2.f);

    vec3 pos =	  B_0u * (B_0v * oPatch.CP_Pos_b0   + B_1v * oPatch.CP_Pos_b01 + B_2v * oPatch.CP_Pos_b10 + B_3v * oPatch.CP_Pos_b1 )
    + B_1u * (B_0v * oPatch.CP_Pos_b03	+ B_1v * oPatch.CP_Pos_b02 + B_2v * oPatch.CP_Pos_b13 + B_3v * oPatch.CP_Pos_b12)
    + B_2u * (B_0v * oPatch.CP_Pos_b30	+ B_1v * oPatch.CP_Pos_b31 + B_2v * oPatch.CP_Pos_b20 + B_3v * oPatch.CP_Pos_b21)
    + B_3u * (B_0v * oPatch.CP_Pos_b3	+ B_1v * oPatch.CP_Pos_b32 + B_2v * oPatch.CP_Pos_b23 + B_3v * oPatch.CP_Pos_b2);

    //vec3 color =  B_0u * (B_0v * oPatch.CP_Color_b0		+ B_1v * oPatch.CP_Color_b01 + B_2v * oPatch.CP_Color_b10 + B_3v * oPatch.CP_Color_b1 )
    //			+ B_1u * (B_0v * oPatch.CP_Color_b03	+ B_1v * oPatch.CP_Color_b02 + B_2v * oPatch.CP_Color_b13 + B_3v * oPatch.CP_Color_b12)
    //			+ B_2u * (B_0v * oPatch.CP_Color_b30	+ B_1v * oPatch.CP_Color_b31 + B_2v * oPatch.CP_Color_b20 + B_3v * oPatch.CP_Color_b21)
    //			+ B_3u * (B_0v * oPatch.CP_Color_b3		+ B_1v * oPatch.CP_Color_b32 + B_2v * oPatch.CP_Color_b23 + B_3v * oPatch.CP_Color_b2);

    vec3 normal = N_u0 * (N_v0 * oPatch.Normal_N0  + N_v1 * oPatch.Normal_N01   + N_v2 * oPatch.Normal_N1)
    + N_u1 * (N_v0 * oPatch.Normal_N03 + N_v1 * oPatch.Normal_N0123 + N_v2 * oPatch.Normal_N12)
    + N_u2 * (N_v0 * oPatch.Normal_N3  + N_v1 * oPatch.Normal_N23   + N_v2 * oPatch.Normal_N2);

    fragdata.pos = pos;
    fragdata.normal	= normal;
    fragdata.color	= vec4(1.f, 0.4f, 0.3f, 1);

    gl_Position = P * V * M * vec4(pos, 1.0);
    position_worldspace = vec3((M * vec4(pos, 1.0)).xyz);
    normal_cameraspace	= vec3((V * M * vec4(normal, 1.0)).xyz);
    eyeDirection_cameraspace = vec3(0.0f, 0.0f, 0.0f) - vec3((V * M * vec4(pos, 1.0)).xyz);
    vec3 lightPosition_cameraspace = (V * vec4(lightPosition_worldspace, 1.f)).xyz;
    lightDirection_cameraspace = lightPosition_cameraspace + eyeDirection_cameraspace;
}