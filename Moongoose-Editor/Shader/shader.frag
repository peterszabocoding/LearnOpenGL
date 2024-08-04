#version 450

const int MAX_POINT_LIGHTS = 10;
const int MAX_SPOT_LIGHTS = 10;

in vec4 WorldPos;
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

layout(binding = 4) uniform sampler2DShadow ShadowMapTexture;

struct Light {
	vec3 color;
	float intensity;
	bool isShadowCasting;
	bool useSoftShadow;

	vec2 shadowMapTopLeft;
	vec2 shadowMapSize;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
	vec3 ambientColor;
	float ambientIntensity;
};

struct PointLight {
	Light base;
	vec3 position;
	float attenuationRadius;
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

uniform mat4 lightTransform;

vec3 normalizedNormal;
vec2 shadowMapTextureSize;

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

vec2 CalculateShadowMapRatio(vec2 shadowMapSize)
{
	return vec2(shadowMapSize.x / shadowMapTextureSize.x, shadowMapSize.y / shadowMapTextureSize.y);
}

float ReadShadowMap(sampler2DShadow shadowMap, vec2 shadowMapPosition, vec2 shadowMapSize, vec3 projCoords)
{
	vec3 offset = vec3(shadowMapPosition.x / shadowMapTextureSize.x, shadowMapPosition.y / shadowMapTextureSize.y, 0.0);
	vec3 ratio = vec3(CalculateShadowMapRatio(shadowMapSize), 1.0);

	vec3 newProjCoords = offset + projCoords * ratio;

	return texture(shadowMap, newProjCoords);
}

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

float CalcPCFSoftShadow(sampler2DShadow shadowMap, vec3 projectedCoords, float bias) {
	float shadow = 0.0;
	vec2 texelSize = 1.0 / directionalLight.base.shadowMapSize;
	for(int x = -2; x <= 2; ++x) {
		for(int y = -2; y <= 2; ++y) {
			float randomFactor = 0.15;
			vec3 projCoords = vec3(projectedCoords.xy + vec2(x + randomFactor * rand(gl_FragCoord.xy),y + randomFactor * rand(gl_FragCoord.xy)) * texelSize, projectedCoords.z - bias);
			
			if (projCoords.x < 0 || projCoords.y < 0 || projCoords.x > 1.0 || projCoords.y > 1.0) return 1.0;
			shadow += ReadShadowMap(shadowMap, directionalLight.base.shadowMapTopLeft, directionalLight.base.shadowMapSize, projCoords).r;
		}
	}
	return shadow / 25.0;
} 

float CalcShadowFactor(sampler2DShadow shadowMap, Light light) {
	if (!light.isShadowCasting) return 1.0;

	vec4 lightSpacePos = lightTransform * WorldPos;
	vec3 projCoords = lightSpacePos.rgb / lightSpacePos.a;
	projCoords = (projCoords * 0.5) + 0.5;

	float bias = 0.005;

	if (projCoords.z - bias > 1.0) return 1.0;

	float shadowFactor = light.useSoftShadow 
		? CalcPCFSoftShadow(shadowMap, projCoords, bias) 
		: ReadShadowMap(shadowMap, vec2(0.0, 0.0), vec2(0.0, 0.0), projCoords);

	return shadowFactor;
}

float CalcLightAttenuation(vec3 fragPos, vec3 lightPos, float rmax)
{
	vec3 ldir = lightPos - fragPos;
	float dist = length(ldir);
	if (dist >= rmax) return 0.0;

	float r2 = dot(ldir, ldir);
	vec2 attenuationConstant = vec2(1.0 / (rmax * rmax), 2.0 / rmax);

	return clamp(r2 * attenuationConstant.x * (sqrt(r2) * attenuationConstant.y - 3.0) + 1.0, 0.0, 1.0);
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) 
{
	float diffuseFactor = clamp(dot(Normal, normalize(direction)), 0.0, 1.0);
	vec4 diffuseColor = vec4(light.color * light.intensity * diffuseFactor, 1.0f);

	return diffuseColor * shadowFactor;
}

vec4 CalcDirectionalLight() 
{
	float shadowFactor = CalcShadowFactor(ShadowMapTexture, directionalLight.base);
	return vec4(directionalLight.ambientColor, 1.0) * directionalLight.ambientIntensity
	 + CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight) 
{
	vec3 direction = pLight.position - FragPos;
	float dist = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pLight.base, direction, 1.0);
	float attenuation = CalcLightAttenuation(FragPos, pLight.position, pLight.attenuationRadius);
	return color * attenuation;
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

	shadowMapTextureSize = textureSize(ShadowMapTexture, 0);

	//vec3 n = texture(NormalTexture, TexCoord).rgb;
	vec3 n = vec3(0.5, 0.5, 1);
	n = n * 2.0 - 1.0;   
	n = normalize(TBN * n); 
	normalizedNormal = normalize(n);

    vec4 albedo = texture(AlbedoTexture, TexCoord);
    vec4 color = albedo * (CalcDirectionalLight() + CalcPointLights() + CalcSpotLights());

    FragColor = vec4(color.rgb, 1.0f);
    oEntityID = EntityID;
}
