#include "mgpch.h"
#include "LightSystem.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	Signature LightSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<TransformComponent>());
		signature.set(world->GetComponentType<LightComponent>());

		return signature;
	}

	void LightSystem::Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
		shader->Bind();
		shader->ResetLights();

		for (auto const& entity : m_Entities)
		{
			const auto cLight = world->GetComponent<LightComponent>(entity);
			auto cTransform = world->GetComponent<TransformComponent>(entity);

			switch (cLight.m_Type)
			{
				case LightType::Directional:
					shader->SetDirectionalLight(
						cTransform.GetForwardDirection(),
						cLight.m_Color,
						cLight.m_Intensity);
					break;
				case LightType::Point:
					shader->SetPointLight(
						cTransform.m_Position,
						cLight.m_Color,
						cLight.m_Intensity,
						cLight.m_AttenuationRadius);
					break;
				case LightType::Spot:
					shader->SetSpotLight(
						cTransform.GetTransform(),
						cTransform.m_Position,
						cLight.m_Color,
						cLight.m_Intensity,
						cLight.m_AttenuationRadius, 
						cLight.m_AttenuationAngle);
					break;
			}
		}

		shader->Unbind();
	}
}
