#include "GuiWidgets.h"

void GuiWidgets::DrawButton(const std::string& label, const std::function<void()>& onButtonClicked)
{
	ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
	labelSize.x += 15.0f;
	labelSize.y += 15.0f;

	if (ImGui::Button(label.c_str(), labelSize)) onButtonClicked();
}

void GuiWidgets::DrawFloatControl(const std::string& label, float& values, float min, float max, float steps, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	auto& labelSize = ImGui::CalcTextSize(label.c_str());

	ImGui::Text(label.c_str());
	ImGui::SameLine();

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, labelSize.x + 15.0);
	ImGui::NextColumn();

	ImGui::SetColumnWidth(1, columnWidth);
	ImGui::DragFloat("", &values, steps, min, max, "%.2f");

	ImGui::Columns(1);

	ImGui::PopID();
}

void GuiWidgets::DrawIntControl(const std::string& label, int& values, int min, int max, int resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	auto& labelSize = ImGui::CalcTextSize(label.c_str());

	ImGui::Text(label.c_str());
	ImGui::SameLine();

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, labelSize.x + 15.0);
	ImGui::NextColumn();

	ImGui::SetColumnWidth(1, columnWidth);
	ImGui::DragInt("", &values, min, max);

	ImGui::Columns(1);

	ImGui::PopID();
}

void GuiWidgets::DrawRGBColorPicker(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Dummy({ columnWidth , 1.0f });

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth / 3);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::SetColumnWidth(1, columnWidth);
	ImGui::ColorEdit3("", &values[0], 0.1f);
	ImGui::Dummy({ columnWidth , 1.0f });

	ImGui::Columns(1);

	ImGui::PopID();
}

void GuiWidgets::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth) {
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Text(label.c_str());
	ImGui::SameLine();

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("X", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

void GuiWidgets::DrawColorDragControl(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("R", buttonSize))
		values.x = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##R", &values.x, 0.1f, 0.0f, 1.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("G", buttonSize))
		values.y = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##G", &values.y, 0.1f, 0.0f, 1.0f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("B", buttonSize))
		values.z = resetValue;
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##B", &values.z, 0.1f, 0.0f, 1.0f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

void GuiWidgets::DrawSingleSelectDropdown(const char* title, std::vector<std::string> options, int currentSelected, std::function<void(int)> onItemSelected) {
	if (options.size() == 0) return;

	std::string currentSelectedString = options.at(currentSelected);

	ImGui::PushID(title);
	ImGui::Text(title);
	ImGui::SameLine();
	if (ImGui::BeginCombo("##", currentSelectedString.c_str()))
	{
		for (int i = 0; i < options.size(); i++)
		{
			bool isSelected = currentSelectedString == options[i];
			if (ImGui::Selectable(options[i].c_str(), isSelected))
			{
				currentSelectedString = options.at(i);
				onItemSelected(i);
			}

			if (isSelected) ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopID();
}

void GuiWidgets::DrawList(const char* title, std::vector<std::string> listItems, int currentSelected, std::function<void(int)> onItemSelected) {
	ImVec2 size = ImGui::GetContentRegionAvail();

	//if (ImGui::BeginListBox(title, ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	if (ImGui::BeginListBox(title, size))
	{
		for (int n = 0; n < listItems.size(); n++)
		{
			const bool is_selected = (currentSelected == n);
			if (ImGui::Selectable(listItems[n].c_str(), is_selected)) {
				currentSelected = n;
				onItemSelected(currentSelected);
			}

			// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndListBox();
	}
}

void GuiWidgets::DrawCheckBox(const char* title, bool* isChecked)
{
	ImGui::PushID(title);

	ImGui::Text(title);
	ImGui::SameLine();
	ImGui::Checkbox("##", isChecked);

	ImGui::PopID();
}

void GuiWidgets::DrawTextureImage(void* data, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGui::Image(data, size, uv0, uv1, tint_col, border_col);
}

void GuiWidgets::DrawButtonImage(const Ref<Moongoose::Texture2D>& imageNormal,
	const Ref<Moongoose::Texture2D>& imageHovered, const Ref<Moongoose::Texture2D>& imagePressed, ImU32 tintNormal,
	ImU32 tintHovered, ImU32 tintPressed, ImVec2 rectMin, ImVec2 rectMax)
{
	auto* drawList = ImGui::GetWindowDrawList();
	if (ImGui::IsItemActive())
	{
		drawList->AddImage(
			imagePressed->GetPointerToData(),
			rectMin, rectMax,
			ImVec2(0, 0), ImVec2(1, 1),
			tintPressed);
	}
	else if (ImGui::IsItemHovered())
	{
		drawList->AddImage(
			imageHovered->GetPointerToData(),
			rectMin, rectMax,
			ImVec2(0, 0), ImVec2(1, 1),
			tintHovered);
	}
	else
	{
		drawList->AddImage(
			imageNormal->GetPointerToData(),
			rectMin, rectMax,
			ImVec2(0, 0), ImVec2(1, 1),
			tintNormal);
	}
}

void GuiWidgets::ImageButtonWithText(const Moongoose::UUID id, const Ref<Moongoose::Texture2D>& icon, const std::string& text,
	ImVec2 buttonPos, ImVec2 buttonSize, const std::function<void(const Moongoose::UUID)>& onButtonClicked)
{
	ImGui::BeginGroup();
	ImGui::PushID(id);
	ImGui::SetCursorPos(buttonPos);

	if (ImGui::Button("", buttonSize))
	{
		onButtonClicked(id);
	}

	constexpr ImVec2 imgPadding = ImVec2{ 20.0f, 20.0f };
	const ImVec2 imgSize = { buttonSize.x - 4 * imgPadding.x, buttonSize.x - 4 * imgPadding.y };
	const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

	ImGui::SetCursorPos({ buttonPos.x + 2 * imgPadding.x, buttonPos.y + 2 * imgPadding.y });
	ImGui::Image(icon->GetPointerToData(), imgSize);

	ImGui::SetCursorPos({ buttonPos.x + (buttonSize.x - textSize.x) * 0.5f, buttonPos.y + buttonSize.y - 1.5f * imgPadding.y });
	ImGui::Text(text.c_str());

	ImGui::PopID();
	ImGui::EndGroup();
}
