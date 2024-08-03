#version 450

in vec3 FragPos;
in vec2 TexCoord;
in flat int EntityID;
in float Distance;

uniform vec3 TintColor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;

layout(binding = 0) uniform sampler2D AlbedoTexture;

void main()
{
    vec4 albedo = texture(AlbedoTexture, TexCoord) * vec4(TintColor, 1.0);
    float transparency = clamp((Distance - 1.0) / (3.0 - 1.0), 0.0, 1.0);

    if (albedo.a < 0.1 || transparency <= 0.01) discard;

    FragColor = vec4(albedo.rgb, transparency);
    oEntityID = EntityID;
}
