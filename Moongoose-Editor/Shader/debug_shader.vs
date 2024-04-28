#version 450                                            

layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
   vec4 world_pos = model * vec4(aPos, 1.0);
   gl_Position = projection * view * world_pos;
   FragPos = world_pos.xyz;
} 