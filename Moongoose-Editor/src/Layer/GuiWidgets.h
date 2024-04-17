#pragma once

#include <string>
#include <cstring>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <functional>

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
};

