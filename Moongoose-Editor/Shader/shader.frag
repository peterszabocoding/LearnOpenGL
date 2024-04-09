#version 450

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    FragColor = vec4(TexCoord, 0.0f, 1.0f);
} 