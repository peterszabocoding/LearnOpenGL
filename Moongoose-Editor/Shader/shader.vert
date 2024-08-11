#version 450                                            

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec4 WorldPos;
out vec3 FragPos;
out vec4 ViewPos;
out vec2 TexCoord;
out vec3 Normal;
out vec3 EyePosition;
out mat3 TBN;
out flat int EntityID;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform int aEntityID;
uniform vec3 eyePosition;

void main()
{
   vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
   vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
   vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
   TBN = mat3(T, B, N);

   WorldPos = model * vec4(aPos, 1.0);
   ViewPos = view * WorldPos;
   gl_Position = projection * view * WorldPos;
   
   FragPos = WorldPos.xyz;
   TexCoord = aTexCoords;
   Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
   EntityID = aEntityID;
   EyePosition = eyePosition;
} 