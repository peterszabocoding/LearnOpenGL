#include "FramebufferViewerLayer.h"
#include "GUI/GuiWidgets.h"


void FramebufferViewerLayer::onImGuiRender()
{
	ImGui::Begin("FBO Preview");

	std::vector<Ref<Moongoose::Framebuffer>> framebuffers = Moongoose::FramebufferManager::GetFramebuffers();
	std::vector<std::string> framebufferNames;

	for (Ref<Moongoose::Framebuffer> buffer : framebuffers) framebufferNames.push_back(buffer->m_Name);

	GuiWidgets::DrawSingleSelectDropdown("Framebuffers: ", framebufferNames, selectedFramebuffer, [&](int selected) {
		selectedFramebuffer = selected;
	});

	ImGui::Separator();

	Ref<Moongoose::Framebuffer> selectedBuffer = framebuffers[selectedFramebuffer];
	ImVec2 availableSpace = ImGui::GetContentRegionAvail();

	float aspect = (float) selectedBuffer->m_Specs.Height / (float) selectedBuffer->m_Specs.Width;
	ImVec2 imageSize = { 
		availableSpace.x, 
		availableSpace.x * aspect
	};

	ImGui::Text("Resolution: %d x %d", selectedBuffer->m_Specs.Width, selectedBuffer->m_Specs.Height);

	size_t i = 0;
	std::vector<std::string> bufferAttachments;
	for (size_t i = 0; i < selectedBuffer->m_ColorAttachmentSpecs.size(); i++)
	{
		std::string attachmentName = Moongoose::Util::FramebufferTextureFormatToString(selectedBuffer->m_ColorAttachmentSpecs[i].TextureFormat);
		bufferAttachments.push_back(std::to_string(i + 1) + ": " + attachmentName);
	}

	GuiWidgets::DrawSingleSelectDropdown("Color Attachments: ", bufferAttachments, selectedAttachment, [&](int selected) {
		selectedAttachment = selected;
	});


	// Draw selected color attachment
	ImGui::Image((void*)selectedBuffer->GetColorAttachments()[selectedAttachment], imageSize, ImVec2(0, 1), ImVec2(1, 0));
	
	// Dra
	if (selectedBuffer->GetDepthAttachment() != 0)
	{
		ImGui::Text("Depth Attachment:");
		ImGui::Image((void*)selectedBuffer->GetDepthAttachment(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
}
