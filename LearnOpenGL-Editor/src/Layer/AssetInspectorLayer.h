#pragma once
#include <LearnOpenGL.h>

#define CAST_TO(type, x) std::static_pointer_cast<type>(x)

class Asset;
class ImVec2;

class AssetInspectorLayer : public LearnOpenGL::Layer
{
public:
	AssetInspectorLayer() = default;
	~AssetInspectorLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(LearnOpenGL::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	void DrawMeshAssetGui(LearnOpenGL::AssetDeclaration& decl, const Ref<LearnOpenGL::Mesh>& mesh) const;
	void DrawTextureAssetGui(LearnOpenGL::AssetDeclaration& decl, const Ref<LearnOpenGL::Texture2D>& texture) const;
	void DrawMaterialValueInput(const Ref<LearnOpenGL::Material>& material, Ref<LearnOpenGL::Texture2D>& texture, const ImVec2 imgSize) const;
	void DrawMaterialAssetGui(LearnOpenGL::AssetDeclaration& decl, const Ref<LearnOpenGL::Material>& material) const;

	void RenameAsset(const Ref<LearnOpenGL::Asset>& selectedAsset, const char* nameArray) const;

	static void RenderImageTextButton(ImVec2 imageSize, const Ref<LearnOpenGL::Texture2D>& icon, const std::string& text);
private:
	LearnOpenGL::AssetManager* m_AssetManager;

};
