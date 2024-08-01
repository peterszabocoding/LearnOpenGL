#version 450

in vec2 TexCoords;

layout(binding = 0) uniform sampler2D sky;
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(sky, TexCoords).rgb, 1.0);
}