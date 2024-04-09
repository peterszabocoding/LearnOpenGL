#pragma once

#include "Mesh.h"

namespace Moongoose {

	static Mesh QuadMesh() {
		// Position - UVs
		GLfloat vertices[] = {
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		};

		unsigned int indices[] = {
			0, 1, 2,
			1, 3, 2
		};

		return Mesh(vertices, indices, 20, 6, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
	}

	static Mesh Triangle() {
		// Position - UVs
		GLfloat vertices[] = {
			0.5f,  0.0f, 0.0f,		0.0f, 1.0f,
			0.75f, 0.75f, 0.0f,		0.0f, 0.0f,
			0.25f,  0.75f, 0.0f,		1.0f, 1.0f,
		};

		unsigned int indices[] = {
			0, 1, 2
		};

		return Mesh(vertices, indices, 20, 6, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
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

		return new Mesh(vertices, indices, 24, 36, {
			  { ShaderDataType::Float3, "aPos" },
			});
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

		return new Mesh(vertices, indices, 8 * 24, 36, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float2, "aTexCoords" }
			});
	}

}