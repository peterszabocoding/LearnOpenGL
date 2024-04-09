#version 450                                            

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
   vec4 world_pos = model * vec4(aPos, 1.0);
   mat4 rotView = mat4(mat3(view));

   gl_Position = projection * view * world_pos;

   FragPos = world_pos.xyz;
   TexCoord = aTexCoords;
   Normal = transpose(inverse(mat3(model))) * aNormal;
} 