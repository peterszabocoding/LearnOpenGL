#version 450

// ------------------------------------------------------------------
// INPUT VARIABLES --------------------------------------------------
// ------------------------------------------------------------------

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

in vec4 ViewPos;
in vec4 WorldPos;

// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

layout (location = 0) out vec4 gWorldPosition;
layout (location = 1) out vec4 gViewPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gRoughness;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(binding = 0) uniform sampler2D NormalTexture;
layout(binding = 1) uniform sampler2D RoughnessTexture;

uniform int bUseNormalMap;
uniform int bUseRoughnessMap;

uniform float roughnessValue;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

// ------------------------------------------------------------------
// FUNCTIONS --------------------------------------------------------
// ------------------------------------------------------------------

vec3 CalcNormal()
{
    vec3 normal;
    normal.xy = texture(NormalTexture, TexCoord).rg;
	normal.xy = 2.0 * normal.xy - 1.0;  
    normal.z = sqrt(1.0 - dot(normal.xy, normal.xy));
	return normalize(TBN * normal); 
}

// ------------------------------------------------------------------

void main()
{
    gWorldPosition= WorldPos;
    gViewPosition = ViewPos;

    mat3 worldToViewNormalMatrix = transpose(inverse(mat3(view * model)));
    vec3 viewSpaceNormal = bUseNormalMap > 0 ? CalcNormal() : Normal;
	
    gNormal	= vec4(worldToViewNormalMatrix * viewSpaceNormal, 1.0);
	gRoughness	= vec4(bUseNormalMap > 0  ? texture(RoughnessTexture, TexCoord).rgb : vec3(roughnessValue), 1.0);
}