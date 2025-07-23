#pragma once

#include <string>
#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <functional>

#include "LearnOpenGL/Renderer/Texture.h"

#include "glm/glm.hpp"

class GuiWidgets {
private:
	GuiWidgets() {}
	~GuiWidgets() = default;

public:
	static void DrawButton(const std::string& label, const std::function<void()>& onButtonClicked);

	static void DrawFloatControl(const std::string& label, float& values, float min, float max, float steps = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawIntControl(const std::string& label, int& values, int min, int max, int resetValue, float columnWidth);

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawColorDragControl(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawRGBColorPicker(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);

	static void DrawSingleSelectDropdown(const char* title, std::vector<std::string> options, int currentSelected, std::function<void(int)> onItemSelected);

	static void DrawList(const char* title, std::vector<std::string> listItems, int currentSelected, std::function<void(int)> onItemSelected);

	static void DrawCheckBox(const char* title, bool* isChecked);

	static void DrawTextureImage(void* data, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& imageNormal, const Ref<LearnOpenGL::Texture2D>& imageHovered, const Ref<LearnOpenGL::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax);

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& imageNormal, const Ref<LearnOpenGL::Texture2D>& imageHovered, const Ref<LearnOpenGL::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle)
	{
		DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
	};

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImVec2 rectMin, ImVec2 rectMax)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectMin, rectMax);
	};

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed,
		ImRect rectangle)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, rectangle.Min, rectangle.Max);
	};

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& imageNormal, const Ref<LearnOpenGL::Texture2D>& imageHovered, const Ref<LearnOpenGL::Texture2D>& imagePressed,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
	{
		DrawButtonImage(imageNormal, imageHovered, imagePressed, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	};

	static void DrawButtonImage(const Ref<LearnOpenGL::Texture2D>& image,
		ImU32 tintNormal, ImU32 tintHovered, ImU32 tintPressed)
	{
		DrawButtonImage(image, image, image, tintNormal, tintHovered, tintPressed, ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
	};

	static void ImageButtonWithText(const LearnOpenGL::UUID id, const Ref<LearnOpenGL::Texture2D>& icon, const std::string& text,
		ImVec2 buttonPos, ImVec2 buttonSize, const std::function<void(const LearnOpenGL::UUID)>& onButtonClicked);
};

