#include "mgpch.h"
#include "Texture.h"
#include "OpenGL/OpenGLTexture2D.h"
#include "OpenGL/OpenGLTextureCube.h"

namespace Moongoose{

	Ref<Texture2D> Texture2D::Create()
	{
		return CreateRef<OpenGLTexture2D>();
	}

	Ref<TextureCube> TextureCube::Create()
	{
		return CreateRef<OpenGLTextureCube>();
	}

}
