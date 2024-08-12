#version 450                                            

layout (location = 0) in vec3 aPos; 


struct Light {
	vec3 color;
	float intensity;
	bool isShadowCasting;
	bool useSoftShadow;

	float bias;
	vec2 shadowMapTopLeft;
	vec2 shadowMapSize;

	mat4 lightTransform;
};

uniform mat4 model;
uniform Light light;

void main()
{
   gl_Position = light.lightTransform * model * vec4(aPos, 1.0);
} 