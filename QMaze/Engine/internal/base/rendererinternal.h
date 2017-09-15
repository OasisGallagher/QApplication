#include "surface.h"
#include "renderer.h"
#include "internal/base/objectinternal.h"

class RenderOption;

class RendererInternal : public IRenderer, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Renderer)

public:
	RendererInternal();
	~RendererInternal();

public:
	virtual void Render(Surface surface);

	virtual void AddMaterial(Material material) { materials_.push_back(material); }
	virtual Material GetMaterial(int index) { return materials_[index]; }
	virtual void SetMaterial(int index, Material value) { materials_[index] = value; }
	virtual int GetMaterialCount() const { return materials_.size(); }

	virtual void AddOption(RenderCapacity cap, RenderParameter parameter0, RenderParameter parameter1);

private:
	void DrawCall(Surface surface);
	void DrawMesh(Mesh mesh, Material material);

	void ClearRenderOptions();
	void BindRenderOptions();
	void UnbindRenderOptions();

private:
	std::vector<Material> materials_;
	std::vector<RenderOption*> options_;
};
