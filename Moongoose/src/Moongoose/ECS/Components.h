#pragma once

#include <vector>
#include <bitset>
#include <glm/glm.hpp>
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Shader.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/Billboard.h"

namespace Moongoose {

	using ComponentType = std::uint8_t;
	const ComponentType MAX_COMPONENTS = 32;
	using Signature = std::bitset<MAX_COMPONENTS>;

	enum class LightType: uint8_t
	{
		DIRECTIONAL = 0,
		POINT,
		SPOT
	};

	namespace Utils {

		static std::string LightTypeToString(LightType type)
		{
			switch (type)
			{
			case LightType::DIRECTIONAL: return "Directional";
			case LightType::POINT: return "Point";
			case LightType::SPOT: return "Spot";
			}
			return "Unknown";
		}

		static std::vector<std::string> GetLightTypeStrings()
		{
			return { "Directional", "Point", "Spot" };
		}
	}

	struct Component
	{
		bool m_Active = false;
	};

	struct IDComponent : public Component
	{
		UUID m_ID = 0;

		IDComponent(UUID id = UUID()) : m_ID(id) {}
	};

	struct TagComponent: public Component
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {}

		operator std::string& () { return Tag; }
		operator const std::string& () const { return Tag; }
	};

	struct TransformComponent: public Component
	{
		glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 m_Scale = glm::vec3(1.0f, 1.0f, 1.0f);

		TransformComponent() = default;
		TransformComponent(
			glm::vec3 position, 
			glm::vec3 rotation, 
			glm::vec3 scale): m_Position(position), m_Rotation(rotation), m_Scale(scale) {}

		glm::mat4 getTransform() const {
			return glm::translate(glm::mat4(1.0f), m_Position)
				* glm::toMat4(glm::quat(glm::radians(m_Rotation)))
				* glm::scale(glm::mat4(1.0f), m_Scale);
		}

		static glm::mat4 GetModelMatrix(const TransformComponent& component)
		{
			return glm::translate(glm::mat4(1.0f), component.m_Position)
				* glm::toMat4(glm::quat(glm::radians(component.m_Rotation)))
				* glm::scale(glm::mat4(1.0f), component.m_Scale);
		}

		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
		{
			// From glm::decompose in matrix_decompose.inl

			using namespace glm;
			using T = float;

			mat4 LocalMatrix(transform);

			// Normalize the matrix.
			if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
				epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
			{
				// Clear the perspective partition
				LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
				LocalMatrix[3][3] = static_cast<T>(1);
			}

			// Next take care of translation (easy).
			translation = vec3(LocalMatrix[3]);
			LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

			vec3 Row[3], Pdum3;

			// Now get scale and shear.
			for (length_t i = 0; i < 3; ++i)
				for (length_t j = 0; j < 3; ++j)
					Row[i][j] = LocalMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = length(Row[0]);
			Row[0] = detail::scale(Row[0], static_cast<T>(1));
			scale.y = length(Row[1]);
			Row[1] = detail::scale(Row[1], static_cast<T>(1));
			scale.z = length(Row[2]);
			Row[2] = detail::scale(Row[2], static_cast<T>(1));

			// At this point, the matrix (in rows[]) is orthonormal.
			// Check for a coordinate system flip.  If the determinant
			// is -1, then negate the matrix and the scaling factors.
#if 0
			Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
			if (dot(Row[0], Pdum3) < 0)
			{
				for (length_t i = 0; i < 3; i++)
				{
					scale[i] *= static_cast<T>(-1);
					Row[i] *= static_cast<T>(-1);
				}
			}
#endif

			rotation.y = asin(-Row[0][2]);
			if (cos(rotation.y) != 0) {
				rotation.x = atan2(Row[1][2], Row[2][2]);
				rotation.z = atan2(Row[0][1], Row[0][0]);
			}
			else {
				rotation.x = atan2(-Row[2][0], Row[1][1]);
				rotation.z = 0;
			}


			return true;
		}
	};

	struct MeshComponent: public Component
	{
		Ref<Mesh> m_Mesh;

		MeshComponent() {};
		MeshComponent(Ref<Mesh> mesh) : m_Mesh(mesh) {}
	};

	struct LightComponent : public Component
	{
		LightType m_Type = LightType::POINT;
		glm::vec3 m_Color = glm::vec3(1.0f);
		float m_Intensity = 1.0f;
	};

	struct BillboardComponent : public Component
	{
		BillboardComponent() {}
		BillboardComponent(Ref<Texture2D> texture): m_BillboardTexture(texture) {}
		Ref<Texture2D> m_BillboardTexture;
	};

}