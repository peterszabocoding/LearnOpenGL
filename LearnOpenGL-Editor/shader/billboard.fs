#version 450

// ------------------------------------------------------------------
// INPUT VARIABLES --------------------------------------------------
// ------------------------------------------------------------------

in vec3 FragPos;
in vec2 TexCoord;
in flat int EntityID;
in float Distance;

// ------------------------------------------------------------------
// UNIFORMS ---------------------------------------------------------
// ------------------------------------------------------------------

layout(binding = 0) uniform sampler2D AlbedoTexture;
uniform vec3 TintColor;

// ------------------------------------------------------------------
// OUTPUT VARIABLES -------------------------------------------------
// ------------------------------------------------------------------

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;

void main()
{
    vec4 albedo = texture(AlbedoTexture, TexCoord) * vec4(TintColor, 1.0);
    float transparency = clamp((Distance - 1.0) / (3.0 - 1.0), 0.0, 1.0);

    if (albedo.a < 0.01 || transparency <= 0.01) discard;

    FragColor = vec4(albedo.rgb, albedo.a * transparency);
    oEntityID = EntityID;
}
