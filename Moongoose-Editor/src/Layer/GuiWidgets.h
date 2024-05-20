#pragma once

#include <string>
#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <functional>

#include "Moongoose/Renderer/Texture.h"

#include "glm/glm.hpp"

class GuiWidgets {
private:
	GuiWidgets() {}
	~GuiWidgets() {}

public:
	static void DrawFloatControl(const std::string& label, float& values, float min, float max, float steps = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawColorDragControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawRGBColorPicker(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawSingleSelectDropdown(const char* title, std::vector<std::string> options, int currentSelected, std::function<void(int)> onItemSelected);

	static void DrawList(const char* title, std::vector<std::string> listItems, int currentSelected, std::function<void(int)> onItemSelected);

	static void DrawCheckBox(const char* title, bool* isChecked);

	static void DrawTextureImage(void* data, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& imageNormal, const Ref<Moongoose::Texture2D>& imageHovered, const Ref<Moongoose::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax)
	{
		auto* drawList = ImGui::GetWindowDrawList();
		if (ImGui::IsItemActive())
			drawList->AddImage(
				imagePressed->getPointerToData(), 
				rectMin, rectMax, 
				ImVec2(0, 0), ImVec2(1, 1), 
				tintPressed);
		else if (ImGui::IsItemHovered())
			drawList->AddImage(
				imageHovered->getPointerToData(), 
				rectMin, rectMax, 
				ImVec2(0, 0), ImVec2(1, 1),
				tintHovered);
		else
			drawList->AddImage(
				imageNormal->getPointerToData(),
				rectMin, rectMax, 
				ImVec2(0, 0), ImVec2(1, 1), 
				tintNormal);
	};

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& imageNormal, const Ref<Moongoose::Texture2D>& imageHovered, const Ref<Moongoose::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle)
	{
		DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
	};

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectMin, rectMax);
	};

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
	};

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& imageNormal, const Ref<Moongoose::Texture2D>& imageHovered, const Ref<Moongoose::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
	{
		DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	};

	static void DrawButtonImage(const Ref<Moongoose::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	};

	static void ImageButtonWithText(const Ref<Moongoose::Texture2D>& icon, const std::string& text, ImVec2 buttonPos, ImVec2 buttonSize)
	{
		ImGui::PushID(text.c_str());

		ImGui::SetCursorPos(buttonPos);
		ImGui::Button("", buttonSize);

		ImVec2 imgPadding = ImVec2{ 20.0f, 20.0f };
		ImVec2 imgSize = { buttonSize.x - 4 * imgPadding.x, buttonSize.x - 4 * imgPadding.y };

		ImGui::SetCursorPos({ buttonPos.x + 2 * imgPadding.x, buttonPos.y + 2 * imgPadding.y });
		ImGui::Image(icon->getPointerToData(), imgSize);

		ImVec2 text_size = ImGui::CalcTextSize(text.c_str());
		ImGui::SetCursorPos({ buttonPos.x + (buttonSize.x - text_size.x) * 0.5f, buttonPos.y + buttonSize.y - 1.5f * imgPadding.y });
		ImGui::Text(text.c_str());

		ImGui::PopID();
	}
};

