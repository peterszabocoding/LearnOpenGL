#pragma once

// For use by Moongoose applications

#include <stdio.h>

#include "LearnOpenGL/Log.h"
#include "LearnOpenGL/Layer.h"
#include "LearnOpenGL/Ui/ImGuiLayer.h"
#include "LearnOpenGL/Application.h"

#include "LearnOpenGL/Input/Input.h"
#include "LearnOpenGL/Input/KeyCodes.h"
#include "LearnOpenGL/Input/MouseButtonCodes.h"
		  
#include "LearnOpenGL/Events/Event.h"
#include "LearnOpenGL/Events/ApplicationEvent.h"
#include "LearnOpenGL/Events/MouseEvents.h"
#include "LearnOpenGL/Events/KeyboardEvents.h"
		  
#include "LearnOpenGL/Renderer/Mesh.h"
#include "LearnOpenGL/Renderer/MeshPrimitives.h"
#include "LearnOpenGL/Renderer/Buffer.h"
#include "LearnOpenGL/Renderer/Framebuffer.h"
#include "LearnOpenGL/Renderer/FramebufferManager.h"
#include "LearnOpenGL/Renderer/Material.h"
#include "LearnOpenGL/Renderer/Shader/Shader.h"
#include "LearnOpenGL/Renderer/Texture.h"
#include "LearnOpenGL/Renderer/Material.h"
#include "LearnOpenGL/Renderer/VertexArray.h"
#include "LearnOpenGL/Renderer/PerspectiveCamera.h"
#include "LearnOpenGL/Renderer/Renderer.h"
#include "LearnOpenGL/Renderer/RendererAPI.h"
#include "LearnOpenGL/Renderer/RenderCommand.h"
#include "LearnOpenGL/Renderer/Transform.h"

#include "LearnOpenGL/Asset/AssetManager.h"

#include "LearnOpenGL/ECS/Entity.h"
#include "LearnOpenGL/ECS/Components.h"
#include "LearnOpenGL/ECS/WorldManager.h"

#include "LearnOpenGL/Util/FileSystem.h"
