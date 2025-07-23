#include "FramebufferViewerLayer.h"
#include "GUI/GuiWidgets.h"


using namespace LearnOpenGL;

void FramebufferViewerLayer::onImGuiRender()
{
	ImGui::Begin("FBO Preview");

	std::vector<Ref<Framebuffer>> framebuffers = GetApplication()->GetFramebufferManager()->GetFramebuffers();
	if (framebuffers.empty())
	{
		ImGui::End();
		return;
	}


	std::vector<std::string> framebufferNames;

	for (Ref<Framebuffer> buffer : framebuffers) framebufferNames.push_back(buffer->m_Name);

	GuiWidgets::DrawSingleSelectDropdown("Framebuffers: ", framebufferNames, selectedFramebuffer, [&](const int selected) {
		selectedFramebuffer = selected;
	});

	ImGui::Separator();

	const Ref<Framebuffer> selectedBuffer = framebuffers[selectedFramebuffer];
	const ImVec2 availableSpace = ImGui::GetContentRegionAvail();

	const float aspect = (float) selectedBuffer->m_Specs.height / (float) selectedBuffer->m_Specs.width;
	const ImVec2 imageSize = { 
		availableSpace.x, 
		availableSpace.x * aspect
	};

	ImGui::Text("Resolution: %d x %d", selectedBuffer->m_Specs.width, selectedBuffer->m_Specs.height);

	std::vector<std::string> bufferAttachments;
	for (size_t i = 0; i < selectedBuffer->m_ColorAttachmentSpecs.size(); i++)
	{
		std::string attachmentName = Util::FramebufferTextureFormatToString(selectedBuffer->m_ColorAttachmentSpecs[i].TextureFormat);
		bufferAttachments.push_back(std::to_string(i + 1) + ": " + attachmentName);
	}

	if (!selectedBuffer->GetColorAttachments().empty())
	{
		GuiWidgets::DrawSingleSelectDropdown("Color Attachments: ", bufferAttachments, selectedAttachment, [&](const int selected) {
			selectedAttachment = selected;
		});

		// Draw selected color attachment
		if (selectedBuffer->GetColorAttachments()[selectedAttachment] != 0)
		{
			ImGui::Image((void*)selectedBuffer->GetColorAttachments()[selectedAttachment], imageSize, ImVec2(0, 1), ImVec2(1, 0));
		}
	}
	
	// Draw depth attachment
	if (selectedBuffer->GetDepthAttachment() != 0)
	{
		ImGui::Text("Depth Attachment:");
		ImGui::Image((void*)selectedBuffer->GetDepthAttachment(), imageSize, ImVec2(0, 1), ImVec2(1, 0));
	}

	ImGui::End();
}
