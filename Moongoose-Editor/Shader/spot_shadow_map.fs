#version 330 core
out vec4 FragColor;

uniform float nearPlane;
uniform float farPlane;
uniform 

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{             
    FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / farPlane), 1.0); // perspective
}  