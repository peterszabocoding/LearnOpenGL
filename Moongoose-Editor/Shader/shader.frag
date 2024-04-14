#version 450

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

layout(binding = 0) uniform sampler2D AlbedoTexture;

void main()
{
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f);
    float diff = max(dot(Normal, vec3(1.0f, 1.0f, 1.0f)), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec4 textureColor = texture(AlbedoTexture, TexCoord);
    vec3 color = (ambient + diffuse) * textureColor.rgb;
    //vec3 color = (ambient + diffuse) * vec3(0.5, 0.5, 0.5);
    FragColor = vec4(color, 1.0f);
} 