#version 450

in vec3 FragPos;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;


void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
    oEntityID = -1;
}
