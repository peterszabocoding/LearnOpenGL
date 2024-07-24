#version 450

in vec3 FragPos;
in vec2 TexCoord;
in flat int EntityID;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;

layout(binding = 0) uniform sampler2D AlbedoTexture;

void main()
{
    vec4 albedo = texture(AlbedoTexture, TexCoord);
    if (albedo.a < 0.1) discard;

    FragColor = vec4(albedo.rgb, 1.0);
    oEntityID = EntityID;
}
