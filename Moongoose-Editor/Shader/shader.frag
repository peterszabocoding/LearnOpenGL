#version 450

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in flat int EntityID;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;

layout(binding = 0) uniform sampler2D AlbedoTexture;

struct Light {
	vec3 color;
	float intensity;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

uniform DirectionalLight directionalLight;

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) {
	float diffuseFactor = clamp(dot(Normal, normalize(direction)), 0.0, 1.0);
	vec4 diffuseColor = vec4(light.color * light.intensity * diffuseFactor, 1.0f);
	return diffuseColor;
}

vec4 CalcDirectionalLight() {
	//float shadowFactor = CalcShadowFactor(directionalShadowMap, directionalLight.base);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, 1.0f);
}

void main()
{
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f);
    float diff = max(dot(Normal, vec3(1.0f, 1.0f, 1.0f)), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

    vec4 albedo = texture(AlbedoTexture, TexCoord);
    vec4 color = albedo * CalcDirectionalLight();

    FragColor = vec4(color.rgb, 1.0f);
    oEntityID = EntityID;
}
