#pragma once
#include "TextureAtlas.h"
#include "glm/vec3.hpp"

enum class ShadowType: uint8_t
{
	NONE = 0,
	HARD = 1,
	SOFT = 2
};

enum class ShadowMapResolution : uint16_t
{
	ULTRA_LOW = 128,
	LOW = 256,
	MEDIUM = 512,
	HIGH = 1024,
	VERY_HIGH = 2048,
	ULTRA_HIGH = 4096
};

struct Light
{
	glm::vec3 color = glm::vec3(1.0f);
	float intensity = 1.0f;
	ShadowType shadowType = ShadowType::NONE;
	ShadowMapResolution shadowMapResolution = ShadowMapResolution::MEDIUM;
	TextureAtlas::AtlasBox* shadowMapRegion = nullptr;
};

struct DirectionalLight : Light
{
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 ambientColor = glm::vec3(1.0f);
	float ambientIntensity = 0.1f;
};

struct PointLight : Light
{
	glm::vec3 position = glm::vec3(0.0f);
	float attenuationRadius = 10.0f;
};

struct SpotLight : PointLight
{
	glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	float attenuationAngle = 0.75f;
};