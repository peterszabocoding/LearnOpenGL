#include "pch.h"
#include "RenderCommand.h"

namespace LearnOpenGL
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = CreateScope<RendererAPI>();
	unsigned int RenderCommand::drawCount;
}
