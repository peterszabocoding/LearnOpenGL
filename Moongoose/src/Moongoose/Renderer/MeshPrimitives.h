#pragma once

#include "Mesh.h"
#include "Moongoose/Core.h"
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

namespace Moongoose {

	static Mesh QuadMesh(float scale = 1.0f) {
		// Position - UVs
		float vertices[] = {
			-scale, -scale, 0.0f,	0.0f, 0.0f,
			 scale, -scale, 0.0f,	1.0f, 0.0f,
			-scale,  scale, 0.0f,	0.0f, 1.0f,
			 scale,  scale, 0.0f,	1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 3, 2,
			0, 1, 3
		};
		Mesh mesh;
		mesh.AddSubmesh(0, vertices, indices, 4 * 5, 6, 
			{
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
		return mesh;
	}

	static Mesh QuadMeshWorld(float scale = 1.0f) {
		// Position - UVs
		float vertices[] = {
			-scale, -scale, 0.0f,	0.0f, 1.0f,
			 scale, -scale, 0.0f,	1.0f, 1.0f,
			-scale,  scale, 0.0f,	0.0f, 0.0f,
			 scale,  scale, 0.0f,	1.0f, 0.0f,
		};

		unsigned int indices[] = {
			0, 3, 2,
			0, 1, 3
		};
		Mesh mesh;
		mesh.AddSubmesh(0, vertices, indices, 4 * 5, 6,
			{
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
		return mesh;
	}

	static Ref<Mesh> QuadMeshRef(float scale = 1.0f)
	{
		// Position - UVs
		float vertices[] = {
			-scale, -scale, 0.0f,	0.0f, 1.0f,
			 scale, -scale, 0.0f,	1.0f, 1.0f,
			-scale,  scale, 0.0f,	0.0f, 0.0f,
			 scale,  scale, 0.0f,	1.0f, 0.0f,
		};

		unsigned int indices[] = {
			0, 3, 2,
			0, 1, 3
		};

		Ref<Mesh> mesh = CreateRef<Mesh>();
		mesh->AddSubmesh(0, vertices, indices, 4 * 5, 6,
			{
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
		return mesh;
	}

	static Mesh Triangle() {
		// Position - UVs
		float vertices[] = {
			0.5f,  0.0f, 0.0f,		0.0f, 1.0f,
			0.75f, 0.75f, 0.0f,		0.0f, 0.0f,
			0.25f,  0.75f, 0.0f,	1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2
		};

		Mesh mesh;
		mesh.AddSubmesh(0, 
			vertices, indices, 	20, 	6, 
			{
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});

		return mesh;
	}

	static Mesh* SkyboxCube(float size) {
		float vertices[] = {
			 -size,   -size,     size,
			  size,   -size,     size,
			  size,   -size,    -size,
			 -size,   -size,    -size,
			 -size,    size,     size,
			  size,    size,     size,
			  size,    size,    -size,
			 -size,    size,    -size,
		};

		unsigned int indices[] = {
			1, 2, 6,
			6, 5, 1,

			0, 4, 7,
			7, 3, 0,

			4, 5, 6,
			6, 7, 4,

			0, 3, 2,
			2, 1, 0,

			0, 1, 5,
			5, 4, 0,

			3, 7, 6,
			6, 2, 3
		};

		Mesh* mesh = new Mesh();
		mesh->AddSubmesh(0, vertices, indices, 24, 36, {
			  { ShaderDataType::Float3, "aPos" },
			});

		return mesh;
	}

	static Mesh* EnvCube() {
		// Position - Normals - UVs
		float vertices[] = {
			// back face
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right              0
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left              1
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right          2
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left          3

			// front face
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right                4
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left                5
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right            6
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left            7

			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right              8
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left              9
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right          10
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left          11

			// right face
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right               12
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left                 13
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right           14
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left             15

			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right            16
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left              17
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right          18
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left            19

			// top face
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right               20
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left               21
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right             22
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f  // bottom-left             23
		};

		unsigned int indices[] = {
			3, 0, 2,
			0, 3, 1,

			7, 6, 4,
			4, 5, 7,

			8, 9, 11,
			11, 10, 8,

			13, 14, 12,
			14, 13, 15,

			16, 17, 19,
			19, 18, 16,

			21, 22, 20,
			22, 21, 23
		};

		Mesh* mesh = new Mesh();
		mesh->AddSubmesh(0, vertices, indices, 8 * 24, 36, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});

		return mesh;
	}

	static Mesh Sphere(float radius, unsigned int rings, unsigned int sectors) {
		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		float const R = 1/(float)(rings - 1);
		float const S = 1/(float)(sectors - 1);
		int r, s;

		vertices.resize(rings * sectors * 8);

		std::vector<float>::iterator v = vertices.begin();
		for (r = 0; r < rings; r++) {
			for (s = 0; s < sectors; s++) {
				float const y = sin(-M_PI_2 + M_PI * r * R);
				float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
				float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

				*v++ = x * radius;
				*v++ = y * radius;
				*v++ = z * radius;

				*v++ = x;
				*v++ = y;
				*v++ = z;

				*v++ = s * S;
				*v++ = r * R;
			}
		}

		indices.resize(rings * sectors * 4);
		std::vector<unsigned int>::iterator i = indices.begin();
		for (r = 0; r < rings; r++) {
			for (s = 0; s < sectors; s++) {
				*i++ = r * sectors + s;
				*i++ = r * sectors + (s + 1);
				*i++ = (r + 1) * sectors + (s + 1);
				*i++ = (r + 1) * sectors + s;
			}
		}

		Mesh mesh;
		mesh.AddSubmesh(0, &vertices[0], &indices[0], 8 * vertices.size(), indices.size(), {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
		return mesh;
	}

	static Ref<Mesh> BoundingBox(glm::vec3 min, glm::vec3 max)
	{
		glm::vec3 diagonal = max - min;

		std::vector<glm::vec4> vertices4 = { 
			glm::vec4(	-1.0f,	-1.0f,	1.0f,	1.0f),
			glm::vec4(	-1.0f,	-1.0f, -1.0f,	1.0f),
			glm::vec4(	1.0f,	-1.0f, -1.0f,	1.0f),
			glm::vec4(	1.0f,	-1.0f,	1.0f,	1.0f),
			glm::vec4(	-1.0f,	1.0f,	1.0f,	1.0f),
			glm::vec4(	-1.0f,	1.0f,	-1.0f,	1.0f),
			glm::vec4(	1.0f,	1.0f,	-1.0f,	1.0f),
			glm::vec4(	1.0f,	1.0f,	1.0f,	1.0f)
		};

		auto transform = glm::translate(glm::mat4(1.0f), min + diagonal * 0.5f) 
			* glm::scale(glm::mat4(1.0f), diagonal * 0.5f);

		for (auto& vCoord : vertices4)
		{
			vCoord = transform * vCoord;
		}

		float vertices[] = {
			vertices4[0].x, vertices4[0].y, vertices4[0].z,
			vertices4[1].x, vertices4[1].y, vertices4[1].z,
			vertices4[2].x, vertices4[2].y, vertices4[2].z,
			vertices4[3].x, vertices4[3].y, vertices4[3].z,
			vertices4[4].x, vertices4[4].y, vertices4[4].z,
			vertices4[5].x, vertices4[5].y, vertices4[5].z,
			vertices4[6].x, vertices4[6].y, vertices4[6].z,
			vertices4[7].x, vertices4[7].y, vertices4[7].z,
		};

		unsigned int indices[] = {
			0, 3, 7, // Front
			7, 4, 0,

			1, 5, 6, // Back
			6, 2, 1,

			0, 4, 5, // Left
			5, 1, 0,

			3, 2, 6, // Right
			6, 7, 3,

			4, 7, 6, // Top
			6, 5, 4,

			0, 1, 2, // Bottom
			2, 3, 0
		};

		Ref<Mesh> mesh = CreateRef<Mesh>();
		mesh->AddSubmesh(0, vertices, indices, 8 * 3, 36, { { ShaderDataType::Float3, "aPos" } });

		return mesh;
	}
}