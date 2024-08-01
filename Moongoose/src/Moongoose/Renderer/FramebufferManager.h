#pragma once

#include <unordered_map>
#include "Moongoose/Renderer/Framebuffer.h"

namespace Moongoose {

	class FramebufferManager
	{
	public:
		static FramebufferManager& Get()
		{
			static FramebufferManager manager;
			return manager;
		}

		static Ref<Framebuffer> CreateFramebuffer(const std::string& name, const FramebufferSpecs& specs);
		static Ref<Framebuffer> GetFramebufferByName(const std::string& name);
		static std::vector<Ref<Framebuffer>> GetFramebuffers();

	private:
		static std::unordered_map<std::string, Ref<Framebuffer>> s_Framebuffers;
	};
}

