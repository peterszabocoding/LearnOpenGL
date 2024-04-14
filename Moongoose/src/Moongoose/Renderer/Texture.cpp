#include "mgpch.h"
#include "Texture.h"
#include "Moongoose/Renderer/OpenGL/OpenGLTexture2D.h"

namespace Moongoose{

	Ref<Texture2D> Texture2D::Create(TextureSpecs specs)
	{
		return CreateRef<OpenGLTexture2D>(specs);
	}

}