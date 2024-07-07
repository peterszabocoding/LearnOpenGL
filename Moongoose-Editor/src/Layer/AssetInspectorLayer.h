#pragma once
#include <Moongoose.h>

#define CAST_TO(type, x) std::static_pointer_cast<type>(x)

class Asset;
class ImVec2;

class AssetInspectorLayer : public Moongoose::Layer
{
public:
	AssetInspectorLayer() {};
	virtual ~AssetInspectorLayer() {};

	virtual void onAttach() override {};
	virtual void onDetach() override {};
	virtual void onUpdate(float deltaTime) override {};
	virtual void onEvent(Moongoose::Event& event) override {};
	virtual void onImGuiRender() override;

private:
	void DrawMeshAssetGUI(Moongoose::AssetDeclaration& decl, Ref<Moongoose::Mesh> mesh);
	void DrawTextureAssetGUI(Moongoose::AssetDeclaration& decl, Ref<Moongoose::Texture2D> texture);
	void DrawMaterialAssetGUI(Moongoose::AssetDeclaration& decl, Ref<Moongoose::Material> material);
	void RenderImageTextButton(ImVec2 imageSize, Ref<Moongoose::Texture2D> icon, std::string text);

};
