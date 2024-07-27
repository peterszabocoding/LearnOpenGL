#version 450

const int MAX_POINT_LIGHTS = 100;
const int MAX_SPOT_LIGHTS = 100;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in vec3 EyePosition;
in flat int EntityID;
in mat3 TBN;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int oEntityID;

layout(binding = 0) uniform sampler2D AlbedoTexture;
layout(binding = 1) uniform sampler2D NormalTexture;
layout(binding = 2) uniform sampler2D MetallicTexture;
layout(binding = 3) uniform sampler2D RoughnessTexture;

struct Light {
	vec3 color;
	float intensity;
	bool isShadowCasting;
	bool useSoftShadow;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct SpotLight {
	PointLight base;
	vec3 direction;
	float attenuationAngle;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;
uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform Material material;

vec3 normalizedNormal;




const float PI = 3.14159265359;
// ----------------------------------------------------------------------------

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// ----------------------------------------------------------------------------

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------


// ------------------------ LIGHTS --------------------------------------------

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) 
{
	float diffuseFactor = clamp(dot(Normal, normalize(direction)), 0.0, 1.0);
	vec4 diffuseColor = vec4(light.color * light.intensity * diffuseFactor, 1.0f);

	return diffuseColor * shadowFactor;
}

vec4 CalcDirectionalLight() 
{
	//float shadowFactor = CalcShadowFactor(directionalShadowMap, directionalLight.base);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, 1.0f);
}

vec4 CalcPointLight(PointLight pLight) 
{
	vec3 direction = pLight.position - FragPos;
	float dist = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pLight.base, direction, 1.0);
	float attenuation = pLight.exponent * dist * dist 
						+ pLight.linear * dist 
						+ pLight.constant;
	return color / attenuation;
}

vec4 CalcSpotLight(SpotLight sLight) 
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float slFactor = dot(rayDirection, sLight.direction);

	if (slFactor > sLight.attenuationAngle) {
		vec4 color = CalcPointLight(sLight.base);
		return color * (1.0 - (1.0 - slFactor) * (1.0/(1 - sLight.attenuationAngle)));
	} else {
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalcPointLights() 
{
	vec4 totalColor = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++) {
		totalColor += CalcPointLight(pointLights[i]);
	}
	return totalColor;
}

vec4 CalcSpotLights() 
{
	vec4 totalColor = vec4(0, 0, 0, 0);
	for(int i = 0; i < spotLightCount; i++) {
		totalColor += CalcSpotLight(spotLights[i]);
	}
	return totalColor;
}

// ----------------------------------------------------------------------------


void main()
{
    vec3 ambient = vec3(0.05f, 0.05f, 0.05f);
    float diff = max(dot(Normal, vec3(1.0f, 1.0f, 1.0f)), 0.0);
    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

	//vec3 n = texture(NormalTexture, TexCoord).rgb;
	vec3 n = vec3(0.5, 0.5, 1);
	n = n * 2.0 - 1.0;   
	n = normalize(TBN * n); 
	normalizedNormal = normalize(n);

    vec4 albedo = texture(AlbedoTexture, TexCoord);
    vec4 color = albedo * (CalcDirectionalLight() + CalcPointLights());

    FragColor = vec4(color.rgb, 1.0f);
    oEntityID = EntityID;
}
