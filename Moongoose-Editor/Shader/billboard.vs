#version 450                                            

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoord;
out flat int EntityID;
out float Distance;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec3 eyePosition;
uniform int aEntityID;

void main()
{
    vec3 objPos = vec3(model * vec4(0.0, 0.0, 0.0, 1.0));
    vec3 toCamera = normalize(eyePosition - objPos);
    vec3 right = cross(vec3(0.0, 1.0, 0.0), toCamera);
    vec3 up = cross(toCamera, right);
    
    mat4 billboardMatrix = mat4(
        vec4(right, 0.0),
        vec4(up, 0.0),
        vec4(toCamera, 0.0),
        vec4(objPos, 1.0)
    );

    gl_Position = projection * view * billboardMatrix * vec4(aPos, 1.0);

    FragPos = gl_Position.xyz;
    Distance = length((eyePosition - objPos));
    TexCoord = aTexCoords;
    EntityID = aEntityID;
} 