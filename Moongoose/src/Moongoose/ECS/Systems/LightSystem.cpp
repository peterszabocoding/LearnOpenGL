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

	void LightSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
		Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
		shader->Bind();
		shader->ResetLights();

		for (auto const& entity : m_Entities)
		{
			LightComponent cLight = world->GetComponent<LightComponent>(entity);
			TransformComponent cTransform = world->GetComponent<TransformComponent>(entity);

			switch (cLight.m_Type)
			{
				case LightType::DIRECTIONAL:
					shader->SetDirectionalLight(
						cTransform.getTransform(),
						cLight.m_Color,
						cLight.m_Intensity);
					break;
				case LightType::POINT:
					shader->SetPointLight(
						cTransform.m_Position,
						cLight.m_Color,
						cLight.m_Intensity,
						0.3f, 0.2f,0.1f);
					break;
			}
		}

		shader->Unbind();
	}
}
