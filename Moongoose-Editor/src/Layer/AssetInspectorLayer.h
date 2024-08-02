#pragma once
#include <Moongoose.h>

#define CAST_TO(type, x) std::static_pointer_cast<type>(x)

class Asset;
class ImVec2;

class AssetInspectorLayer : public Moongoose::Layer
{
public:
	AssetInspectorLayer() = default;
	~AssetInspectorLayer() override = default;

	virtual void onAttach() override;
	virtual void onDetach() override {}
	virtual void onUpdate(float deltaTime) override {}
	virtual void onEvent(Moongoose::Event& event) override {}
	virtual void onImGuiRender() override;

private:
	void DrawMeshAssetGui(Moongoose::AssetDeclaration& decl, const Ref<Moongoose::Mesh>& mesh) const;
	void DrawTextureAssetGui(Moongoose::AssetDeclaration& decl, const Ref<Moongoose::Texture2D>& texture) const;
	void DrawMaterialAssetGui(Moongoose::AssetDeclaration& decl, const Ref<Moongoose::Material>& material) const;

	static void RenderImageTextButton(ImVec2 imageSize, const Ref<Moongoose::Texture2D>& icon, const std::string& text);
private:
	Moongoose::AssetManager* m_AssetManager;

};
