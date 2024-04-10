#version 450

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

void main()
{
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f);
    float diff = max(dot(Normal, vec3(1.0f, 1.0f, 1.0f)), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec3 color = (ambient + diffuse) * vec3(0.5f, 0.5f, 0.5f);
    FragColor = vec4(color, 1.0f);
} 