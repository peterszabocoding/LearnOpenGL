#include "pch.h"
#include "FramebufferManager.h"

namespace LearnOpenGL
{
	std::unordered_map<std::string, Ref<Framebuffer>> FramebufferManager::s_Framebuffers;

	Ref<Framebuffer> FramebufferManager::CreateFramebuffer(const std::string& name)
	{
		Ref<Framebuffer> framebuffer = CreateRef<Framebuffer>(name);
		s_Framebuffers[name] = framebuffer;
		return framebuffer;
	}

	Ref<Framebuffer> FramebufferManager::GetFramebufferByName(const std::string& name)
	{
		return s_Framebuffers[name];
	}

	std::vector<Ref<Framebuffer>> FramebufferManager::GetFramebuffers()
	{
		std::vector<Ref<Framebuffer>> framebuffers;
		for (auto& framebuffer : s_Framebuffers) framebuffers.push_back(framebuffer.second);
		return framebuffers;
	}
}
